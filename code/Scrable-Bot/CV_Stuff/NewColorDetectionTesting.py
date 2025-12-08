import cv2
import numpy as np
import time
from picamera2 import Picamera2
import RPi.GPIO as GPIO

# GPIO Pin Configuration
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# Can detection value smoothing
last_can_found_change = 0
last_arrived_change = 0
can_found_state = False
arrived_state = False
CAN_FOUND_CHANGE_DELAY = 0.2  # seconds
ARRIVED_CHANGE_DELAY = 0.2   # seconds

# Define GPIO pins
CAN_LEFT_PIN = 17      # GPIO 17
CAN_RIGHT_PIN = 27     # GPIO 27
CAN_FOUND_PIN = 22     # GPIO 22
ARRIVED_AT_CAN_PIN = 23  # GPIO 23

# Setup GPIO pins as outputs
for pin in [CAN_LEFT_PIN, CAN_RIGHT_PIN, CAN_FOUND_PIN, ARRIVED_AT_CAN_PIN]:
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)  # Start with all pins low

# Screen configuration
SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 720
DEADZONE_WIDTH = 100  # Center deadzone where neither left nor right is active
DEADZONE_LEFT = SCREEN_WIDTH // 2 - DEADZONE_WIDTH // 2
DEADZONE_RIGHT = SCREEN_WIDTH // 2 + DEADZONE_WIDTH // 2

# Size thresholds for "arrived at can"
MIN_CAN_AREA = 300    # Minimum area to consider as "found"
ARRIVAL_AREA = 15000  # Area threshold for "arrived" (adjust based on your needs)

# Variables for debouncing
last_detection_time = 0
detection_cooldown = 0.5  # seconds
can_confidence = 0
CAN_FOUND_THRESHOLD = 3  # Need 3 consecutive detections to be "sure"

# Initialize camera
picam2 = Picamera2()
config = picam2.create_preview_configuration(main={"size": (SCREEN_WIDTH, SCREEN_HEIGHT)})
picam2.configure(config)
picam2.start()


def update_gpio(can_detected, can_center_x, can_area, can_aspect_ratio):
    global can_confidence, can_found_state, arrived_state
    global last_can_found_change, last_arrived_change
    
    current_time = time.time()
    
    # Reset left/right pins (these can change freely)
    GPIO.output(CAN_LEFT_PIN, GPIO.LOW)
    GPIO.output(CAN_RIGHT_PIN, GPIO.LOW)
    
    if can_detected and can_center_x is not None:
        # Update can confidence - only increase when detected
        if can_confidence < CAN_FOUND_THRESHOLD:
            can_confidence += 1
        
        # LEFT/RIGHT detection with deadzone (instant)
        if can_center_x < DEADZONE_LEFT:
            GPIO.output(CAN_LEFT_PIN, GPIO.HIGH)
        elif can_center_x > DEADZONE_RIGHT:
            GPIO.output(CAN_RIGHT_PIN, GPIO.HIGH)
        
        # CAN FOUND pin - instant when detecting
        if can_confidence >= CAN_FOUND_THRESHOLD:
            can_found_state = True
            GPIO.output(CAN_FOUND_PIN, GPIO.HIGH)
            last_can_found_change = current_time  # Reset timer when we have confidence
        # Don't immediately drop confidence when not detected in a single frame
        
        # ARRIVED AT CAN - instant when detecting
        if can_area >= ARRIVAL_AREA:
            arrived_state = True
            GPIO.output(ARRIVED_AT_CAN_PIN, GPIO.HIGH)
            last_arrived_change = current_time  # Reset timer when arrived
        else:
            arrived_state = False
            GPIO.output(ARRIVED_AT_CAN_PIN, GPIO.LOW)
            
    else:
        # No detection - LEFT/RIGHT pins already reset
        
        # CAN FOUND pin with rate limiting - only decrease confidence after timeout
        if can_found_state:
            # Check if enough time has passed since we last had high confidence
            if current_time - last_can_found_change >= CAN_FOUND_CHANGE_DELAY:
                # Start decreasing confidence slowly
                if can_confidence > 0:
                    can_confidence -= 1
                
                if can_confidence < CAN_FOUND_THRESHOLD:
                    can_found_state = False
                    GPIO.output(CAN_FOUND_PIN, GPIO.LOW)
            # else: keep current state (don't change anything)
        else:
            # Already off, just update confidence
            if can_confidence > 0:
                can_confidence -= 1
        
        # ARRIVED AT CAN with rate limiting
        if arrived_state:
            # Check if enough time has passed since we last had arrival
            if current_time - last_arrived_change >= ARRIVED_CHANGE_DELAY:
                arrived_state = False
                GPIO.output(ARRIVED_AT_CAN_PIN, GPIO.LOW)
            # else: keep current state (don't change anything)
      
