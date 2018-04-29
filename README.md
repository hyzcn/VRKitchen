VCLA VRKitchen Platform 

Requirement: Unreal 4.17 in Windows 10/Ubuntu 16.04, Oculus Rift and Touch, (ROS, Baxter simulator)

Usage in Linux:

1. Start Baxter simulator

cd ros_ws

./baxter.sh sim

roslaunch baxter_gazebo baxter_world.launch

2. Start Unreal Editor

cd ~/UnrealEngine-4.13/Engine/Binaries/Linux

./UE4Editor "~/VRKitchen/VRInteractPlatform.uproject"

3. Execute python script

cd ros_ws

./baxter.sh sim

python ~/VRKitchen/Script/BaxterAgent.py
