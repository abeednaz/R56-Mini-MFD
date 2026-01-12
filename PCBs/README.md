# PCB Overview

## OBD-II Connector Board
The connector board connects to the J1962 connector under the vehicle's steering column. Pinout of the vehicle's connector:
![](https://www.flexihub.com/images/upload/flexihub/articles/diagnostics/pinout/bmw_obd2.png)

The connector board reroutes the battery power (pin 16) through a MOSFET switch triggered by the ignition signal (pin 1). The 12V switched power line is distributed to an ELM327 OBD-II code scanner, a 12V DC power connector, and a 12V--5V buck converter to power the gauge electronics. This allows the code scanner and gauge to only be switched on while the vehicle is in accesory mode or the engine is running, reducing the parasitic battery drain while the vehicle is off to ~145 nW.

<img width="1126" height="613" alt="image" src="https://github.com/user-attachments/assets/76d4fc67-11f6-4b22-b1f7-19c3990ccc69" />

### Connector board v2.7 rework:
Two components (SW1 and Q2) were captured incorrectly in the schematic and PCB. The rework instructions are as follows:
1. Remove SW1 and solder wire between P1 and P2
2. Cut trace at X1
3. Cut trace at X2
4. Solder wire between P3 and P4
5. Solder wire between P5 and P6

 PCB Front                 | PCB Back
:-------------------------:|:-------------------------:
![conn_board_2-7_rework_front](https://github.com/user-attachments/assets/37f411a1-0201-4ad1-990e-5af38816a7ad)  |  ![conn_board_2-7_rework_back](https://github.com/user-attachments/assets/c54043fa-390c-4bff-82e2-e3fb1493af04)


### Assembly photos
 Soldered assembly front   | Soldered assembly back    | With connector
:-------------------------:|:-------------------------:|:--------------:|
![conn_board_2-7_pic1](https://github.com/user-attachments/assets/715eb6d9-4ef1-44b0-b6fc-5ad28d48f5e3) |  ![conn_board_2-7_pic2](https://github.com/user-attachments/assets/92b2965d-94a5-47d5-9377-c2f4f990aa5b) | ![conn_board_2-7_pic3](https://github.com/user-attachments/assets/6e2573d1-0ce8-4bc3-ab32-b2713f89954b)


## Screen Accessory Board
The screen used for this project is an off-the-shelf solution from [Waveshare](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.75#Hardware_Description). It is a fully-contained solution with an ESP32-S3 microprocesor and a high-resolution AMOLED screen in addition to a PMIC and IMU. To adapt it for the gauge, I needed to add a button to allow a user to switch between gauges, and I needed a sensor to check the ambient light level to dim the display at night. I also needed a way to switch the gauge on and off, so I moved the switch from the OBD-II distribution board to the screen accessory board.

<img width="1089" height="610" alt="image" src="https://github.com/user-attachments/assets/46032e79-35f6-4d50-bd6c-2d0849a12315" />

 PCB Front                 | PCB Back
:-------------------------:|:-------------------------:
<img width="686" height="774" alt="image" src="https://github.com/user-attachments/assets/507d2b83-08dc-445f-95d9-1f427283e758" /> | <img width="686" height="774" alt="image" src="https://github.com/user-attachments/assets/43ea4ef5-4b37-4c84-a90f-661aa8be7f55" />

### Assembly photos
 Soldered assembly front   | Soldered assembly back    
:-------------------------:|:-------------------------:
![screen_board_1-0_assembly2](https://github.com/user-attachments/assets/3145db14-239d-4fa5-a45d-89f6f4f1d4cd) |  ![screen_board_1-0_assembly1](https://github.com/user-attachments/assets/7593e51b-8adf-49f3-9ed3-740d539ebd6d)

