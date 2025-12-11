# Robot Build Guide 
BRAB PUTT
## Introduction 
A compact autonomous robot designed to assist with outdoor litter collection.
#### Purpose of this guide 
This guide demonstrates how to build BRAB PUTT, the small bi-terrain rover designed to find aluminium drink cans using machine vision and simple motor controls.
## Parts and Materials 
### Electronic Components 
•	1 × Arduino Mega 2560
•	1 × Pixy2 camera
•	2 × L298N motor drivers
•	4 × 12 V worm-gear DC motors. Specifically the JVY-370
•	1 × 12 V NiMH battery
•	Jumper wires
•	Power switch
•	Breadboard
Power bank

### Mechanical Components
•	Acrylic chassis plates (210 × 210 mm)
•	PLA 3D printed brackets[]()
•	Off-road RC wheels[]()
•	Bolts, spacers, nuts
•	Mount for Raspberry PI and Raspberry Pi camera module 3
### Tools
3D Printer
screw driver for bolts and wrench for nuts.
Wire strippers
hot gun gun
laser cutter
Multi-meter
## Chassis construction
### Cut the Acrylic
here is the dxf file[]()
Using the laser printer, cut the panel using the DXF file as a reference. The whole grid is provided in the DXF file.
### Assemble the frame
3D print this shell[]() using your 3d printer of choice. Ensuring it has a base plate of at least 215mm. It comes with mounting holes/for the specified encoder motor. When it is printed, you should be able to screw it into place.
After the print is finished, fit your acrylic board where all of your components will sit, then screw in your motors.
## Wiring
A wiring diagram for the Arduino and motors
For the Arduino to the Raspberry Pi, the code used pins 17,27,22,23
Don't forget to make sure the Arduino and Pi have a common ground, otherwise you will have issues when it comes to reading the PI values
## Software Setup
Install the following libraries:
Arduino IDE, encoder library by Paul Stroffgen []()
### Uploading the code
Upload the Arduino IDE code while plugged into the laptop and selecting the correct port. []()
### Uploading the code from Raspberry
Upload the Raspberry PI code provided []()
### CALIBRATION

## Voltages
12V 5A constant supply to motors.
1 12V battery will power all motors.
5 volts and 5 amps from a power bank to the Raspberry Pi.
The Raspberry Pi can then power the Arduino through USB
## Testing the robot
Whilst connected to a power supply with the power supply turned off, upload the code, then once uploaded, plug remove the arduino plug from your laptop into the power supply if it has a USB port. Then turn the power supply on to test. This is what I found to be the safest way to test code by uploading from your laptop.
If any componets aren't powering on when they should be use a multimeter and test for continuity.
### initial testing
Place the robot on a stand where you can see the wheels move without it running away from you, to test very specific features in a safe, controlled environment.
### Indoor testing
Now using a battery instead of a power supply and a power bank for the Arduino or Raspberry Pi. It should move around freely and provide deeper insights for testing further complexities.
### Outdoor testing
Test on a relatively flat and controlled area, and progress down to harsher, more uncontrollable environments to further your robots capabilities.
## Troubleshooting
Multi-meter continuity test on all components and wires powering them to ensure they are receiving power.
Double-checking wires to ensure they're in the correct placement.
For the Arduino and Raspberry Pi, adding serial printlns helps with debugging and finding if certain functions are getting reached by the code.
Breaking it down to trialling specific functions and then rebuilding it with a deeper understanding.
For the Raspberry Pi, the info should be shown on the screen so you can test if the info is correct
There are also testing functions for the Arduino, so if needed, it is possible to use those code snippets to find any issues
# Safety Notes
Do not cross wires from positive to negative in batteries, as it will cause a short circuit. This will short your device, and it will become unusable.
Try not to leave your computer plugged into the Arduino while the battery is plugged in for testing, as it can send high voltage used to power the motors back into your computer and short your capacitors which will, at best, result in faulty ports and at worst, corrupt your laptop.
