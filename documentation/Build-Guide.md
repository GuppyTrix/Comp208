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

[the can for something]()
### Mechanical Components
•	Acrylic chassis plates (210 × 210 mm)
•	PLA 3D printed brackets[]()
•	Off-road RC wheels[]()
•	Bolts, spacers, nuts
•	Mount for Raspberry PI and raspberry pi camera module 3
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
Using the laser printer cut the panel using the dxf file as reference. The holes grid is provided in the dxf file.
### Assemble the frame
3D print this shell[]() using your 3d printer of choice. Ensuring it has a base plate of atleast 215mm. It comes with mounting holes/for the specified encoder motor. When it is printed you should be able to screw it into place.
After the print is finished fit your acrylic board where all of your components will sit then screw in your motors.
## Wiring
A wiring diagram, for the arduino and motors
now a diagram from the arduino to the raspberry.
## Software Setup
Install the following libraries:
Arduino IDE, RASPBERRY PI, encoder library by Paul Stroffgen []()
### Uploading the code
Upload the arduino IDE code whilst plugged into the laptop and selecting the correct port. []()
### Uploading the code from Raspberry
Upload the Raspberry PI code provided []()
### CALIBRATION

## Voltages
12 volt 5 amps constant supply to motors.
1 12 volt battery will power all motors.
5 volts and 5 amps from a power bank to raspberry PI.
the raspberry pi can then power the arduino through that voltage.
## Testing the robot
Whilst connected to a power supply with the power supply turned off upload the code, then once uploaded plug remove the arduino plug from your laptop into the power supply if it has the USB port. Then turn the power supply on to test. This is what I found to be the most safe way to test code with uploading from your laptop.
If any componets aren't powering on when they should be use a multimeter and test for continuity.
### initial testing
place the robot on a stand where you can see the wheels move without it running away from you to test very specific features in a safe controlled environment.
### Indoor testing
Now using a battery instead of a power supply and a power bank for the arduino or raspberry PI. It should move around freely and provided deeper insights for testing further complexities.
### Outdoor testing
Test on a relatively flat and controlled area, and progress down to harsher more uncontrollable environments to further your robots capabilities.
## Troubleshooting
Multi-meter continuity test on all components and wires powering them to ensure they are receiveing power.
Double checking wires to ensure they're in correct placement.
Arduino and raspberry PI adding serial.printlns help with debugging and finding if certain functions are getting reached by the code.
Breaking it down to trialing specific functions and then rebuilding it with a deeper understanding.
# Safety Notes
Do not cross wires from positive to negatives in batteries or 5v from arduino. This will short your device and it will become unusable.
Try not to leave your computer plugged into the arduino for testing as it can send higher voltage for powering the motors back into your computer and short your capacitors which will at the least result in faulty ports and at worst corrupt your laptop.
