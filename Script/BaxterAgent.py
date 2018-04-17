import sys
import math
import baxter_interface
import rospy
from rapidjson import loads,dumps 
from socketClient import Client
from socketServer import Server
import tf
import tf2_ros
import tf2_msgs.msg
import geometry_msgs.msg
import numpy as np
from PIL import Image
from io import BytesIO
from tuck_arms import Tuck
import time
import random
import copy
# from dqn_pixel import DQNagent, plot_losses, plot_durations
from multiprocessing.pool import ThreadPool
import matplotlib
import matplotlib.pyplot as plt
from collections import deque
from itertools import count
import subprocess
import signal
import os
import logging

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.autograd import Variable
import torchvision.transforms as T

from agent import *
from component import *
from utils import *
from component.bench import Monitor
from skimage import color, transform


resize = T.Compose([T.ToPILImage(),
					T.Resize(40, interpolation=Image.CUBIC),
					T.ToTensor()])

baxter_mesh_list = [	
			'head', 'torso', 'left_arm_mount',\
			'left_upper_shoulder', 'left_lower_shoulder', 'left_upper_elbow',\
			'left_lower_elbow', 'left_upper_forearm', 'left_lower_forearm',\
			'left_wrist', 'right_arm_mount','right_upper_shoulder', \
			'right_lower_shoulder', 'right_upper_elbow','right_lower_elbow',\
			'right_upper_forearm', 'right_lower_forearm', 'right_wrist'\
		]

def quat_from_euler(pose):
	pose = copy.deepcopy(pose)
	theta = pose['Theta']
	q = tf.transformations.quaternion_from_euler(0, 0, theta)
	new_rot = {'X':q[0], 'Y':q[1], 'Z':q[2], 'W':q[3]}
	quat = {}
	quat['Loc'] = pose['Loc']
	quat['Rot'] = new_rot
	return quat

class baxter_ue_publisher(object):
	def __init__(self, pose):
		self.pose = copy.copy(pose)
		self.rate = 500
		self.tfBuffer = tf2_ros.Buffer(rospy.Duration(5.0))
		self.listener = tf2_ros.TransformListener(self.tfBuffer)
		self.br = tf2_ros.TransformBroadcaster()
		self.start = False

	# @profile
	def pub_ue_pose(self, event):
		tf_list = []

		t = geometry_msgs.msg.TransformStamped()
		t.header.stamp = rospy.Time.now()
		t.header.frame_id = "world"
		t.child_frame_id = "ue_base"
		t.transform.translation.x = self.pose['Loc']['X']
		t.transform.translation.y = self.pose['Loc']['Y']
		t.transform.translation.z = self.pose['Loc']['Z']
		t.transform.rotation.x = self.pose['Rot']['X']
		t.transform.rotation.y = self.pose['Rot']['Y']
		t.transform.rotation.z = self.pose['Rot']['Z']
		t.transform.rotation.w = self.pose['Rot']['W']
		tf_list.append(t)


		for mesh in baxter_mesh_list:
			try:
				trans = self.tfBuffer.lookup_transform('base', mesh, rospy.Time())
			except Exception as e:
				print "pub look up failed",e
				return 
			trans.header.stamp = t.header.stamp
			trans.header.frame_id = "ue_base"
			trans.child_frame_id = "ue_"+mesh
			trans.transform.translation.x *= 100
			trans.transform.translation.y *= -100
			trans.transform.translation.z *= 100
			trans.transform.rotation.y *= -1
			trans.transform.rotation.w *= -1
			tf_list.append(trans)

		self.br.sendTransform(tf_list)
		# q = tf.transformations.quaternion_from_euler(msg['Rot']['X'], msg['Rot']['Y'], msg['Rot']['Z'])
		# t.transform.rotation.x = q[0]
		# t.transform.rotation.y = q[1]
		# t.transform.rotation.z = q[2]
		# t.transform.rotation.w = q[3]

	def change_loc(self, new_loc):
		self.pose['Loc'] = new_loc

	def change_rot(self, new_rot):
		self.pose['Rot'] = new_rot

	def start_pub(self):
		self.timer = rospy.Timer(rospy.Duration(1.0/self.rate), self.pub_ue_pose)
		self.start = True

	def print_pose(self):
		print(self.pose)

	def stop(self):
		self.timer.shutdown()
		self.start = False

	def is_start(self):
		return self.start


