import cv2
from picamera2 import Picamera2
import time
import numpy as np

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(
	raw ={"size":(2640,1232)},
	main={"size":(640,480), "format":'RGB888'}))
picam2.start()
time.sleep(2)


while True:
	
	img = picam2.capture_array()

	cv2.imshow("Output", img)	
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

picam2.stop()	
picam2.close()
