# Mechatronics_Project
Rubiks Race solving by 3 DOF Arm

https://www.hasbro.com/common/documents/dad2884a1c4311ddbd0b0800200c9a66/D597E72E5056900B10F8D850BDEA6516.pdf

&nbsp;
___
# Tasks:
1. Build and 3D print a servo-controlled robotic arm, and design an embedded system for its control.
2. Design and construct a color sensor using electronic components.
3. Create an algorithm to solve the puzzle and determine the necessary movements for the robotic arm.

&nbsp;
<br>

## What is Rubiks Race Puzzle and How to play? (Actual Puzzle)
<p align="center">
  <img src="images/Rubiks_Race_puzzle.png" alt="Image 1" width ="550">
</p>

<br>
<br>


<!-- ====================================================================================================== -->

## 1. Build and 3D print a servo-controlled robotic arm, and design an embedded system for its control.
Components:<br>
  1. Servo MG-996R  * 1
  2. Servo SG90 9G Micro Servo  * 2
  3. Arduino Uno * 1
  4. Arduino Proto Shield * 1
  5. Robotic Arm : [download the model here](https://a360.co/3R4SsM0)
  6. M4 Screws and nuts * 4
  7. M3 screws and nuts * 6

     
<p align="center">
  <img src="images/Arm_assembly.png" alt="Image 1" height ="300">
  <img src="images/exploded_view.png" alt="Image 1" height ="300">
</p>

Assembly Instructions:<br>
* Attach Servo MG-996R with M4 screws
* fix the base with servo assembly on top of it using 4 M3 screws and bolts.
* Assemble an Arm with servo 1 as shown in picture. Assemble such that each servo can operate in limits shown in image.
<p align="center">
  <img src="images/17021268612185707055149107339741.jpg" alt="Image 1" height ="300">
  <img src="images/top_view.png" alt="Image 1" height ="300">
  <img src="images/front_view.png" alt="Image 1" height ="300">
</p>

  

<!-- ====================================================================================================== -->

## 2. Design and construct a color sensor using electronic components.

Components:<br>
  1. RGB Led * 1
  2. [5528 LDR Mini Photoresistor](https://cdn11.bigcommerce.com/s-yo2n39m6g3/images/stencil/1280x1280/products/704/6303/d9c8d789-7c9b-42a2-af7b-5ad5c29b4def__71085.1632206657.JPG?c=2?imbypass=on) * 1
  3. Resistor 10 kOhm * 1
  4. Resistor 220 Ohm * 3
  5. [prototype board](https://protosupplies.com/wp-content/uploads/2019/02/PCB-2x8-cm-Universal-PCB-Board.jpg)
  6. [Rubber Finger](https://m.media-amazon.com/images/W/MEDIAX_792452-T1/images/I/71JWB0M2+lL._AC_SX679_.jpg)

Assembly instructions:
* [Solder RGB LED and LDR on proto board as shown in fig.](images/17021272272572036238491611179348.jpg)
* [Extend wires towards Arduino proto shield and solder female connector at end](images/20231209_082113.jpg)
* Solder 10k resitor and 220 Ohm resistors on Arduino proto shield as shown in schematic
* Connect :
  + servo -> Arm 1 -> Arduino D9
  + servo -> Arm 2 -> Arduino D10
  + servo -> Arm 2 -> Arduino D11
  + [Sensor connector on proto shield as shown in picture](images/20231209_083949.jpg)
              
  <p align="center">
    <img src="images/17021272272572036238491611179348.jpg" alt="Image 1" height ="150">
    <img src="images/Sensor.jpg" alt="Image 1" height ="150">
    <img src="images/sensor2.jpg" alt="Image 1" height ="150">
    <img src="images/20231209_082113.jpg" alt="Image 1" height ="150">
    <img src="images/20231209_081609.jpg" alt="Image 1" height ="150">
    <img src="images/20231209_083949.jpg" alt="Image 1" height ="150">
    
  </p>
<br>
<br>
<br>
<!-- ------------------------------------------------------------------------------------------------------------------------------------------------------- -->

## How color detection works:

* Color starts with light. Light is a form of electromagnetic radiation, and it travels in waves. Different colors correspond to different wavelengths of light. When light interacts with an object, some wavelengths are absorbed by the object, and others are reflected.
<p align="center">
    <img src="https://www.color-meanings.com/wp-content/uploads/absorption-reflection-colors-objects-1024x858.png" alt="Image 1" height ="200">
</p>


* In this project, RGB LED diod is used as a light source [datasheet](https://cdn.sparkfun.com/datasheets/Components/LED/YSL-R596AR3G4B5C-C10.pdf?_gl=1*135yzks*_ga*MjA4NTU0MjE3NC4xNjk4MDI2ODM3*_ga_T369JS7J9N*MTcwMjEzMzIzMi41LjAuMTcwMjEzMzIzMi42MC4wLjA.)
  <p align="center">
    <img src="images/RGB_LED.png" alt="Image 1" height ="200">
    <img src="https://www.thegeekpub.com/wp-content/uploads/2021/11/Arduino-RGB-LED-0003-RGB-LED-switching-colors-animated.gif" alt="Image 1" height ="200">
  </p>
---
  

* A photoresistor, composed of semiconductor materials like cadmium sulfide, changes its electrical resistance based on the intensity of incident light. When exposed to light, it generates electron-hole pairs, altering its conductivity. The resistance is inversely proportional to light intensity, decreasing as light increases. Photoresistors are used in circuits, often arranged in voltage dividers, to detect or control light levels. For detail information click [here](https://www.circuitstoday.com/wp-content/uploads/2017/10/Photoresistor.jpg).

  <p align="center">
    <img src="https://www.circuitstoday.com/wp-content/uploads/2017/10/Photoresistor-Working-Priciple-e1507308614699.png" alt="Image 1" height ="200">
    <img src="https://www.circuitstoday.com/wp-content/uploads/2017/10/Photoresistor.jpg" alt="Image 1" height ="200">
    <img src="https://adam-meyer.com/arduino/images/2012/11/arduino-LDR-photoresistor1.png" alt="Image 1" height ="200">
    
    
  </p>
  
---
<br>
* RGB LED Flashes Each color for 30ms. At same time Arduino reads Analog voltage on pin A0(where sensor connected)
<p align="center">
  <img src="images/20231209_083137_AdobeExpress.gif" alt="Image 1" height ="200">
</p>





&nbsp;
<br>
___
## Circuit Diagram:
<p align="center">
    <img src="images/circuit.png" alt="Image 1" height ="400">
  </p>

Calibration of color sensor:
* Sensor calibrates starting of each cycle:
*   <p align="center">
    <img src="images/20231209_083137_AdobeExpress.gif" alt="Image 1" height ="200">
    </p>







## 3.Create an algorithm to solve the puzzle and determine the necessary movements for the robotic arm.

* [Download the code in src folder of this repo.](src/main.cpp)
* Paste it in your project fodlder as main.cpp
* upload it on arduino Uno.

<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>

---
---
---
# Section 1

This is the content of section 1.

<!-- Add a horizontal rule for visual separation -->
---
<!-- Or, add an empty line for additional space -->
&nbsp;
<!-- Or, use HTML <br> for line break -->
<br>

# Section 2

This is the content of section 2.


This is a line without indentation.

    This is indented using spaces.

&emsp;This is indented using an em space in HTML.