class BaxterAgent(object):
	def __init__(
		self, start_pose={'Loc':{"X":-1010, "Y":66, "Z":130}, 'Theta':1/2.0*(math.pi)}, \
		client_endpoint=("127.0.0.1", 10120), \
		server_endpoint=("127.0.0.1", 10121)):
		rospy.init_node('BaxterAgent')
		self.tfBuffer = tf2_ros.Buffer(rospy.Duration(5.0))
		self.listener = tf2_ros.TransformListener(self.tfBuffer)
		self.base_pub = baxter_ue_publisher(quat_from_euler(start_pose))
		# print(quat_from_euler(start_pose))
		self.client_endpoint = client_endpoint
		self.server_endpoint = server_endpoint
		self.client = Client(self.client_endpoint)
		self.server = Server(self.server_endpoint)
		self.frame = 0
		self.start_pose = copy.deepcopy(start_pose)
		self.left_arm = baxter_interface.Limb('left')
		self.right_arm = baxter_interface.Limb('right')
		self.left_names = self.left_arm.joint_names()
		self.right_names = self.right_arm.joint_names()
		self.current_state = {
		"left_arm": map(self.left_arm.joint_angle, self.left_names),
		"right_arm": map(self.right_arm.joint_angle, self.right_names),
		"baxter_pose": copy.deepcopy(self.start_pose)
		}
		# TO DO: set state limit!
		self.limit_min = [-1.7016, -2.147, -3.0541, -0.05, -3.059, -1.5707, -3.059]
		self.limit_max = [1.7016, 1.047, 3.0541, 2.618, 3.059, 2.094, 3.059]
		self.tuck = Tuck(False)
		self.pool = ThreadPool(processes=4)
		self.retry_num = 20
		self.baxter_mesh_list = [	
			'base', 'head', 'torso', 'left_arm_mount',\
			'left_upper_shoulder', 'left_lower_shoulder', 'left_upper_elbow',\
			'left_lower_elbow', 'left_upper_forearm', 'left_lower_forearm',\
			'left_wrist', 'right_arm_mount','right_upper_shoulder', \
			'right_lower_shoulder', 'right_upper_elbow','right_lower_elbow',\
			'right_upper_forearm', 'right_lower_forearm', 'right_wrist'\
		]
		self.FNULL = open(os.devnull, 'w')
		self.execCmd = "/home/binroot/vr_pkg/LinuxNoEditor/VRInteractPlatform/Binaries/Linux/VRInteractPlatform"
		self.p = None
		# self.p = subprocess.Popen("exec "+self.execCmd, shell=True,
		# 	stdin=None, stdout=self.FNULL, stderr=None, close_fds=True)

	def start(self):
		while True:
			try:
				self.tuck.supervised_tuck()
				break
			except Exception as e:
				print e
				print "Tuck Failed"
				# time.sleep(1.0)
				pass

		if not self.base_pub.is_start():
			self.base_pub.start_pub()
		self.frame = 1
		# self.p = subprocess.Popen("exec "+self.execCmd, shell=True,
		# 	stdin=None, stdout=self.FNULL, stderr=None, close_fds=True)
		# subprocess.PIPE
		self.server.listen()
		self.client.connect()
		data_frame = self.send_tf()
		i = 0
		while data_frame == None and i < self.retry_num and not rospy.is_shutdown():
			print("sending tf")
			data_frame = self.send_tf()
			i += 1
		return data_frame


	def reset(self):
		# if self.p:
		# 	self.p.kill()
		self.send_tf(reset=True)
		self.stop()
		# self.base_pub.stop()
		self.server = Server(self.server_endpoint)
		self.client = Client(self.client_endpoint)	
		return self.start()
		# self.send_tf(reset=True)
		# self.stop()
		# self.base_pub.stop()
		# self.__init__(
		# 		start_pose=self.start_pose, \
		# 		client_endpoint=self.client_endpoint, \
		# 		server_endpoint=self.server_endpoint
		# 		)
		# first_data = self.start()
		# while first_data is None:
		# 	self.client.connect()
		# 	self.send_tf(reset=True)
		# 	self.stop()
		# 	self.__init__(
		# 		start_pose=self.start_pose, \
		# 		client_endpoint=self.client_endpoint, \
		# 		server_endpoint=self.server_endpoint
		# 		)
		# 	first_data = self.start()
		# return first_data

	# send transform to ue4, return data
	# @profile
	def send_tf(self, reset=False):
		# print("sending tf")
		data = {}
		data['RobotName'] = 'BaxterRobot'
		data['Anim'] = []
		data['reset'] = reset
		t1 = time.time()
		if not reset:
			i = 0
			for mesh in self.baxter_mesh_list:	
				try:
					t = self.tfBuffer.lookup_transform('world', 'ue_'+mesh, rospy.Time())
				except Exception as e:
					print "look up failed",e
					return None
				data['Anim'].append({})
				data['Anim'][i]['MeshPose'] = \
				{'Loc':{"X":t.transform.translation.x, "Y":t.transform.translation.y, "Z":t.transform.translation.z},\
				 'Rot':{"X":t.transform.rotation.x, "Y":t.transform.rotation.y, "Z":t.transform.rotation.z, "W":t.transform.rotation.w}}
				data['Anim'][i]['MeshName'] = mesh
				i += 1
		# print("look up time", time.time()-t1)
		msg = dumps(data)
		msg = msg+"\n"
		self.client.send(msg.encode())
		# print(data)
		if not reset:
			return self.receive_msg()

	def receive_msg(self):
		try:
			data_frame = {}
			depth_head = self.server.getBuffer()
			# print(depth_head)
			assert(depth_head == "Depth")
			depth_data = np.load(BytesIO(self.server.getBuffer()))
			
			rgb_head = self.server.getBuffer()
			assert(rgb_head == "RGB")
			rgb_data = np.load(BytesIO(self.server.getBuffer()))
			# rgb_image = Image.fromarray(rgb_data)
			# rgb_image.show()
			# rgb_image.close()

			humandata_head = self.server.getBuffer()
			assert(humandata_head == "HumanData")
			a = self.server.getBuffer()
			if a:
				human_data = loads(a)
			else:
				human_data = None

			statedata_head = self.server.getBuffer()
			assert(statedata_head == "StateData")
			a = self.server.getBuffer()
			if a:
				state_data = a.split()
				state_data = np.array([float(i) for i in state_data])
				# print(state_data)
			else:
				state_data = None

			reward_head = self.server.getBuffer()
			assert(reward_head == "Reward")
			reward = float(self.server.getBuffer())

			done_head = self.server.getBuffer()
			assert(done_head == "Done")
			done = (self.server.getBuffer() == "1")

			data_frame["frame"] = self.frame
			data_frame["depth"] = depth_data
			data_frame["rgb"] = rgb_data
			data_frame["human"] = human_data
			data_frame["state"] = state_data
			data_frame["reward"] = reward
			data_frame["done"] = done
			return data_frame
		except Exception:
			return None
		# data_frame = {}
		# depth_head = self.server.getBuffer()
		# # print(depth_head)
		# assert(depth_head == "Depth")
		# depth_data = np.load(BytesIO(self.server.getBuffer()))
		
		# rgb_head = self.server.getBuffer()
		# assert(rgb_head == "RGB")
		# rgb_data = np.load(BytesIO(self.server.getBuffer()))
		# # rgb_image = Image.fromarray(rgb_data)
		# # rgb_image.show()
		# # rgb_image.close()

		# humandata_head = self.server.getBuffer()
		# assert(humandata_head == "HumanData")
		# a = self.server.getBuffer()
		# if a:
		# 	human_data = loads(a)
		# else:
		# 	human_data = None

		# statedata_head = self.server.getBuffer()
		# assert(statedata_head == "StateData")
		# a = self.server.getBuffer()
		# if a:
		# 	state_data = a.split()
		# 	state_data = np.array([float(i) for i in state_data])
		# 	print(state_data)
		# else:
		# 	state_data = None

		# reward_head = self.server.getBuffer()
		# assert(reward_head == "Reward")
		# reward = float(self.server.getBuffer())

		# done_head = self.server.getBuffer()
		# assert(done_head == "Done")
		# done = (self.server.getBuffer() == "1")

		# data_frame["frame"] = self.frame
		# data_frame["depth"] = depth_data
		# data_frame["rgb"] = rgb_data
		# data_frame["human"] = human_data
		# data_frame["state"] = state_data
		# data_frame["reward"] = reward
		# data_frame["done"] = done
		# return data_frame



	def set_arm_joint(self, arm="left_arm", joint_num=0, delta=0.1, scale=1):
		if arm == "left_arm":
			limb = self.left_arm
			jn = self.left_names
		if arm == "right_arm":
			limb = self.right_arm
			jn = self.right_names

		current_position = limb.joint_angle(jn[joint_num])
		joint_command = {jn[joint_num]: current_position + scale*delta}
		limb.set_joint_positions(joint_command)
		self.current_state[arm][joint_num] = limb.joint_angle(jn[joint_num])

	def move_X(self, delta=20, scale=1):
		current_loc = self.current_state['baxter_pose']['Loc']
		current_loc['X'] += scale*delta
		self.base_pub.change_loc(current_loc)
		self.current_state['baxter_pose']['Loc']['X'] = current_loc['X']

	def move_Y(self, delta=20, scale=1):
		current_loc = self.current_state['baxter_pose']['Loc']
		current_loc['Y'] += scale*delta
		self.base_pub.change_loc(current_loc)
		self.current_state['baxter_pose']['Loc']['Y'] = current_loc['Y']

	def rotate(self, delta=1/2.0*(math.pi),scale=1):
		current_pose = self.current_state['baxter_pose']
		current_pose['Theta'] += scale*delta
		if current_pose['Theta'] > math.pi:
			current_pose['Theta'] -= 2*math.pi
		if current_pose['Theta'] < -math.pi:
			current_pose['Theta'] += 2*math.pi
		new_pose = quat_from_euler(current_pose)
		self.base_pub.change_rot(new_pose['Rot'])
		self.current_state['baxter_pose'] = current_pose

	# @profile
	def step(self, action):
		# action dict
		# {
		# "left_arm": [0, 0, 0, 0, 0, 0, 0],
		# "right_arm": [0, 0, 0, 0, 0, 1, 0],
		# "move_X": 0, 
		# "move_Y": 0,
		# "rotation": 0
		# }
		self.rotate(scale=action['rotation'])
		self.move_X(scale=action['move_X'])
		self.move_Y(scale=action['move_Y'])	

		for i in range(7):
			if action['left_arm'][i] != 0:
				self.set_arm_joint(arm="left_arm", joint_num=i, scale=action['left_arm'][i])
				break

		for i in range(7):		
			if action['right_arm'][i] != 0:
				self.set_arm_joint(arm="right_arm", joint_num=i, scale=action['right_arm'][i])
				break

		data_frame = None
		i = 0
		self.base_pub.pub_ue_pose(None)

		# TO DO!
		# time.sleep(0.04)

		while data_frame == None and i < self.retry_num and not rospy.is_shutdown():
			data_frame = self.send_tf()
			# print("sending!")
			i += 1

		self.frame += 1

		return data_frame

	# TO DO 
	def thread_step(self, action):
		asyn_res = self.pool.apply_async(self.step, (action))
		return asyn_res.get()

	def stop(self):
		self.server.stop()
		self.client.disconnect()
		if self.p:
			self.p.kill()
			del self.p
			self.p = None

		if self.server:
			del self.server
			self.server = None

		if self.client:
			del self.client
			self.client = None



