EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Transistor_FET:IRF540N Q1
U 1 1 66988AC0
P 4100 3000
F 0 "Q1" H 4304 3046 50  0000 L CNN
F 1 "IRF540N" H 4304 2955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 4350 2925 50  0001 L CIN
F 3 "http://www.irf.com/product-info/datasheets/data/irf540n.pdf" H 4100 3000 50  0001 L CNN
	1    4100 3000
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:IRF9540N Q2
U 1 1 66989B4F
P 4200 2000
F 0 "Q2" H 4404 2046 50  0000 L CNN
F 1 "IRF9540N" H 4404 1955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 4400 1925 50  0001 L CIN
F 3 "http://www.irf.com/product-info/datasheets/data/irf9540n.pdf" H 4200 2000 50  0001 L CNN
	1    4200 2000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R1
U 1 1 6698BDA7
P 3700 2300
F 0 "R1" H 3632 2254 50  0000 R CNN
F 1 "10k" H 3632 2345 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3740 2290 50  0001 C CNN
F 3 "~" H 3700 2300 50  0001 C CNN
	1    3700 2300
	-1   0    0    1   
$EndComp
Text GLabel 3200 3000 0    50   Input ~ 0
V2
Text GLabel 3200 1900 0    50   Input ~ 0
V1
Wire Wire Line
	4000 1900 3700 1900
Wire Wire Line
	3700 1900 3700 2150
Connection ~ 3700 2150
Wire Wire Line
	3700 2150 3700 2200
Wire Wire Line
	3700 2400 3700 2450
Wire Wire Line
	3700 2700 4200 2700
Wire Wire Line
	4200 2700 4200 2200
Connection ~ 3700 2450
Wire Wire Line
	3700 2450 3700 2700
Connection ~ 4200 2700
Wire Wire Line
	3700 1900 3200 1900
Connection ~ 3700 1900
Text GLabel 4700 1900 2    50   Output ~ 0
V3
Wire Wire Line
	4200 2700 4200 2800
$Comp
L Device:R_US R2
U 1 1 669991E9
P 3700 3300
F 0 "R2" H 3632 3254 50  0000 R CNN
F 1 "1Meg" H 3632 3345 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3740 3290 50  0001 C CNN
F 3 "~" H 3700 3300 50  0001 C CNN
	1    3700 3300
	-1   0    0    1   
$EndComp
Wire Wire Line
	3700 3000 3900 3000
Wire Wire Line
	3700 3000 3700 3150
Connection ~ 3700 3150
Wire Wire Line
	3700 3150 3700 3200
Wire Wire Line
	4200 3200 4200 3700
Wire Wire Line
	3700 3400 3700 3450
Wire Wire Line
	3700 3700 4200 3700
Connection ~ 3700 3450
Wire Wire Line
	3700 3450 3700 3700
Wire Wire Line
	3700 3000 3200 3000
Connection ~ 3700 3000
Connection ~ 4200 3700
$Comp
L power:GND #PWR0101
U 1 1 6699CB3D
P 4200 3900
F 0 "#PWR0101" H 4200 3650 50  0001 C CNN
F 1 "GND" H 4205 3727 50  0000 C CNN
F 2 "" H 4200 3900 50  0001 C CNN
F 3 "" H 4200 3900 50  0001 C CNN
	1    4200 3900
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x08_Top_Bottom J1
U 1 1 6699D9BD
P 1500 2400
F 0 "J1" H 1550 2917 50  0000 C CNN
F 1 "OBD Input" H 1550 2826 50  0000 C CNN
F 2 "Connector_JST:JST_PUD_B16B-PUDSS_2x08_P2.00mm_Vertical_Renumbered" H 1500 2400 50  0001 C CNN
F 3 "~" H 1500 2400 50  0001 C CNN
	1    1500 2400
	1    0    0    -1  
$EndComp
Text GLabel 2600 2800 2    50   Output ~ 0
V1
Text GLabel 1200 2100 0    50   Output ~ 0
V2
Wire Wire Line
	1300 2100 1200 2100