def detect_coke_can(frame):
    """Detect Coke can and return information"""
    global last_detection_time
    
    # Convert to HSV for color detection
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Red color range (Coke can red)
    lower_red1 = np.array([0, 100, 100])
    upper_red1 = np.array([10, 255, 255])
    lower_red2 = np.array([160, 100, 100])
    upper_red2 = np.array([180, 255, 255])
    
    # Create masks for red color
    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
    red_mask = cv2.bitwise_or(mask1, mask2)
    
    # Find contours
    contours, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    best_can = None
    best_area = 0
    can_center_x = None
    can_area = 0
    can_aspect_ratio = 0
    can_detected = False
    
    for contour in contours:
        area = cv2.contourArea(contour)
            
        # Filter by minimum area
        if area > MIN_CAN_AREA:
            # Get bounding rectangle
            x, y, w, h = cv2.boundingRect(contour)
            aspect_ratio = float(w) / h
                
            # Coke can typically has aspect ratio ~0.5-0.7 when upright
            # and reasonable size limits
            if 0.3 < aspect_ratio < 0.8 and area > best_area:
                best_area = area
                best_can = (x, y, w, h)
                can_center_x = x + w // 2
                can_area = area
                can_aspect_ratio = aspect_ratio
        
    if best_can is not None:
        can_detected = True
    
    return can_detected, best_can, can_center_x, can_area, can_aspect_ratio, red_mask
    
    
