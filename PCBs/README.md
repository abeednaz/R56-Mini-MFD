# PCB Overview

## Connector Board
The connector board connects to the J1962 connector under the vehicle's steering column. Pinout of the vehicle's connector:
![](https://www.flexihub.com/images/upload/flexihub/articles/diagnostics/pinout/bmw_obd2.png)

The connector board reroutes the battery power (pin 16) through a MOSFET switch triggered by the ignition signal (pin 1). The 12V switched power line is distributed to an ELM327 OBD-II code scanner, a 12V DC power connector, and a 12V--5V buck converter to power the gauge electronics. This allows the code scanner and gauge to only be switched on while the vehicle is in accesory mode or the engine is running, reducing the parasitic battery drain while the vehicle is off to ~145 nW.

## Connector board v2.7 rework:
Two components (SW1 and Q2) were captured incorrectly in the schematic and PCB. The rework instructions are as follows:
1. Solder wire between P1 and P2
2. Cut trace at X1
3. Cut trace at X2
4. Solder wire between P3 and P4
5. Solder wire between P5 and P6

 PCB Front                 | PCB Back
:-------------------------:|:-------------------------:
![](/Reference/readme_assets/conn_board_2-7_rework_front.jpg)  |  ![](/Reference/readme_assets/conn_board_2-7_rework_back.jpg)

## Assembly photos
 Soldered assembly front   | Soldered assembly back    | With connector
:-------------------------:|:-------------------------:|:--------------:|
![](/Reference/readme_assets/conn_board_2-7_pic1.JPG)  |  ![](/Reference/readme_assets/conn_board_2-7_pic2.JPG) | ![](/Reference/readme_assets/conn_board_2-7_pic3.JPG)

 Enclosure inside view     | Assembled in enclosure    | Assembled in enclosure
:-------------------------:|:-------------------------:|:----------------------:|
![](/Reference/readme_assets/conn_board_2-7_assembled1.JPG) | ![](/Reference/readme_assets/conn_board_2-7_assembled2.JPG) | ![](/Reference/readme_assets/conn_board_2-7_assembled3.JPG)