Text GLabel 1200 2200 0    50   Output ~ 0
OBD_Pin2
Wire Wire Line
	1300 2200 1200 2200
Text GLabel 1200 2300 0    50   Output ~ 0
OBD_Pin3
Wire Wire Line
	1300 2300 1200 2300
Text GLabel 1200 2500 0    50   Output ~ 0
OBD_Pin5
Wire Wire Line
	1300 2500 1200 2500
Text GLabel 1200 2600 0    50   Output ~ 0
OBD_Pin6
Wire Wire Line
	1300 2600 1200 2600
Text GLabel 1200 2700 0    50   Output ~ 0
OBD_Pin7
Wire Wire Line
	1300 2700 1200 2700
Text GLabel 1200 2800 0    50   Output ~ 0
OBD_Pin8
Wire Wire Line
	1300 2800 1200 2800
Text GLabel 1900 2700 2    50   Output ~ 0
OBD_Pin15
Wire Wire Line
	1900 2700 1800 2700
Text GLabel 1900 2600 2    50   Output ~ 0
OBD_Pin14
Wire Wire Line
	1900 2600 1800 2600
Text GLabel 1900 2500 2    50   Output ~ 0
OBD_Pin13
Wire Wire Line
	1900 2500 1800 2500
Text GLabel 1900 2400 2    50   Output ~ 0
OBD_Pin12
Wire Wire Line
	1900 2400 1800 2400
Text GLabel 1900 2300 2    50   Output ~ 0
OBD_Pin11
Wire Wire Line
	1900 2300 1800 2300
Text GLabel 1900 2200 2    50   Output ~ 0
OBD_Pin10
Wire Wire Line
	1900 2200 1800 2200
Text GLabel 1900 2100 2    50   Output ~ 0
OBD_Pin9
Wire Wire Line
	1900 2100 1800 2100
Wire Wire Line
	1300 3400 1200 3400
Wire Wire Line
	1900 4100 1800 4100
Wire Wire Line
	1300 3500 1200 3500
Wire Wire Line
	1300 3600 1200 3600
Wire Wire Line
	1300 3800 1200 3800
Wire Wire Line
	1300 3900 1200 3900
Wire Wire Line
	1300 4000 1200 4000
Text GLabel 1200 4100 0    50   Input ~ 0
OBD_Pin8
Wire Wire Line
	1300 4100 1200 4100
Wire Wire Line
	1900 4000 1800 4000
Wire Wire Line
	1900 3900 1800 3900
Wire Wire Line
	1900 3800 1800 3800
Wire Wire Line
	1900 3700 1800 3700
Wire Wire Line
	1900 3600 1800 3600
Wire Wire Line
	1900 3500 1800 3500
Wire Wire Line
	1900 3400 1800 3400
Text GLabel 1200 4000 0    50   Input ~ 0
OBD_Pin7
Text GLabel 1200 3900 0    50   Input ~ 0
OBD_Pin6
Text GLabel 1200 3800 0    50   Input ~ 0
OBD_Pin5
Text GLabel 1200 3600 0    50   Input ~ 0
OBD_Pin3
Text GLabel 1200 3500 0    50   Input ~ 0
OBD_Pin2
Text GLabel 1200 3400 0    50   Input ~ 0
V2
Text GLabel 1900 3400 2    50   Input ~ 0
OBD_Pin9
Text GLabel 1900 3500 2    50   Input ~ 0
OBD_Pin10
Text GLabel 1900 3600 2    50   Input ~ 0
OBD_Pin11
Text GLabel 1900 3700 2    50   Input ~ 0
OBD_Pin12
Text GLabel 1900 3800 2    50   Input ~ 0
OBD_Pin13
Text GLabel 1900 3900 2    50   Input ~ 0
OBD_Pin14
Text GLabel 1900 4000 2    50   Input ~ 0
OBD_Pin15
Text GLabel 1900 4100 2    50   Input ~ 0
V3
Wire Wire Line
	4700 1900 4400 1900
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 669BF845
P 8300 1700
F 0 "J3" H 8328 1676 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8328 1585 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 8328 1539 50  0001 L CNN
F 3 "~" H 8300 1700 50  0001 C CNN
	1    8300 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 669C909D
