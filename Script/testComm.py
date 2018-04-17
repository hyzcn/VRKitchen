from socketClient import Client
from socketServer import Server
from rapidjson import loads,dumps 
import time
import numpy as np
from PIL import Image
from ast import literal_eval
from io import BytesIO


endpoint = ("128.97.86.170", 10020)
client = Client(endpoint)
serverpoint = ("128.97.86.192", 10021)
server = Server(serverpoint)
server.listen()

client.connect()
data = {}
data['RobotName'] = 'BaxterRobot'
data['Anim'] = []
msg = dumps(data)
msg = msg+"\n"
client.send(msg.encode())

time.sleep(0.5)
recv_data = server.getBuffer()
print(recv_data)
recv_data = server.getBuffer()
print(len(recv_data))
print(type(recv_data))
recv_data = server.getBuffer()
print(recv_data)
recv_data = server.getBuffer()
print(len(recv_data))
print(type(recv_data))

a = np.load(BytesIO(recv_data))
print(a.shape)
img = Image.fromarray(a, 'RGB')
print(img)
img.show()

client.disconnect()
server.stop()
