import cv2
from picamera2 import Picamera2
from PIL import Image
import time
import numpy as np

from util import get_limits

# Correct BGR color for red
red = [0, 0, 255]  # Pure red in BGR colorspace (B=0, G=0, R=255)

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(
    raw={"size": (2640, 1232)},
    main={"size": (640, 480), "format": 'RGB888'}))
picam2.start()
time.sleep(2)  # Let camera adjust

while True:
    frame = picam2.capture_array()
    
    # Convert to HSV
    hsvImage = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Get HSV limits for red
    lowerLimit, upperLimit = get_limits(color=red)
    
    # Create mask
    mask = cv2.inRange(hsvImage, lowerLimit, upperLimit)
    
    # Optional: Clean up the mask with morphology
    kernel = np.ones((5, 5), np.uint8)
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
    
    # Find contours
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    # Draw bounding boxes for all significant contours
    for contour in contours:
        x, y, w, h = cv2.boundingRect(contour)
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 3)
    
    # Show the frame and mask for debugging
    cv2.imshow('frame', frame)
    cv2.imshow('mask', mask)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

picam2.stop()
picam2.close()
cv2.destroyAllWindows()
