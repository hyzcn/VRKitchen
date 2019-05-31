#!/usr/bin/python2.7
import sys, math, copy, time, subprocess, os
import numpy as np
from io import BytesIO
from rapidjson import loads,dumps 
from socketClient import Client
from socketServer import Server
from PIL import Image
import logging
from skimage import color, transform

SPEED = 20

class DiscreteAgent(object):
	def __init__(
			self, init_state,\
			client_endpoint=("127.0.0.1", 10120), \
			server_endpoint=("127.0.0.1", 10121)):
		self.client_endpoint = client_endpoint
		self.server_endpoint = server_endpoint
		self.client = Client(self.client_endpoint)
		self.server = Server(self.server_endpoint)
		self.retry_num = 50
		self.FNULL = open(os.devnull, 'w')
		self.frame = 0
		self.epochs = 0
		self.data = {}
		self.restart_ue = 100
		self.init_state = copy.deepcopy(init_state)
		self.execCmd = "../Binaries/ToolUse/VRInteractPlatform/Binaries/Linux/VRInteractPlatform"
		self.crouch = False
		self.p = subprocess.Popen("exec "+self.execCmd, shell=True,\
			stdin=None, stdout=self.FNULL, stderr=None, close_fds=True)

	def start(self):
		self.state = copy.deepcopy(self.init_state)
		self.state_old = copy.deepcopy(self.init_state)
		self.frame = 1
		self.server.listen()
		self.client.connect()
		data_frame = self.send_tf()
		i = 0
		while data_frame == None and i < self.retry_num:
			print("sending tf again")
			data_frame = self.send_tf()
			i += 1

		self.data = data_frame

		if data_frame == None:
			self.reset()
		return data_frame

	def reset(self):
		# if self.epochs % self.restart_ue == 0:
		# 	if self.p:
		# 		self.p.kill()
		# 	self.p = subprocess.Popen("exec "+self.execCmd, shell=True,
		# 	stdin=None, stdout=self.FNULL, stderr=None, close_fds=True)
		# else:
		# 	self.send_tf(reset=True)

		self.send_tf(reset=True)

		self.stop()
		self.server = Server(self.server_endpoint)
		self.client = Client(self.client_endpoint)	
		self.epochs += 1
		return self.start()

	def stop(self):
		self.server.stop()
		self.client.disconnect()
		# if self.p:
		# 	self.p.kill()
		# 	del self.p
		# 	self.p = None

		if self.server:
			del self.server
			self.server = None

		if self.client:
			del self.client
			self.client = None

	def send_tf(self, Name="Agent1", reset=False, world=False, anim_speed=0.0):
		data = {}
		data['AgentName'] = Name
		data['State'] = {}
		data['reset'] = reset

		if not reset:
			data['State']["ActorPose"] = \
				copy.deepcopy({
					"ActorLoc": self.state["Actor"]["Loc"],\
					"ActorRot": self.state["Actor"]["Rot"],\
					"CurrentSpeed": anim_speed
				})

			data['State']['HeadPose'] = {}
			data['State']['crouch'] = {}
			data['State']['HeadPose']["HeadWorldTransform"] = \
				copy.deepcopy({
					"Rot": self.state["Actor"]["Rot"],\
					"Trsl": self.state["Actor"]["Loc"],\
					"Scale": {"X":1.0,"Y":1.0,"Z":1.0}
				})

			data['State']['HeadPose']["HeadWorldTransform"]["Rot"]["Pitch"] = \
				copy.deepcopy(self.state["Head"]["Rot"]["Pitch"])

			data['State']['HeadPose']["HeadWorldTransform"]["Trsl"]["Z"] += 180

			data['State']["LeftHandGrab"] = \
				copy.deepcopy({
					"LeftGrab": self.state["LeftHand"]['Grab'],
					"LeftRelease": self.state["LeftHand"]["Release"],
					"ActorName": self.state["LeftHand"]["ActorName"],
					"CompName": self.state["LeftHand"]["CompName"]
				})

			data['State']["LeftHandPose"] = \
				copy.deepcopy({
					"LeftHandWorldPos": self.state["LeftHand"]['Loc'],
					"LeftHandWorldRot": self.state["LeftHand"]['Rot']
				})

			data['State']["RightHandGrab"] = \
				copy.deepcopy({
					"RightGrab": self.state["RightHand"]['Grab'],
					"RightRelease": self.state["RightHand"]['Release'],
					"ActorName": self.state["RightHand"]["ActorName"],
					"CompName": self.state["RightHand"]["CompName"]
				})

			data['State']["RightHandPose"] = \
				copy.deepcopy({
					"RightHandWorldPos": self.state["RightHand"]['Loc'],
					"RightHandWorldRot": self.state["RightHand"]['Rot']
				})

			x = data['State']["LeftHandPose"]["LeftHandWorldPos"]["X"]
			y = data['State']["LeftHandPose"]["LeftHandWorldPos"]["Y"]
			z = data['State']["LeftHandPose"]["LeftHandWorldPos"]["Z"]
			HandTheta = data['State']["LeftHandPose"]["LeftHandWorldRot"]["Yaw"]
			x_new, y_new, z_new, hand_new = self.RelToWorld(x, y ,z, HandTheta)
			data['State']["LeftHandPose"]["LeftHandWorldRot"]["Yaw"] = hand_new
			data['State']["LeftHandPose"]["LeftHandWorldPos"]["X"] = x_new
			data['State']["LeftHandPose"]["LeftHandWorldPos"]["Y"] = y_new
			data['State']["LeftHandPose"]["LeftHandWorldPos"]["Z"] = z_new

			x = data['State']["RightHandPose"]["RightHandWorldPos"]["X"]
			y = data['State']["RightHandPose"]["RightHandWorldPos"]["Y"]
			z = data['State']["RightHandPose"]["RightHandWorldPos"]["Z"]
			HandTheta = data['State']["RightHandPose"]["RightHandWorldRot"]["Yaw"]
			x_new, y_new, z_new, hand_new = self.RelToWorld(x, y ,z, HandTheta)
			data['State']["RightHandPose"]["RightHandWorldRot"]["Yaw"] = hand_new
			data['State']["RightHandPose"]["RightHandWorldPos"]["X"] = x_new
			data['State']["RightHandPose"]["RightHandWorldPos"]["Y"] = y_new
			data['State']["RightHandPose"]["RightHandWorldPos"]["Z"] = z_new
			data['State']["crouch"]["crouch"] = self.crouch

			data['State']['depth'] = self.state['depth']
			data['State']['rgb'] = self.state['rgb']
			data['State']['mask'] = self.state['mask']
			
			if world:
				data['State']["RightHandPose"]["RightHandWorldPos"] = self.state["RightHand"]["WorldLoc"]
				data['State']["LeftHandPose"]["LeftHandWorldPos"] = self.state["LeftHand"]["WorldLoc"]

				x,y,z = self.WorldToRel(self.state["RightHand"]["WorldLoc"]["X"], \
				self.state["RightHand"]["WorldLoc"]["Y"], self.state["RightHand"]["WorldLoc"]["Z"])
				self.state["RightHand"]["Loc"]["X"] = x
				self.state["RightHand"]["Loc"]["Y"] = y
				self.state["RightHand"]["Loc"]["Z"] = z

				x,y,z = self.WorldToRel(self.state["LeftHand"]["WorldLoc"]["X"], \
				self.state["LeftHand"]["WorldLoc"]["Y"], self.state["LeftHand"]["WorldLoc"]["Z"])
				self.state["LeftHand"]["Loc"]["X"] = x
				self.state["LeftHand"]["Loc"]["Y"] = y
				self.state["LeftHand"]["Loc"]["Z"] = z

			else:
				self.state["RightHand"]["WorldLoc"] = data['State']["RightHandPose"]["RightHandWorldPos"]
				self.state["LeftHand"]["WorldLoc"] = data['State']["LeftHandPose"]["LeftHandWorldPos"]

			# print self.state
		msg = dumps(data)
		msg = msg+"\n"
		self.client.send(msg.encode())
		del msg
		del data

		# if reset, no need to receive feed back
		if not reset:
			data_recv = self.receive_msg()

			if data_recv and not data_recv['success']:
				self.state = copy.deepcopy(self.state_old)


			return data_recv

	def receive_msg(self):
		try:
			data_frame = {}

			object_head = self.server.getBuffer()
			assert(object_head == "Objects")
			object_data = loads(self.server.getBuffer())
			#print object_data
			
			if self.state['depth']:
				depth_head = self.server.getBuffer()
				assert(depth_head == "Depth")
				depth_data = np.load(BytesIO(self.server.getBuffer()))
				data_frame["depth"] = depth_data

			if self.state['rgb']:
				rgb_head = self.server.getBuffer()
				assert(rgb_head == "RGB")
				rgb_data = np.load(BytesIO(self.server.getBuffer()))
				data_frame["rgb"] = rgb_data
				del rgb_data
				# rgb_image = Image.fromarray(rgb_data)
				# rgb_image.show()
				# rgb_image.close()

			if self.state['mask']:
				mask_head = self.server.getBuffer()
				assert(mask_head == "object_mask")
				mask_data = np.load(BytesIO(self.server.getBuffer()))
				data_frame["object_mask"] = mask_data
			
			reward_head = self.server.getBuffer()
			assert(reward_head == "Reward")
			reward = float(self.server.getBuffer())

			done_head = self.server.getBuffer()
			assert(done_head == "Done")
			done = (self.server.getBuffer() == "1")

			success_head = self.server.getBuffer()
			assert(success_head == "Success")
			success = (self.server.getBuffer() == "1")

			data_frame["objects"] = object_data
			data_frame["frame"] = self.frame
			data_frame["reward"] = reward
			data_frame["done"] = done
			data_frame["success"] = success

			return data_frame

		except Exception as e:
			print(e)
			return None

	def RelToWorld(self, x, y, z, theta):
		abs_length = np.sqrt(x**2+y**2)
		actor_theta = math.atan2(x, y)/math.pi*180
		actor_theta -= self.state["Actor"]["Rot"]["Yaw"]
		x_new = abs_length*math.sin(actor_theta/180*math.pi)+self.state["Actor"]["Loc"]["X"]
		y_new = abs_length*math.cos(actor_theta/180*math.pi)+self.state["Actor"]["Loc"]["Y"]
		z_new = z+self.state["Actor"]["Loc"]["Z"]

		theta_new = theta+self.state["Actor"]["Rot"]["Yaw"]
		if theta_new > 180:
			theta_new -= 360
		if theta_new < -180:
			theta_new += 360
		return x_new, y_new, z_new, theta_new

	def WorldToRel(self, x_new, y_new, z_new):
		z = z_new - self.state["Actor"]["Loc"]["Z"]
		x_temp = x_new - self.state["Actor"]["Loc"]["X"]
		y_temp = y_new - self.state["Actor"]["Loc"]["Y"]
		abs_length = (x_temp/(math.sin(math.atan2(x_temp, y_temp))+1e-5))**2
		alpha = (math.atan2(x_temp, y_temp)*180/math.pi+self.state["Actor"]["Rot"]["Yaw"])
		rate = math.tan(alpha/180*math.pi)
		x = np.sqrt(abs_length*rate**2/(rate**2+1))
		y = x/rate

		return x, y, z

	def Move(self, entity, idx, delta=10, scale=1):
		move_dict = {0:"X", 1:"Y", 2:"Z"}
		axis = move_dict[idx]
		self.state[entity]["Loc"][axis] += scale*delta

	def ControlRightHand(self, delta_loc, delta_rot, grab_strength, grab_actor, grab_comp, scale=100):
		self.state["RightHand"]["Loc"]["X"] += scale*delta_loc[0]
		self.state["RightHand"]["Loc"]["Y"] += scale*delta_loc[1]
		self.state["RightHand"]["Loc"]["Z"] += scale*delta_loc[2]
		self.state["RightHand"]["Rot"]["Pitch"] += scale*delta_rot[0]
		self.state["RightHand"]["Rot"]["Yaw"] += scale*delta_rot[1]
		self.state["RightHand"]["Rot"]["Roll"] += scale*delta_rot[2]
		if grab_strength > 0:
			self.state["RightHand"]["Grab"] = True
			self.state["RightHand"]["ActorName"] = grab_actor
			self.state["RightHand"]["CompName"] = grab_comp


	def Rotate(self, entity, idx, delta=90, scale=1):
		rot = copy.deepcopy(self.state[entity]["Rot"])

		if entity == "Actor":
			assert(idx == 1)
		if entity == "Head":
			assert(idx == 0)

		rotate_dict = {0:"Pitch", 1:"Yaw", 2:"Roll"}
		axis = rotate_dict[idx]
		rot[axis] += scale*delta
		if rot[axis] > 180:
			rot[axis] -= 360
		if rot[axis] < -180:
			rot[axis] += 360
		self.state[entity]["Rot"][axis] = rot[axis]

	def Crouch(self):
		if self.crouch == False:
			self.crouch = True
			self.Move("LeftHand", 2, scale=-8)
			self.Move("LeftHand", 1, scale=-2)
			self.Rotate("LeftHand", 1, scale=1.0)
			self.state["LeftHand"]["NeutralLoc"]["Z"] -= 80
			self.state["LeftHand"]["NeutralLoc"]["Y"] -= 20
			self.state["LeftHand"]["NeutralRot"]["Yaw"] += 90

			#self.step("RightHandMoveDown", scale = 8.5)
			self.Move("RightHand", 2, scale=-8.5)
			self.state["RightHand"]["NeutralLoc"]["Z"] -= 85

			# self.state["Head"]["Rot"]["Roll"] = -self.state["Head"]["Rot"]["Pitch"]
			# self.state["Head"]["Rot"]["Pitch"] = 0

		data_frame = None
		i = 0
		while data_frame == None and i < self.retry_num:
			data_frame = self.send_tf(world=False)
			i += 1

	
		if data_frame == None:
			print("Connection break. Manual shutdown.") 

		self.frame += 1
		self.data = data_frame

		return data_frame

	def Standup(self):
		if self.crouch == True:
			self.crouch = False
			self.Move("LeftHand", 2, scale=8)
			self.Move("LeftHand", 1, scale=2)
			self.Rotate("LeftHand", 1, scale=-1.0)
			self.state["LeftHand"]["NeutralLoc"]["Z"] += 80
			self.state["LeftHand"]["NeutralLoc"]["Y"] += 20
			self.state["LeftHand"]["NeutralRot"]["Yaw"] -= 90

			self.Move("RightHand", 2, scale=8.5)
			self.state["RightHand"]["NeutralLoc"]["Z"] += 85

			# self.state["Head"]["Rot"]["Pitch"] = -self.state["Head"]["Rot"]["Roll"]
			# self.state["Head"]["Rot"]["Roll"] = 0

		data_frame = None
		i = 0
		while data_frame == None and i < self.retry_num:
			data_frame = self.send_tf(world=False)
			i += 1

		if data_frame == None:
			print("Connection break. Manual shutdown.") 

		self.frame += 1
		self.data = data_frame

		return data_frame


	def Walk(self, loc_final, speed=5, anim_speed=40):
		loc_now = self.state["Actor"]["Loc"]
		dif = np.array([loc_final[key] - loc_now[key] for key in loc_final])
		dif_len = np.linalg.norm(dif)

		times = np.ceil(dif_len/speed)
		move_unit = dif/times

		for i in np.arange(times):
			loc_now["Y"] += move_unit[0]
			loc_now["X"] += move_unit[1]
			loc_now["Z"] += move_unit[2]
			
			# print loc_now

			data_frame = None
			i = 0
			while data_frame == None and i < self.retry_num:
				data_frame = self.send_tf(anim_speed=anim_speed)
				i += 1

			if data_frame == None:
				print("Connection break. Manual shutdown.") 

			self.frame += 1
			self.data = data_frame

		return data_frame

	def GrabObject(self, entity, actor_name, comp_name):
		self.state[entity]["ActorName"] = actor_name
		self.state[entity]["CompName"] = comp_name
		self.state[entity]["Grab"] = True
		a = self.send_tf(world=True)
		# set grab to false to avoid checking too many times
		self.state[entity]["Grab"] = False
		return a

	def ReleaseObject(self, entity):
		self.state[entity]["Release"] = True
		a = self.send_tf(world=True)
		self.state[entity]["Release"] = False
		self.state[entity]["ActorName"] = ""
		self.state[entity]["CompName"] = ""
		return a

	def MoveToObject(self, entity, actor_name, comp_name, \
		speed=SPEED, closest=False):
		ActorLoc = copy.deepcopy(self.state["Actor"]["Loc"])

		if self.crouch:
			ActorLoc["Z"] += 50
		else:
			ActorLoc["Z"] += 150

		start_time = time.time()
		if closest:
			min_dist = 1e8
			min_loc = None

			for actor in self.data["objects"]:
				if actor_name not in self.data["objects"][actor]["ActorType"]:
					continue
				for comp in self.data["objects"][actor]:
					if comp == "ActorType":
						continue
					if comp_name != None and comp != comp_name:
						continue
					CompLoc = self.data["objects"][actor][comp]["Loc"]
					dist = 0
					for key in CompLoc:
						dist += (CompLoc[key]-ActorLoc[key])**2
					if dist < min_dist:
						min_dist = dist
						min_loc = CompLoc

			loc_final = min_loc

		else:
			loc_final = self.data["objects"][actor_name][comp_name]["Loc"]

		# print "take time to find closest", time.time()-start_time
		
		return self.MoveToWorld(entity, loc_final, speed)

	def MoveAndGrabObject(self, entity, actor_name, comp_name, \
		speed=SPEED, closest=False):
		ActorLoc = copy.deepcopy(self.state["Actor"]["Loc"])
		if self.crouch:
			ActorLoc["Z"] += 50
		else:
			ActorLoc["Z"] += 150

		if closest:
			min_dist = 1e8
			min_loc = None
			min_actor = None
			min_comp = None

			for actor in self.data["objects"]:
				if actor_name not in self.data["objects"][actor]["ActorType"]:
					continue
				for comp in self.data["objects"][actor]:
					if comp == "ActorType":
						continue

					if comp_name != None and comp != comp_name:
						continue
					CompLoc = self.data["objects"][actor][comp]["Loc"]
					dist = 0
					for key in CompLoc:
						dist += (CompLoc[key]-ActorLoc[key])**2
					if dist < min_dist:
						min_dist = dist
						min_loc = CompLoc
						min_actor = actor
						min_comp = comp

			loc_final = min_loc

		else:
			loc_final = self.data["objects"][actor_name][comp_name]["Loc"]
		
		self.MoveToWorld(entity, loc_final, speed)
		self.GrabObject(entity, min_actor, "ContainerMesh")
		return self.MoveToNeutral(entity, speed)

	def MoveToNeutral(self, entity, speed=SPEED):
		loc = self.state[entity]["NeutralLoc"]
		x_new, y_new, z_new, _ = self.RelToWorld(loc["X"], loc["Y"], loc["Z"], 0)
		loc_final = {"X":x_new, "Y":y_new, "Z":z_new}
		return self.MoveToWorld(entity, loc_final, speed)		

	def MoveContactToWorld(self, entity, loc_final, speed=SPEED):
		eps = 1e-3
		
		loc_now =  self.state[entity]["WorldLoc"]
		dif = np.array([loc_final[key] - loc_now[key] for key in loc_final])
		dif_len = np.linalg.norm(dif)

		times = np.ceil(dif_len/speed+eps)
		move_unit = dif/times

		for i in np.arange(times):
			loc_now["Y"] += move_unit[0]
			loc_now["X"] += move_unit[1]
			loc_now["Z"] += move_unit[2]
			
			# print loc_now

			data_frame = None
			i = 0
			while data_frame == None and i < self.retry_num:
				data_frame = self.send_tf(world=True)
				i += 1

			if data_frame == None:
				print("Connection break. Manual shutdown.") 

			self.frame += 1
			self.data = data_frame

		return data_frame

	def MoveContactToObject(self, entity, contact_name, \
			actor_name, comp_name, speed=SPEED):
		grab_actor_name = self.state[entity]["ActorName"]
		temp = [
			self.data["objects"][actor_name][comp_name]["Loc"][key] - \
			self.data['objects'][grab_actor_name][contact_name]["Loc"][key] + \
			self.state[entity]["WorldLoc"][key] 
			for key in self.data["objects"][actor_name][comp_name]["Loc"]
		]
		loc_final = {"Y": temp[0], "X":temp[1], "Z":temp[2]}
		# print self.data["objects"][actor_name][comp_name]["Loc"]
		# print self.data['objects'][grab_actor_name][contact_name]["Loc"]
		# print self.state[entity]["WorldLoc"] 
		# print loc_final
		return self.MoveContactToWorld(entity, loc_final, speed)

	def MoveToWorld(self, entity, loc_final, speed=SPEED):
		eps = 1e-3
		loc_now = self.state[entity]["WorldLoc"]
		if loc_final == None:
			loc_final = loc_now
		dif = np.array([loc_final[key] - loc_now[key] for key in loc_final])
		dif_len = np.linalg.norm(dif)

		times = np.ceil(dif_len/speed+eps)
		move_unit = dif/times


		for i in np.arange(times):
			loc_now["Y"] += move_unit[0]
			loc_now["X"] += move_unit[1]
			loc_now["Z"] += move_unit[2]
			
			# print loc_now

			data_frame = None
			i = 0
			while data_frame == None and i < self.retry_num:
				data_frame = self.send_tf(world=True)
				i += 1

			if data_frame == None:
				print("Connection break. Manual shutdown.") 

			self.frame += 1
			self.data = data_frame

		return data_frame

	def CutObject(self, entity, actor_name, speed=5):
		self.MoveToObject(entity,"Knife","StaticMeshComponent0")
		self.GrabObject(entity,"Knife","StaticMeshComponent0")
		self.MoveToNeutral(entity)
		self.step(entity+"TwistLeft", scale=5)
		self.MoveContactToObject(entity,"CutPoint", actor_name, "Fruit")
		self.MoveToNeutral(entity)
		self.MoveContactToObject(entity,"CutPoint", actor_name, "Fruit")
		self.MoveToNeutral(entity)
		self.MoveContactToObject(entity,"CutPoint", actor_name, "Fruit")
		self.MoveToNeutral(entity)
		self.MoveContactToObject(entity,"CutPoint", actor_name, "ProceduralMeshComponent_0")
		self.MoveToNeutral(entity)
		self.MoveContactToObject(entity,"CutPoint", actor_name, "ProceduralMeshComponent_0")
		self.MoveToNeutral(entity)
		self.MoveContactToObject(entity,"CutPoint", actor_name, "ProceduralMeshComponent_0")
		res = self.MoveToNeutral(entity)
		return res

	def OpenBottle(self, entity1, entity2, opener_name, bottle_name, speed=5):
		self.MoveToObject(entity1, opener_name, "StaticMeshComponent0", speed)
		self.GrabObject(entity1, opener_name, "StaticMeshComponent0")
		self.MoveToNeutral(entity1, speed)
		self.MoveToObject(entity2, bottle_name, "GrabPoint", speed)
		self.GrabObject(entity2, bottle_name, "ContainerMesh")
		self.MoveToNeutral(entity2, speed)
		self.MoveContactToObject(entity1, "Box", bottle_name, "Box", speed)
		res = self.MoveToNeutral(entity1, speed)
		return res

	def ToastBread(self, entity, bread_name, toaster_name, speed=5):
		self.MoveToObject(entity, bread_name, "GrabPoint", speed)
		self.GrabObject(entity, bread_name, "StaticMeshComponent0")
		self.MoveToNeutral(entity, speed)
		self.step(entity+"RotateUp", scale=5)
		self.MoveContactToObject(entity, "StaticMeshComponent0", toaster_name, "PutBox", speed)
		self.ReleaseObject(entity)
		res = self.MoveToNeutral(entity, speed)
		return res

	def GoToPos(self, PosName):
		data_frame = None
		i = 0
		while data_frame == None and i < self.retry_num:
			data_frame = self.send_tf()
			i += 1

		if data_frame == None:
			print("Go to Pos. Connection break. Manual shutdown.") 

		self.frame += 1
		self.data = data_frame

		return data_frame


	def step(self, action, world=False, scale=1.0, loc=None, rot=None, grab_strength=None, grab_actor=None, grab_comp=None):
		theta = self.state["Actor"]["Rot"]["Yaw"]/180*math.pi
		x = math.cos(theta)*scale
		y = math.sin(theta)*scale
		self.state_old = copy.deepcopy(self.state)
		# print x
		# print y
		st = time.time()
		if action == "ActorMoveForward":
			self.Move("Actor", 0, scale=x)
			self.Move("Actor", 1, scale=y)
		elif action == "ControlRightHand":
			self.ControlRightHand(loc, rot, grab_strength, grab_actor, grab_comp, scale=scale)
		elif action == "MoveToNeutral":
			self.MoveToNeutral("RightHand", speed=10000)
		elif action == "Crouch":
			self.Crouch()
			print("crouch", time.time()-st)
		elif action == "Standup":
			self.Standup()
			print("Standup", time.time()-st)
		elif action == "GrabKnife":
			self.GrabObject("RightHand", "Knife", "StaticMeshComponent0")
		elif action == "ActorMoveBackward":
			self.Move("Actor", 0, scale=-x)
			self.Move("Actor", 1, scale=-y)
		elif action == "ActorMoveRight":
			self.Move("Actor", 0, scale=-y)
			self.Move("Actor", 1, scale=x)
		elif action == "ActorMoveLeft":
			self.Move("Actor", 0, scale=y)
			self.Move("Actor", 1, scale=-x)
		elif action == "ActorRotateRight":
			self.Rotate("Actor", 1, scale=1)
		elif action == "ActorRotateLeft":
			self.Rotate("Actor", 1, scale=-1)
		elif action == "LeftHandMoveForward":
			self.Move("LeftHand", 0, scale=scale)
		elif action == "LeftHandMoveBackward":
			self.Move("LeftHand", 0, scale=-scale)
		elif action == "LeftHandMoveRight":
			self.Move("LeftHand", 1, scale=scale)
		elif action == "LeftHandMoveLeft":
			self.Move("LeftHand", 1, scale=-scale)
		elif action == "LeftHandMoveUp":
			self.Move("LeftHand", 2, scale=scale)
		elif action == "LeftHandMoveDown":
			self.Move("LeftHand", 2, scale=-scale)
		elif action == "LeftHandRotateRight":
			self.Rotate("LeftHand", 1, scale=1.0/5*scale)
		elif action == "LeftHandRotateLeft":
			self.Rotate("LeftHand", 1, scale=-1.0/5*scale)
		elif action == "LeftHandRotateUp":
			self.Rotate("LeftHand", 0, scale=1.0/5*scale)
		elif action == "LeftHandRotateDown":
			self.Rotate("LeftHand", 0, scale=-1.0/5*scale)
		elif action == "LeftHandTwistRight":
			self.Rotate("LeftHand", 2, scale=1.0/5*scale)
		elif action == "LeftHandTwistLeft":
			self.Rotate("LeftHand", 2, scale=-1.0/5*scale)
		elif action == "RightHandMoveForward":
			self.Move("RightHand", 0, scale=scale)
		elif action == "RightHandMoveBackward":
			self.Move("RightHand", 0, scale=-scale)
		elif action == "RightHandMoveRight":
			self.Move("RightHand", 1, scale=scale)
		elif action == "RightHandMoveLeft":
			self.Move("RightHand", 1, scale=-scale)
		elif action == "RightHandRotateRight":
			self.Rotate("RightHand", 1, scale=1.0/5*scale)
		elif action == "RightHandRotateLeft":
			self.Rotate("RightHand", 1, scale=-1.0/5*scale)
		elif action == "RightHandRotateUp":
			self.Rotate("RightHand", 0, scale=1.0/5*scale)
		elif action == "RightHandRotateDown":
			self.Rotate("RightHand", 0, scale=-1.0/5*scale)
		elif action == "RightHandTwistRight":
			self.Rotate("RightHand", 2, scale=1.0/5*scale)
		elif action == "RightHandTwistLeft":
			self.Rotate("RightHand", 2, scale=-1.0/5*scale)
		elif action == "RightHandMoveUp":
			self.Move("RightHand", 2, scale=scale)
		elif action == "RightHandMoveDown":
			self.Move("RightHand", 2, scale=-scale)
		elif action == "LookUp":
			self.Rotate("Head", 0, scale=1.0/3*scale)
		elif action == "LookDown":
			self.Rotate("Head", 0, scale=-1.0/3*scale)
		elif action == "LeftHandGrab":
			self.state["LeftHand"]["Grab"] = True
		elif action == "LeftHandRelease":
			self.state["LeftHand"]["Grab"] = False
		elif action == "RightHandGrab":
			self.state["RightHand"]["Grab"] = True
		elif action == "RightHandRelease":
			self.state["RightHand"]["Grab"] = False
		else:
			print("Not a valid command")

		data_frame = None
		i = 0
		while data_frame == None and i < self.retry_num:
			data_frame = self.send_tf(world=world)
			i += 1

		if data_frame == None:
			print("Connection break. Manual shutdown.") 

		self.frame += 1
		self.data = data_frame

		self.state["RightHand"]["Grab"] = False
		self.state["RightHand"]["Release"] = False
		self.state["LeftHand"]["Grab"] = False
		self.state["LeftHand"]["Release"] = False

		return data_frame