P 8000 1900
F 0 "#PWR0102" H 8000 1650 50  0001 C CNN
F 1 "GND" H 8005 1727 50  0000 C CNN
F 2 "" H 8000 1900 50  0001 C CNN
F 3 "" H 8000 1900 50  0001 C CNN
	1    8000 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 1900 8000 1800
Wire Wire Line
	8000 1800 8100 1800
Wire Wire Line
	8100 1700 8000 1700
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 669D44DA
P 8300 2300
F 0 "J4" H 8328 2276 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8328 2185 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 8328 2139 50  0001 L CNN
F 3 "~" H 8300 2300 50  0001 C CNN
	1    8300 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 669D44E5
P 8000 2500
F 0 "#PWR0103" H 8000 2250 50  0001 C CNN
F 1 "GND" H 8005 2327 50  0000 C CNN
F 2 "" H 8000 2500 50  0001 C CNN
F 3 "" H 8000 2500 50  0001 C CNN
	1    8000 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 2500 8000 2400
Wire Wire Line
	8000 2400 8100 2400
Wire Wire Line
	8100 2300 8000 2300
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 669D7051
P 8300 2900
F 0 "J5" H 8328 2876 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8328 2785 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 8328 2739 50  0001 L CNN
F 3 "~" H 8300 2900 50  0001 C CNN
	1    8300 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 669D7058
P 8000 3100
F 0 "#PWR0104" H 8000 2850 50  0001 C CNN
F 1 "GND" H 8005 2927 50  0000 C CNN
F 2 "" H 8000 3100 50  0001 C CNN
F 3 "" H 8000 3100 50  0001 C CNN
	1    8000 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 3100 8000 3000
Wire Wire Line
	8000 3000 8100 3000
Wire Wire Line
	8100 2900 8000 2900
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 669D8B09
P 8300 3500
F 0 "J6" H 8328 3476 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8328 3385 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 8328 3339 50  0001 L CNN
F 3 "~" H 8300 3500 50  0001 C CNN
	1    8300 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 669D8B10
P 8000 3700
F 0 "#PWR0105" H 8000 3450 50  0001 C CNN
F 1 "GND" H 8005 3527 50  0000 C CNN
F 2 "" H 8000 3700 50  0001 C CNN
F 3 "" H 8000 3700 50  0001 C CNN
	1    8000 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 3700 8000 3600
Wire Wire Line
	8000 3600 8100 3600
Wire Wire Line
	8100 3500 8000 3500
$Comp
L Regulator_Linear:L7805 U1
U 1 1 669EA3BD
P 6200 1900
F 0 "U1" H 6200 2142 50  0000 C CNN
F 1 "L7805" H 6200 2051 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 6225 1750 50  0001 L CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/41/4f/b3/b0/12/d4/47/88/CD00000444.pdf/files/CD00000444.pdf/jcr:content/translations/en.CD00000444.pdf" H 6200 1850 50  0001 C CNN
	1    6200 1900
	1    0    0    -1  
$EndComp
Text GLabel 5500 1900 0    50   Input ~ 0
V3
$Comp
L Device:C C2
U 1 1 669F3D1F
P 6700 2300
F 0 "C2" H 6815 2346 50  0000 L CNN
F 1 "0.1uF" H 6815 2255 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_Tantal_D4.5mm_P2.50mm" H 6815 2209 50  0001 L CNN
F 3 "~" H 6700 2300 50  0001 C CNN
	1    6700 2300
	1    0    0    -1  
$EndComp
Text GLabel 6900 1900 2    50   Output ~ 0
V4
Connection ~ 5700 1900
Wire Wire Line
	6500 1900 6700 1900
Wire Wire Line
	6700 1900 6700 2150
Connection ~ 6700 1900
Wire Wire Line
	6700 1900 6900 1900
