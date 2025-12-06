import numpy as np
import cv2

def get_limits(color):
    c = np.uint8([[color]])  # BGR values
    hsvC = cv2.cvtColor(c, cv2.COLOR_BGR2HSV)

    hue = hsvC[0][0][0]  # Get the hue value
    
    # For red, we need to handle the wrap-around
    if hue <= 10 or hue >= 170:
        # Red needs two ranges
        lower_limit1 = np.array([0, 100, 100], dtype=np.uint8)
        upper_limit1 = np.array([10, 255, 255], dtype=np.uint8)
        lower_limit2 = np.array([170, 100, 100], dtype=np.uint8)
        upper_limit2 = np.array([180, 255, 255], dtype=np.uint8)
        
        # Return both ranges
        return (lower_limit2, upper_limit2)
    else:
        # For other colors
        lower_limit = np.array([hue - 10, 100, 100], dtype=np.uint8)
        upper_limit = np.array([hue + 10, 255, 255], dtype=np.uint8)
        return (lower_limit, upper_limit)
