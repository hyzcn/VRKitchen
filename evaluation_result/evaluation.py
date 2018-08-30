from __future__ import division
import os
import numpy as np

my_path = os.getcwd()
x_path = my_path+"/X/"
noX_path = my_path+"/noX/"
X_result = []
noX_result = []

for fn in os.listdir(x_path):
	if "Test" in fn:
		f = open(x_path+fn, "r")
		temp = []
		data = f.readlines()
		if len(data) > 18:
			data = data[:-1]
		print data
		for line in data:
			temp.append(int(line.strip()))
		X_result.append(temp)

for fn in os.listdir(noX_path):
	if "Test" in fn:
		f = open(noX_path+fn, "r")
		temp = []
		data = f.readlines()
		if len(data) > 18:
			data = data[:-1]
		print data
		for line in data:
			temp.append(int(line.strip()))
		noX_result.append(temp)

robot_knowledge1 = [4, 5, 3]
robot_knowledge2 = [[4, 3, 3], [4, 3, 3], [4, 5, 3]]
robot_knowledge3 = [5, 4, 3]
robot_knowledge4 = [[1, 4, 3], [1, 4, 3], [5, 4, 3]]

robot_plan1 = ["take the lemon", "cut the lemon"]
robot_plan2 = ["take the lemon", "cut the lemon"]
robot_plan3 = ["take the cup", "use the juicer"]
robot_plan4 = ["take the cup", "use the juicer"]

cup_state = ["", "in the red cabinet", "on the table", "in the fridge", "no idea", "except fridge"]
lemon_state = ["", "in the red cabinet", "on the table", "in the fridge", "on the board", "no idea"]
juicer_state = ["", "in the red cabinet", "in the fridge", "on the table", "no idea"]

plan = {
	"take the lemon": ["open fridge", "grab lemon", "close fridge"], 
	"cut the lemon": ["grab knife", "cut", "place knife"], 
	"take the cup": ["open cabinet", "grab cup", "close cabinet"], 
	"use the juicer": ["place cup", "place lemon"]
}

for res in X_result:
	if res[3] == 2:
		res[3] += 2

	if res[4] == 2:
		res[4] += 3

	if res[5] == 2:
		res[5] += 2

	if res[9] == 2:
		res[9] += 1
	elif res[9] == 3:
		res[9] -= 1

	if res[10] == 2:
		res[10] += 2

	if res[12] == 2:
		res[12] += 1
	elif res[12] == 3:
		res[12] += 2

	if res[13] == 2:
		res[13] += 2

	if res[14] == 2:
		res[14] += 2

for res in noX_result:
	if res[3] == 2:
		res[3] += 2

	if res[4] == 2:
		res[4] += 3

	if res[5] == 2:
		res[5] += 2

	if res[9] == 2:
		res[9] += 1
	elif res[9] == 3:
		res[9] -= 1

	if res[10] == 2:
		res[10] += 2

	if res[12] == 2:
		res[12] += 1
	elif res[12] == 3:
		res[12] += 2

	if res[13] == 2:
		res[13] += 2

	if res[14] == 2:
		res[14] += 2