Connection ~ 6700 2150
Wire Wire Line
	6700 2150 6700 2200
Wire Wire Line
	5700 2600 6200 2600
Wire Wire Line
	6700 2400 6700 2450
Connection ~ 6700 2450
Wire Wire Line
	6700 2450 6700 2600
Wire Wire Line
	6200 2200 6200 2600
Connection ~ 6200 2600
Wire Wire Line
	6200 2600 6700 2600
Wire Wire Line
	6200 2600 6200 2700
$Comp
L power:GND #PWR0106
U 1 1 66A08B2C
P 6200 2700
F 0 "#PWR0106" H 6200 2450 50  0001 C CNN
F 1 "GND" H 6205 2527 50  0000 C CNN
F 2 "" H 6200 2700 50  0001 C CNN
F 3 "" H 6200 2700 50  0001 C CNN
	1    6200 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:D D1
U 1 1 66A0A9F6
P 6200 1400
F 0 "D1" H 6200 1617 50  0000 C CNN
F 1 "D" H 6200 1526 50  0000 C CNN
F 2 "Diode_THT:D_DO-15_P12.70mm_Horizontal" H 6200 1400 50  0001 C CNN
F 3 "~" H 6200 1400 50  0001 C CNN
	1    6200 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 1400 6350 1400
Wire Wire Line
	6700 1400 6700 1900
Connection ~ 6350 1400
Wire Wire Line
	6350 1400 6700 1400
Wire Wire Line
	5700 1900 5700 1400
Wire Wire Line
	5700 1400 6050 1400
Connection ~ 6050 1400
Wire Wire Line
	6050 1400 6100 1400
Wire Wire Line
	5700 1900 5700 2150
$Comp
L Device:C C1
U 1 1 66A14550
P 5700 2300
F 0 "C1" H 5815 2346 50  0000 L CNN
F 1 "0.33uF" H 5815 2255 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_Tantal_D4.5mm_P2.50mm" H 5815 2209 50  0001 L CNN
F 3 "~" H 5700 2300 50  0001 C CNN
	1    5700 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2400 5700 2450
Connection ~ 5700 2450
Wire Wire Line
	5700 2450 5700 2600
Text Notes 600  1100 0    50   ~ 0
Legend:\nV1 - Car battery 12Vdc nominal\nV2 - Ignition signal (12V on, 0V off)\nV3 - Car battery 12Vdc nominal, switched by V2 (12V on, 0V off)\nV4 - 5Vdc supply from 7805\nV5 - 3.3Vdc supply from uC
Wire Wire Line
	600  2400 1300 2400
$Comp
L power:GND #PWR0107
U 1 1 66A2C1D8
P 600 3000
F 0 "#PWR0107" H 600 2750 50  0001 C CNN
F 1 "GND" H 605 2827 50  0000 C CNN
F 2 "" H 600 3000 50  0001 C CNN
F 3 "" H 600 3000 50  0001 C CNN
	1    600  3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	600  2400 600  3000
Wire Wire Line
	4200 3700 4200 3900
$Comp
L power:GND #PWR0108
U 1 1 66A3FA51
P 600 4300
F 0 "#PWR0108" H 600 4050 50  0001 C CNN
F 1 "GND" H 605 4127 50  0000 C CNN
F 2 "" H 600 4300 50  0001 C CNN
F 3 "" H 600 4300 50  0001 C CNN
	1    600  4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	600  3700 600  4300
Wire Wire Line
	5700 1900 5900 1900
Wire Wire Line
	5500 1900 5700 1900
Text GLabel 8000 3500 0    50   Input ~ 0
V4
Text GLabel 8000 2900 0    50   Input ~ 0
V4
Text GLabel 8000 2300 0    50   Input ~ 0
V4
Text GLabel 8000 1700 0    50   Input ~ 0
V4
Connection ~ 5700 2150
Wire Wire Line
	5700 2150 5700 2200
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 66A8B865
P 9800 1700
F 0 "J9" H 9828 1676 50  0000 L CNN
F 1 "Conn_01x02_Female" H 9828 1585 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 9828 1539 50  0001 L CNN
F 3 "~" H 9800 1700 50  0001 C CNN
	1    9800 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 66A8B86B