# if __name__ == "__main__":
# 	st_time = time.time()

# 	start_pose = {'Loc':{"X":-1006, "Y":-10, "Z":130}, 'Theta':-1/2.0*math.pi}
# 	env = BaxterAgent(start_pose)
# 	data = env.start()
# 	# rgb_image = Image.fromarray(data['rgb'])
# 	# rgb_image.show()

# 	action = {
# 		"left_arm": [0, 0, 0, 0, 0, 0, 0],
# 		"right_arm": [0, 0, 0, 0, 0, 0, 0],
# 		"move_X": 0, 
# 		"move_Y": 0,
# 		"rotation": 0
# 		}


# 	max_steps = 50
# 	i = 0

# 	agent = DQNagent()
# 	optimizer = optim.RMSprop(filter(lambda p: p.requires_grad, agent.model.parameters()), lr=1e-3)
# 	losses = []
# 	mean_rewards = []

# 	epochs = 1000

# 	for epoch in range(epochs):
# 		st_time = time.time()
# 		steps = 0
# 		done = False
# 		data = env.reset()
# 		# last_screen = resize(torch.FloatTensor(data['rgb']).permute(2,0,1)).unsqueeze(0).cuda()
# 		last_screen = resize(torch.FloatTensor(data['rgb']).permute(2,0,1)).unsqueeze(0)