'''
class BasicVRTask:
	def __init__(self, max_steps=sys.maxsize, frame_size=84, frame_skip=1, scale=1.0):
		self.steps = 0
		self.max_steps = max_steps
		self.frame_size = frame_size
		self.frame_skip = frame_skip
		self.env = None
		self.scale = scale

	def reset(self):
		self.steps = 0
		if self.env == None:
			self.env = DiscreteAgent(self.start_pose)
			self.env.start()
		data = self.env.reset()
		state = transform.resize(color.rgb2gray(data['rgb']),\
		 (self.frame_size, self.frame_size), mode='constant')
		state = np.moveaxis(state, -1, 0)
		state = np.stack([state for i in range(self.frame_skip)])
		# state = data["state"]
		return state

	def normalize_state(self, state):
		return state

	def step(self, a):
		action_list = ["RightHandMoveUp", "RightHandMoveRight", "RightHandTwistLeft","GrabKnife"]
		action = action_list[a]

		next_states = []
		for i in range(self.frame_skip):
			# data = self.env.step(action)
			data = self.env.step(action, scale=self.scale)
			next_state = transform.resize(color.rgb2gray(data['rgb']),\
			 (self.frame_size, self.frame_size), mode='constant')
			next_state = np.moveaxis(next_state, -1, 0)
			next_states.append(next_state)

		next_states = np.stack(next_states)

		# data = self.env.step(action)
		# next_states = data["state"]

		reward = data['reward']
		done = data['done']
		info = None
		self.steps += 1
		done = (done or self.steps >= self.max_steps)
		# if done:
			# print "reward", reward
		return next_states, reward, done, info
	
	def set_monitor(self, filename):
		self.env = Monitor(self.env, filename)


class PixelVR(BasicVRTask):
	success_threshold = 0
	def __init__(self, init_state, name, scale=1.0, \
		normalized_state=True, max_steps=30, frame_size=84, frame_skip=1):
		BasicVRTask.__init__(self, \
			max_steps, frame_size, frame_skip, scale=scale)
		self.normalized_state = normalized_state
		self.action_dim = 12
		self.name = name
		self.init_state = init_state

	def normalize_state(self, state):
		return np.asarray(state) / 255.0

	def start(self):
		self.env = DiscreteAgent(self.init_state)
		self.env.start()

def dqn_pixel_vr():
	config = Config()
	config.history_length = 1
	# Opening Cabinet
	# init_state = {"Name":"Agent1", \
	# 	"Actor":{"Loc":{"X":-867.0,"Y":340.0,"Z":36.0},\
	# 		"Rot":{"Pitch":0.0,"Yaw":90.0,"Roll":0.0}}, \
	# 	"LeftHand":{"Grab": False, "Loc":{"X":65,"Y":-30,"Z":130},\
	# 		"Rot":{"Pitch":0.0,"Yaw":0.0,"Roll":0.0}},\
	# 	"RightHand":{"Grab": False, "Loc":{"X":60,"Y":40,"Z":100},\
	# 		"Rot":{"Pitch":0.0,"Yaw":0.0,"Roll":0.0}},
	# 	"Head": {"Rot":{"Pitch":-45.0,"Yaw":0.0,"Roll":0.0}}
	# 	}
	# Opening a beer bottle
	init_state = {"Name":"Agent1", \
		"Actor":{"Loc":{"X":-730.0,"Y":65.0,"Z":36.0},\
			"Rot":{"Pitch":0.0,"Yaw":0.0,"Roll":0.0}}, \
		"LeftHand":{"Grab": True, "Loc":{"X":40,"Y":-30,"Z":75},\
			"Rot":{"Pitch":0.0,"Yaw":0.0,"Roll":0.0}},\
		"RightHand":{"Grab": True, "Loc":{"X":50,"Y":-5,"Z":95},\
			"Rot":{"Pitch":0.0,"Yaw":0.0,"Roll":0.0}},
		"Head": {"Rot":{"Pitch":-45.0,"Yaw":0.0,"Roll":0.0}}
		}
	config.task_fn = lambda: PixelVR(init_state, \
		name="Cut Carrot", scale=1/2.0, normalized_state=False, \
		max_steps=30, frame_skip=config.history_length)
	action_dim = config.task_fn().action_dim
	config.optimizer_fn = lambda params: torch.optim.Adam(params, 0.0005)
	# config.optimizer_fn = lambda params: torch.optim.RMSprop(params, lr=0.00005, alpha=0.95, eps=0.01)
	config.network_fn = lambda: NatureConvNet(config.history_length, action_dim, gpu=0)
	# config.network_fn = lambda: DuelingNatureConvNet(config.history_length, action_dim)
	# config.network_fn = lambda: FCNet([12, 256, 256, action_dim])
	config.policy_fn = lambda: GreedyPolicy(epsilon=1.0, final_step=50000, min_epsilon=0.1)
	config.replay_fn = lambda: Replay(memory_size=10000, batch_size=32, dtype=np.uint8)
	config.reward_shift_fn = lambda r: (r)
	config.discount = 0.99
	config.target_network_update_freq = 2500
	config.max_episode_length = 0
	config.exploration_steps= 0
	config.logger = Logger('./log', logger)
	config.test_interval = 50
	config.test_repetitions = 1
	config.episode_limit = 10000000
	config.double_q = True
	# config.double_q = False
	run_episodes(DQNAgent(config))

if __name__ == "__main__":
	# logger.setLevel(logging.INFO)
	# logger.setLevel(logging.DEBUG)
	dqn_pixel_vr()
'''
	





 