P 9500 1900
F 0 "#PWR0110" H 9500 1650 50  0001 C CNN
F 1 "GND" H 9505 1727 50  0000 C CNN
F 2 "" H 9500 1900 50  0001 C CNN
F 3 "" H 9500 1900 50  0001 C CNN
	1    9500 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 1900 9500 1800
Wire Wire Line
	9500 1800 9600 1800
Wire Wire Line
	9600 1700 9500 1700
$Comp
L Connector:Conn_01x02_Female J10
U 1 1 66A8B874
P 9800 2300
F 0 "J10" H 9828 2276 50  0000 L CNN
F 1 "Conn_01x02_Female" H 9828 2185 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 9828 2139 50  0001 L CNN
F 3 "~" H 9800 2300 50  0001 C CNN
	1    9800 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 66A8B87A
P 9500 2500
F 0 "#PWR0111" H 9500 2250 50  0001 C CNN
F 1 "GND" H 9505 2327 50  0000 C CNN
F 2 "" H 9500 2500 50  0001 C CNN
F 3 "" H 9500 2500 50  0001 C CNN
	1    9500 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 2500 9500 2400
Wire Wire Line
	9500 2400 9600 2400
Wire Wire Line
	9600 2300 9500 2300
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 66A8B883
P 9800 2900
F 0 "J11" H 9828 2876 50  0000 L CNN
F 1 "Conn_01x02_Female" H 9828 2785 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 9828 2739 50  0001 L CNN
F 3 "~" H 9800 2900 50  0001 C CNN
	1    9800 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 66A8B889
P 9500 3100
F 0 "#PWR0112" H 9500 2850 50  0001 C CNN
F 1 "GND" H 9505 2927 50  0000 C CNN
F 2 "" H 9500 3100 50  0001 C CNN
F 3 "" H 9500 3100 50  0001 C CNN
	1    9500 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 3100 9500 3000
Wire Wire Line
	9500 3000 9600 3000
Wire Wire Line
	9600 2900 9500 2900
$Comp
L Connector:Conn_01x02_Female J12
U 1 1 66A8B892
P 9800 3500
F 0 "J12" H 9828 3476 50  0000 L CNN
F 1 "Conn_01x02_Female" H 9828 3385 50  0000 L CNN
F 2 "Connector_JST:JST_EH_S2B-EH_1x02_P2.50mm_Horizontal" H 9828 3339 50  0001 L CNN
F 3 "~" H 9800 3500 50  0001 C CNN
	1    9800 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 66A8B898
P 9500 3700
F 0 "#PWR0113" H 9500 3450 50  0001 C CNN
F 1 "GND" H 9505 3527 50  0000 C CNN
F 2 "" H 9500 3700 50  0001 C CNN
F 3 "" H 9500 3700 50  0001 C CNN
	1    9500 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 3700 9500 3600
Wire Wire Line
	9500 3600 9600 3600
Wire Wire Line
	9600 3500 9500 3500
Text GLabel 9500 3500 0    50   Input ~ 0
V5
Text GLabel 9500 2900 0    50   Input ~ 0
V5
Text GLabel 9500 2300 0    50   Input ~ 0
V5
Text GLabel 9500 1700 0    50   Input ~ 0
V5
Text GLabel 900  6900 0    50   Input ~ 0
V4
$Comp
L power:GND #PWR0109
U 1 1 66B2633C
P 900 7200
F 0 "#PWR0109" H 900 6950 50  0001 C CNN
F 1 "GND" H 905 7027 50  0000 C CNN
F 2 "" H 900 7200 50  0001 C CNN
F 3 "" H 900 7200 50  0001 C CNN
	1    900  7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	900  7000 900  7200
