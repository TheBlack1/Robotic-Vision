
import cv
import cv2
import numpy as np
import math
from PIL import Image, ImageDraw
import time
from naoqi import ALProxy

def stiff(pNames, pStiffnessLists, pTimeLists):
    motionProxy.stiffnessInterpolation(pNames, pStiffnessLists, pTimeLists)

def move(jointName, targetAngles,targetTimes,isAbsolute):
    motionProxy.angleInterpolation(jointName, targetAngles, targetTimes, isAbsolute)

isAbsolute = True;

IP = "daffy.local"
IP = "169.254.238.191"

camProxy = ALProxy("ALVideoDevice", IP, 9559)
motionProxy = ALProxy("ALMotion", IP, 9559)
postureProxy = ALProxy("ALRobotPosture", IP, 9559)
Leds = ALProxy("ALLeds",IP, 9559)

videoClient = camProxy.subscribe("python_client", 2, 11, 5)
cv.NamedWindow("robot", 0)

tempx = 0
tempy = 0

stiff("Body",1.0,1.0)
move("HeadYaw",0.0,0.1,isAbsolute)
move("HeadPitch",0.0,0.1,isAbsolute)
stiff("Body",0.0,1.0)
camProxy.setParam(18,1)

while True:
    # read nao camera data
	# 640*480 resolution of camera
	img = camProxy.getImageRemote(videoClient)

    # convert string of nao camera data into image
	im = Image.fromstring("RGB", (img[0], img[1]), img[6])

    # create RGB image
	im = np.array(im)

    # convert RGB to BGR because output will be blueish if isn't changed
	im = cv2.cvtColor(im, cv2.COLOR_RGB2BGR)

    # define range of red color in HSV
	lower_white = np.array([0,0,180], dtype=np.uint8)
	upper_white = np.array([90,90,255], dtype=np.uint8)
	
    # Threshold the HSV image to get only white colors
	mask = cv2.inRange(im, lower_white, upper_white)
  
    # find center of mass and area of color blob
	moments = cv2.moments(mask)
	area = moments['m00']
	
	if(10000 < area):
		x = moments['m10'] / area # x-axis coordinate 
		y = moments['m01'] / area # y-axis coordinate
		print 'x: ' + str(x) + ' y: ' + str(y) + ' area: ' + str(area) 
		cv2.circle(im, (int(x),int(y)),2, (255,255,255), 20)
		
		if(213 > x):
			if((x-10) < tempx < (x+10) and (y-10) < tempy < (y+10)):
				Leds.off("FaceLeds")
			else:
				Leds.off("RightFaceLeds")
				Leds.on("LeftFaceLeds")

		
		elif(213 < x < 426):
			if((x-10) < tempx < (x+10) and (y-10) < tempy < (y+10)):
				Leds.off("FaceLeds")
			else:
				Leds.on("FaceLeds")
			
			
		elif(x > 426):
			if((x-10) < tempx < (x+10) and (y-10) < tempy < (y+10)):
				Leds.off("FaceLeds")
			else:
				Leds.off("LeftFaceLeds")
				Leds.on("RightFaceLeds")
		tempx = x
		tempy = y
		
	else:
		Leds.off("FaceLeds")

	cv2.imshow("eyes",im)    # display frame

	k = cv2.waitKey(5) & 0xFF
	if k == 27:
		break
		
camProxy.unsubscribe(videoClient)    
cv.DestroyWindow("eyes")