def GetTrustRel(X_result):
	j = 0
	rel_plans = np.zeros((len(X_result), 4))
	trust_plans = np.zeros((len(X_result), 4))
	rel_knows = np.zeros((len(X_result), 4))
	trust_knows = np.zeros((len(X_result), 4))
	for res in X_result:
		rel_plan_single = []
		trust_plan_single = []
		rel_know_single = []
		trust_know_single = []

		# before first action
		h_know = res[:3]
		minu_know = res[3:6]
		r_know = robot_knowledge1

		minu_plan_idx = res[6:8]
		h_plan_idx = res[8]
		h_plan_label = ["open fridge", "point to the fridge", "do nothing"]
		h_plan = h_plan_label[h_plan_idx-1]

		r_plan = robot_plan1
		minu_label = [["take the lemon", "open the red cabinet", "take the cup"], ["take the cup", "use the juicer", "cut the lemon"]]
		minu_plan = []

		i = 0
		for item in minu_plan_idx:
			minu_plan.append(minu_label[i][item-1])
			i += 1

		# get trust and reliance for plan
		rel_plan_cur = 0
		trust_plan_cur = 0
		rel_plan_single.append(rel_plan_cur)
		trust_plan_single.append(trust_plan_cur)
		
		# get trust and reliance for knowledge
		same = 0
		for i in range(len(h_know)):
			if h_know[i] == minu_know[i]:
				same += 1
		trust_know_cur = same/len(h_know)

		same = 0
		for i in range(len(r_know)):
			if r_know[i] == minu_know[i]:
				same += 1
		rel_know_cur = same/len(r_know)

		rel_know_single.append(rel_know_cur)
		trust_know_single.append(trust_know_cur)


		# after first action
		r_plan = robot_plan2
		r_know = robot_knowledge2[h_plan_idx-1]
		if h_plan == 0 or h_plan == 1:
			minu_know[1] = 3

		# get trust and reliance for plan
		same = 0
		for i in range(len(r_plan)):
			if r_plan[i] == minu_plan[i]:
				same += 1
		rel_plan_cur += same/len(r_plan)

		for key in r_plan:
			if h_plan in plan[key]:
				trust_plan_cur += 1/len(plan[key])
				break

		rel_plan_single.append(rel_plan_cur)
		trust_plan_single.append(trust_plan_cur)

		# get trust and reliance for knowledge
		same = 0
		for i in range(len(h_know)):
			if h_know[i] == minu_know[i]:
				same += 1
		trust_know_cur += same/len(h_know)

		same = 0
		for i in range(len(r_know)):
			if r_know[i] == minu_know[i]:
				same += 1
		rel_know_cur += same/len(r_know)

		rel_know_single.append(rel_know_cur)
		trust_know_single.append(trust_know_cur)


		# before second action
		h_know = res[9:12]
		minu_know = res[12:15]
		r_know = robot_knowledge3

		minu_plan_idx = res[15:17]
		h_plan_idx = res[17]
		h_plan_label = ["open cabinet", "point to the cabin", "open the fridge"]
		h_plan = h_plan_label[h_plan_idx-1]

		r_plan = robot_plan3
		minu_label = [["use the juicer", "open the fridge", "take the cup"], ["take the cup", "use the juicer", "open the fridge"]]
		minu_plan = []

		i = 0
		for item in minu_plan_idx:
			minu_plan.append(minu_label[i][item-1])
			i += 1

		# get trust and reliance for plan
		rel_plan_single.append(rel_plan_cur)
		trust_plan_single.append(trust_plan_cur)

		# get trust and reliance for knowledge
		same = 0
		for i in range(len(h_know)):
			if h_know[i] == minu_know[i]:
				same += 1
		trust_know_cur += same/len(h_know)

		same = 0
		for i in range(len(r_know)):
			if r_know[i] == minu_know[i]:
				same += 1
		rel_know_cur += same/len(r_know)

		rel_know_single.append(rel_know_cur)
		trust_know_single.append(trust_know_cur)


		# after second action
		r_plan = robot_plan4
		r_know = robot_knowledge4[h_plan_idx-1]
		if h_plan == 0 or h_plan == 1:
			minu_know[0] = 1

		# get trust and reliance for plan
		same = 0
		for i in range(len(r_plan)):
			if r_plan[i] == minu_plan[i]:
				same += 1
		rel_plan_cur += same/len(r_plan)

		for key in r_plan:
			if h_plan in plan[key]:
				trust_plan_cur += 1/len(plan[key])
				break

		rel_plan_single.append(rel_plan_cur)
		trust_plan_single.append(trust_plan_cur)

		# get trust and reliance for knowledge
		same = 0
		for i in range(len(h_know)):
			if h_know[i] == minu_know[i]:
				same += 1
		trust_know_cur += same/len(h_know)

		same = 0
		for i in range(len(r_know)):
			if r_know[i] == minu_know[i]:
				same += 1
		rel_know_cur += same/len(r_know)

		rel_know_single.append(rel_know_cur)
		trust_know_single.append(trust_know_cur)

		rel_plans[j] = rel_plan_single
		trust_plans[j] = trust_plan_single
		rel_knows[j] = rel_know_single
		trust_knows[j] = trust_know_single
		j += 1

	return np.mean(rel_plans, axis=0), np.mean(rel_knows, axis=0), np.mean(trust_plans, axis=0), np.mean(trust_knows, axis=0)


print GetTrustRel(X_result)
print GetTrustRel(noX_result)





