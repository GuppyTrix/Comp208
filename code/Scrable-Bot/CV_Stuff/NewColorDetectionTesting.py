import cv2
import numpy as np
from picamera2 import Picamera2

# Initialize camera
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(
	raw ={"size":(2640,1232)},
	main={"size":(640,480), "format":'RGB888'}))
picam2.start()

def detect_coke_can(frame):
    # Convert to HSV for color detection
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Red color range (Coke can red)
    lower_red1 = np.array([0, 100, 100])
    upper_red1 = np.array([10, 255, 255])
    lower_red2 = np.array([160, 100, 100])
    upper_red2 = np.array([180, 255, 255])
    
    # Create masks
    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
    red_mask = cv2.bitwise_or(mask1, mask2)
    
    # Find contours
    contours, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    for contour in contours:
        area = cv2.contourArea(contour)
        if area > 200:  # Minimum size
            # Approximate shape
            approx = cv2.approxPolyDP(contour, 0.02 * cv2.arcLength(contour, True), True)
            
            # Look for cylindrical shapes (rectangle-ish)
            if len(approx) >= 4:
                x, y, w, h = cv2.boundingRect(contour)
                aspect_ratio = float(w)/h
                
                # Coke can typical aspect ratio
                if 0.3 < aspect_ratio < 0.7:
                    cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
                    cv2.putText(frame, "Coke Can", (x, y-10), 
                               cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
    
    return frame

while True:
    frame = picam2.capture_array()
    detected = detect_coke_can(frame)
    cv2.imshow('Coke Can Detection', detected)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
picam2.stop()
picam2.close()
cv2.destroyAllWindows()