def draw_info(frame, can_detected, best_can, can_center_x, can_area, can_aspect_ratio, gpio_state):
    """Draw information and overlays on the frame"""
    
    # Draw deadzone lines
    cv2.line(frame, (DEADZONE_LEFT, 0), (DEADZONE_LEFT, SCREEN_HEIGHT), (100, 100, 100), 2)
    cv2.line(frame, (DEADZONE_RIGHT, 0), (DEADZONE_RIGHT, SCREEN_HEIGHT), (100, 100, 100), 2)
    cv2.line(frame, (SCREEN_WIDTH//2, 0), (SCREEN_WIDTH//2, SCREEN_HEIGHT), (150, 150, 150), 1)
    
    # Draw center crosshair
    cv2.circle(frame, (SCREEN_WIDTH//2, SCREEN_HEIGHT//2), 10, (200, 200, 200), 1)
    
    if can_detected and best_can is not None:
        x, y, w, h = best_can
        
        # Draw bounding box
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
        
        # Draw center point of can
        cv2.circle(frame, (can_center_x, y + h//2), 5, (0, 0, 255), -1)
        
        # Draw line from screen center to can center
        cv2.line(frame, (SCREEN_WIDTH//2, SCREEN_HEIGHT//2), 
                (can_center_x, y + h//2), (255, 255, 0), 2)
        
        # Display can information
        info_y = 30
        cv2.putText(frame, f"CAN DETECTED", (10, info_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
        cv2.putText(frame, f"Center X: {can_center_x}", (10, info_y + 25), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        cv2.putText(frame, f"Area: {can_area:.0f}", (10, info_y + 50), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        cv2.putText(frame, f"Aspect Ratio: {can_aspect_ratio:.2f}", (10, info_y + 75), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        
        # Show which side of deadzone
        if can_center_x < DEADZONE_LEFT:
            side_text = "LEFT"
            side_color = (255, 100, 100)
        elif can_center_x > DEADZONE_RIGHT:
            side_text = "RIGHT"
            side_color = (100, 100, 255)
        else:
            side_text = "CENTER (DEADZONE)"
            side_color = (100, 255, 100)
            
        cv2.putText(frame, side_text, (can_center_x - 50, y - 10), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, side_color, 2)
    else:
        cv2.putText(frame, "NO CAN DETECTED", (10, 30), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)
    
    # Draw GPIO status
    gpio_y = SCREEN_HEIGHT - 120
    cv2.putText(frame, "GPIO STATUS:", (10, gpio_y), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
    
    status_colors = {
        "ON": (0, 255, 0),
        "OFF": (100, 100, 100)
    }
    
    # Read actual GPIO states
    left_state = "ON" if GPIO.input(CAN_LEFT_PIN) == GPIO.HIGH else "OFF"
    right_state = "ON" if GPIO.input(CAN_RIGHT_PIN) == GPIO.HIGH else "OFF"
    found_state = "ON" if GPIO.input(CAN_FOUND_PIN) == GPIO.HIGH else "OFF"
    arrived_state = "ON" if GPIO.input(ARRIVED_AT_CAN_PIN) == GPIO.HIGH else "OFF"
    
    cv2.putText(frame, f"CAN LEFT: {left_state}", (10, gpio_y + 25), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.5, status_colors[left_state], 2)
    cv2.putText(frame, f"CAN RIGHT: {right_state}", (10, gpio_y + 50), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.5, status_colors[right_state], 2)
    cv2.putText(frame, f"CAN FOUND: {found_state}", (10, gpio_y + 75), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.5, status_colors[found_state], 2)
    cv2.putText(frame, f"ARRIVED AT CAN: {arrived_state}", (10, gpio_y + 100), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.5, status_colors[arrived_state], 2)
    
    # Draw detection confidence
    cv2.putText(frame, f"Confidence: {can_confidence}/{CAN_FOUND_THRESHOLD}", 
               (SCREEN_WIDTH - 200, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 200, 0), 1)
               
    time_until_found_change = max(0, CAN_FOUND_CHANGE_DELAY - (time.time() - last_can_found_change))
    time_until_arrived_change = max(0, ARRIVED_CHANGE_DELAY - (time.time() - last_arrived_change))
    
    cv2.putText(frame, f"Found Cooldown: {time_until_found_change:.1f}s", 
               (SCREEN_WIDTH - 200, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 100), 1)
    cv2.putText(frame, f"Arrived Cooldown: {time_until_arrived_change:.1f}s", 
               (SCREEN_WIDTH - 200, 80), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 100), 1)
    
    return frame
try:
    print("Starting Coke Can Detection System...")
    print(f"Screen: {SCREEN_WIDTH}x{SCREEN_HEIGHT}")
    print(f"Deadzone: {DEADZONE_LEFT} to {DEADZONE_RIGHT}")
    print(f"Arrival area threshold: {ARRIVAL_AREA}")
    print("Press 'q' to quit")
    print("Press 'r' to reset GPIO pins")
    
    while True:
        # Capture frame
        frame = picam2.capture_array()
        
        # Convert from RGB to BGR for OpenCV
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        
        # Detect Coke can
        can_detected, best_can, can_center_x, can_area, can_aspect_ratio, red_mask = detect_coke_can(frame)
        
        # Update GPIO pins based on detection
        update_gpio(can_detected, can_center_x, can_area, can_aspect_ratio)
        
        # Draw information on frame
        frame = draw_info(frame, can_detected, best_can, can_center_x, can_area, can_aspect_ratio, 
                         (GPIO.input(CAN_LEFT_PIN), GPIO.input(CAN_RIGHT_PIN), 
                          GPIO.input(CAN_FOUND_PIN), GPIO.input(ARRIVED_AT_CAN_PIN)))
        
        # Display the frame
        cv2.imshow('Coke Can Detection System', frame)
        
        # Optional: Show the mask for debugging
        # cv2.imshow('Red Mask', red_mask)
        
        # Handle key presses
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break
        elif key == ord('r'):
            # Reset all GPIO pins
            for pin in [CAN_LEFT_PIN, CAN_RIGHT_PIN, CAN_FOUND_PIN, ARRIVED_AT_CAN_PIN]:
                GPIO.output(pin, GPIO.LOW)
            print("GPIO pins reset to LOW")
        elif key == ord('+'):
            ARRIVAL_AREA += 1000
            print(f"Arrival area threshold increased to: {ARRIVAL_AREA}")
        elif key == ord('-'):
            ARRIVAL_AREA = max(1000, ARRIVAL_AREA - 1000)
            print(f"Arrival area threshold decreased to: {ARRIVAL_AREA}")

finally:
    # Cleanup
    cv2.destroyAllWindows()
    for pin in [CAN_LEFT_PIN, CAN_RIGHT_PIN, CAN_FOUND_PIN, ARRIVED_AT_CAN_PIN]:
        GPIO.output(pin, GPIO.LOW)
    GPIO.cleanup()
    print("\nGPIO cleaned up. System shutdown.")