Text GLabel 900  4900 0    50   Output ~ 0
V5
$Comp
L Pin_Headers_Special:ESP32 J7
U 1 1 66B5C4BB
P 1700 4700
F 0 "J7" H 1525 4725 50  0000 C CNN
F 1 "ESP32" H 1525 4634 50  0000 C CNN
F 2 "Pin_Headers_Special:ESP32_PinHeaders" H 1550 4700 50  0001 C CNN
F 3 "" H 1550 4700 50  0001 C CNN
	1    1700 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	600  3700 1300 3700
Wire Wire Line
	1100 4900 900  4900
Wire Wire Line
	900  6900 1100 6900
Wire Wire Line
	900  7000 1100 7000
$Comp
L power:+12V #PWR0114
U 1 1 669EDD0C
P 2500 1900
F 0 "#PWR0114" H 2500 1750 50  0001 C CNN
F 1 "+12V" H 2515 2073 50  0000 C CNN
F 2 "" H 2500 1900 50  0001 C CNN
F 3 "" H 2500 1900 50  0001 C CNN
	1    2500 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2800 2500 2800
Wire Wire Line
	2500 1900 2500 2800
Connection ~ 2500 2800
Wire Wire Line
	2500 2800 2600 2800
$Comp
L Connector_Generic:Conn_02x08_Top_Bottom J2
U 1 1 669AABDF
P 1500 3700
F 0 "J2" H 1550 4217 50  0000 C CNN
F 1 "OBD Female Connector" H 1550 4126 50  0000 C CNN
F 2 "OBD_Connectors:OBD-II Female Connector" H 1500 3700 50  0001 C CNN
F 3 "~" H 1500 3700 50  0001 C CNN
	1    1500 3700
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 66AF1893
P 3700 5000
F 0 "H1" H 3800 5049 50  0000 L CNN
F 1 "MountingHole_Pad" H 3800 4958 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 3700 5000 50  0001 C CNN
F 3 "~" H 3700 5000 50  0001 C CNN
	1    3700 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 66AF1F56
P 3700 5100
F 0 "#PWR01" H 3700 4850 50  0001 C CNN
F 1 "GND" H 3705 4927 50  0000 C CNN
F 2 "" H 3700 5100 50  0001 C CNN
F 3 "" H 3700 5100 50  0001 C CNN
	1    3700 5100
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 66AF5726
P 3700 5500
F 0 "H2" H 3800 5549 50  0000 L CNN
F 1 "MountingHole_Pad" H 3800 5458 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 3700 5500 50  0001 C CNN
F 3 "~" H 3700 5500 50  0001 C CNN
	1    3700 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 66AF572C
P 3700 5600
F 0 "#PWR02" H 3700 5350 50  0001 C CNN
F 1 "GND" H 3705 5427 50  0000 C CNN
F 2 "" H 3700 5600 50  0001 C CNN
F 3 "" H 3700 5600 50  0001 C CNN
	1    3700 5600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 66AF9902
P 4700 5000
F 0 "H3" H 4800 5049 50  0000 L CNN
F 1 "MountingHole_Pad" H 4800 4958 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 4700 5000 50  0001 C CNN
F 3 "~" H 4700 5000 50  0001 C CNN
	1    4700 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 66AF9908
P 4700 5100
F 0 "#PWR03" H 4700 4850 50  0001 C CNN
F 1 "GND" H 4705 4927 50  0000 C CNN
F 2 "" H 4700 5100 50  0001 C CNN
F 3 "" H 4700 5100 50  0001 C CNN
	1    4700 5100
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 66AFCE9C
P 4700 5500
F 0 "H4" H 4800 5549 50  0000 L CNN
F 1 "MountingHole_Pad" H 4800 5458 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 4700 5500 50  0001 C CNN
F 3 "~" H 4700 5500 50  0001 C CNN
	1    4700 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 66AFCEA2
P 4700 5600
F 0 "#PWR04" H 4700 5350 50  0001 C CNN
F 1 "GND" H 4705 5427 50  0000 C CNN
F 2 "" H 4700 5600 50  0001 C CNN
F 3 "" H 4700 5600 50  0001 C CNN
	1    4700 5600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