# 		# current_screen = resize(torch.FloatTensor(data['rgb']).permute(2,0,1)).unsqueeze(0).cuda()
# 		current_screen = resize(torch.FloatTensor(data['rgb']).permute(2,0,1)).unsqueeze(0)

# 		state = current_screen - last_screen
# 		print("epoch", epoch)
# 		rewards = []
# 		while steps < max_steps:
# 			# st_time = time.time()
# 			steps = data["frame"]
# 			# print(steps)
# 			action_idx = agent.act(state)
# 			action["right_arm"] = [0, 0, 0, 0, 0, 0, 0]
# 			action["right_arm"][action_idx%7] = 2*(action_idx//7)-1
# 			# print(action["right_arm"])
# 			data = env.step(action)
# 			# t1 = time.time()
# 			last_screen = current_screen
# 			# current_screen = resize(torch.FloatTensor(data['rgb']).permute(2,0,1)).unsqueeze(0).cuda()
# 			current_screen = resize(torch.FloatTensor(data['rgb']).permute(2,0,1)).unsqueeze(0)
# 			done = data['done']
# 			# print("done", done)
# 			reward = data['reward']
# 			# print("reward", reward)
# 			rewards.append(reward)
# 			if not done:
# 				next_state = current_screen - last_screen
# 			else:
# 				next_state = None
# 			agent.remember(state, action_idx, next_state, reward)
# 			state = next_state
# 			loss = agent.replay(16, optimizer)
# 			# t2 = time.time()
# 			# print(t1-st_time)
# 			# print(t2-t1)
# 			if done or steps == max_steps-1:
# 				print "total time", time.time()-st_time
# 				losses.append(loss)
# 				plot_losses(losses)
# 				mean_rewards.append(np.mean(rewards))
# 				print("mean reward", np.mean(rewards))
# 				plot_durations(mean_rewards)
# 				break
# 				# print("Episode finished after {} timesteps".format(steps))
# 	# i = 0				
# 	# while not rospy.is_shutdown():
# 	# 	try:
# 	# 		data = env.step(action)
# 	# 		print(i)
# 	# 		i += 1
# 	# 		# print(data["frame"])
# 	# 	except Exception:
# 	# 		break

