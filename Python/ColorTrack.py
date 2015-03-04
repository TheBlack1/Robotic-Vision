import cv
import cv2
import numpy as np

cap = cv2.VideoCapture(0)

while(1):

    # Take each frame
    _, frame = cap.read()

    # Convert BGR to HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    # Convert BGR to GREY
    grey = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # Threshold for future use
    thresh1 = cv2.threshold(grey,127,255,cv2.THRESH_BINARY)

    # define range of blue color in HSV
    lower_blue = np.array([0,100,100])
    upper_blue = np.array([30,255,255])

    # Threshold the HSV image to get only blue colors
    mask = cv2.inRange(hsv, lower_blue, upper_blue)

    # Bitwise-AND mask and original image
    res = cv2.bitwise_and(frame,frame, mask= mask)

    moments = cv2.moments(mask)
    area = moments['m00']
    print area
    if(area > 100000):
        x = moments['m10'] / area
        y = moments['m01'] / area
        cv2.circle(frame, (int(x),int(y)),10, (255,255,255), 20)

    cv2.imshow('frame',frame)
    cv2.imshow('mask',mask)
    cv2.imshow('res',res)
    cv2.imshow('grey',grey)
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

print hsv

cv2.destroyAllWindows()
