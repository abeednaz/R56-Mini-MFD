

<h1>R56 MINI Cooper Multi-Function Display</h1>

---

<h2>Project Background</h2>
The second generation of BMW MINI Cooper S is a great car, but it is missing important metrology from the factory. It has a coolant temperature sensor, but there is no gauge to view the temperature. Technically a driver can view the temperature by entering a <a href="https://youtu.be/KhFTyjIRaBU?si=EUw38T1VuMLcuM9A">secret menu</a>, but this is not practical except for debugging purposes as it requires a long series of inputs to display. The car otherwise uses the coolant temperature sensor only to flash a warning light when the engine has already overheated. 

Knowledge of engine temperature is important to me for two reasons:

- Situational awareness to know when engine is up to temperature in order to use full throttle and rev range
- Situational awareness in case of cooling system failures, such as leaking plastic thermostat housing

Aftermarket solutions such as the ScanGauge II and CravenSpeed Gauge Kit exist; however I felt that the ScanGauge did not suit the interior design style of the MINI, and the CravenSpeed kit cost cost more than I was willing to spend on an accessory. This project aimed to add an engine temperature display to the MINI that fits the MINI's interior design language and to take advantage of other sensors that the vehicle has to offer.

<h2>Project Demonstrations</h2>

<h3>Scrolling through the different available sensors</h3>

https://github.com/user-attachments/assets/e3969205-8839-4358-a2c1-934d2600cd6b

<h3>Getting engine up to operating temperature</h3>

https://github.com/user-attachments/assets/219b7ffb-3163-47f8-8801-7a190b6d4f65

<h3>Boost pressure demonstration</h3>

https://github.com/user-attachments/assets/48d6ecf0-7964-4478-b5fc-5cb366bfa986

<h2>Project Block Diagram</h2>

<img width="3655" height="2315" alt="R56_MFD_block_diagram" src="https://github.com/user-attachments/assets/e68424b2-4ba9-4095-88f7-6aba3d5a677b" />

This project consists of the following components:
- COTS ESP32-based microcontroller with AMOLED screen
- COTS ELM327 OBD-II diagnostic code scanner
- Custom PCB to supply ELM327 and ESP32 with 5V and 12V only when vehicle ignition is switched on
- Custom PCB to provide microcontroller with button, ambient light sensor, and 5V from the power supply PCB