# 	# rgb_image = Image.fromarray(data['rgb'])
# 	# rgb_image.show()
	
# 	env.stop()
# 	rospy.spin()

class BasicVRTask:
	def __init__(self, max_steps=sys.maxsize, frame_size=84, frame_skip=1):
		self.steps = 0
		self.max_steps = max_steps
		self.frame_size = frame_size
		self.frame_skip = frame_skip

	def reset(self):
		self.steps = 0
		data = self.env.reset()
		# state = transform.resize(color.rgb2gray(data['rgb']), (self.frame_size, self.frame_size), mode='constant')
		# state = np.moveaxis(state, -1, 0)
		# state = np.stack([state for i in range(self.frame_skip)])
		state = data["state"]
		return state

	def normalize_state(self, state):
		return state

	def step(self, a):
		action = {
			"left_arm": [0, 0, 0, 0, 0, 0, 0],
			"right_arm": [0, 0, 0, 0, 0, 0, 0],
			"move_X": 0, 
			"move_Y": 0,
			"rotation": 0
			}
		action["right_arm"] = [0, 0, 0, 0, 0, 0, 0]
		action["right_arm"][a%7] = 2*(a//7)-1

		# next_states = []
		# for i in range(self.frame_skip):
		# 	data = self.env.step(action)
		# 	next_state = transform.resize(color.rgb2gray(data['rgb']), (self.frame_size, self.frame_size), mode='constant')
		# 	next_state = np.moveaxis(next_state, -1, 0)
		# 	next_states.append(next_state)

		# next_states = np.stack(next_states)

		data = self.env.step(action)
		next_states = data["state"]

		reward = data['reward']
		done = data['done']
		info = None
		self.steps += 1
		done = (done or self.steps >= self.max_steps)
		return next_states, reward, done, info
	
	def set_monitor(self, filename):
		self.env = Monitor(self.env, filename)


class PixelVR(BasicVRTask):
	success_threshold = 0
	def __init__(self, name="open_cabinet", normalized_state=True, max_steps=30, frame_size=84, frame_skip=1):
		BasicVRTask.__init__(self, max_steps, frame_size, frame_skip)
		self.normalized_state = normalized_state
		self.action_dim = 14
		self.name = name
		self.start_pose = {'Loc':{"X":-1000, "Y":-10, "Z":130}, 'Theta':-1/2.0*math.pi}
		self.env = BaxterAgent(self.start_pose)

	def normalize_state(self, state):
		return np.asarray(state) / 255.0

	def start(self):
		self.env.start()

def dqn_pixel_vr():
	config = Config()
	config.history_length = 1
	config.task_fn = lambda: PixelVR(normalized_state=False, max_steps=50, frame_skip=config.history_length)
	action_dim = config.task_fn().action_dim
	config.optimizer_fn = lambda params: torch.optim.RMSprop(params, lr=0.00025, alpha=0.95, eps=0.01)
	# config.network_fn = lambda: NatureConvNet(config.history_length, action_dim, gpu=0)
	# config.network_fn = lambda: DuelingNatureConvNet(config.history_length, action_dim)
	config.network_fn = lambda: FCNet([6, 256, 256, action_dim])
	config.policy_fn = lambda: GreedyPolicy(epsilon=1.0, final_step=10000, min_epsilon=0.1)
	config.replay_fn = lambda: Replay(memory_size=10000, batch_size=32, dtype=np.uint8)
	config.reward_shift_fn = lambda r: r
	config.discount = 0.99
	config.target_network_update_freq = 10000
	config.max_episode_length = 0
	config.exploration_steps= 0
	config.logger = Logger('./log', logger)
	config.test_interval = 50
	config.test_repetitions = 1
	config.episode_limit = 10000000
	config.double_q = True
	# config.double_q = False
	run_episodes(DQNAgent(config))


def a2c_pixel_atari():
	config = Config()
	config.num_workers = 1
	config.task_fn = lambda: PixelVR(normalized_state=False, max_steps=30, frame_size=84)
	# config.task_fn = lambda: ParallelizedTask(task_fn, config.num_workers)
	config.optimizer_fn = lambda params: torch.optim.RMSprop(params, lr=0.0007)
	config.network_fn = lambda: NatureActorCriticConvNet(3, config.task_fn().action_dim, gpu=0)
	config.reward_shift_fn = lambda r: np.sign(r)
	config.policy_fn = SamplePolicy
	config.discount = 0.99
	config.use_gae = False
	config.gae_tau = 0.97
	config.entropy_weight = 0.01
	config.rollout_length = 50
	config.test_interval = 0
	config.iteration_log_interval = 100
	config.gradient_clip = 0.5
	config.logger = Logger('./log', logger, skip=True)
	run_iterations(A2CAgent(config))

if __name__ == "__main__":
	logger.setLevel(logging.INFO)
	logger.setLevel(logging.DEBUG)
	# a2c_pixel_atari()
	dqn_pixel_vr()
	# start_pose = {'Loc':{"X":-1006, "Y":-15, "Z":130}, 'Theta':-1/2.0*math.pi}
	# env = BaxterAgent(start_pose)
	# env.start()
	# action = {
	# 		"left_arm": [0, 0, 0, 0, 0, 0, 0],
	# 		"right_arm": [0, 0, 0, 0, 0, 0, 0],
	# 		"move_X": 0, 
	# 		"move_Y": 0,
	# 		"rotation": 0
	# 		}
	# for i in range(50):
	# 	t = time.time()
	# 	env.reset()
	# 	t1 = time.time()
	# 	print(t1-t)
	# 	env.step(action)
	# 	print(time.time() - t1)
	# env.stop()
	# env.clean()
	# del env

	rospy.spin()


