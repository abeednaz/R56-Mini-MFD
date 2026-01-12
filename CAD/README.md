<h1>Mechanical Design</h1>

---

This project required two mechanical assemblies: one to house the OBD-II distribution PCB, and one to attach the gauge screen to the tachometer.

<table>
  <tr>
    <th scope="row" colspan="3"><h2>OBD-II Distribution Board Assembly</h2></th>
  </tr>
  <tr>
    <td scope="row" colspan="3">
      <video width="320" height="240" controls><source src="https://github.com/user-attachments/assets/be2643b0-f73f-4a08-8e7a-71b6ab15d59e" type="video/mp4"> </video>
    </td>
  </tr>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/8ea832bd-4bc3-448f-a731-a03b64ca9212" alt="J1962 cable assembly"></td>
    <td><img src="https://github.com/user-attachments/assets/70b172ce-2240-4d2a-8871-659cdfbe946b" alt="Board assembly"></td>
    <td><img src="https://github.com/user-attachments/assets/5b2d1481-1aba-4ee8-9484-a1b1d18c96c8" alt="Completed board enclosure"></td>
  </tr>
  <tr>
    <td scope="row" colspan="3">This assembly packages the OBD-II distribution PCB along with a J1962 connector to connect the ELM327 wireless code scanner. The J1962 connector is linked to the PCB with a cable assembly with the opposite end being a Molex iGrid 16-pin connector.</td>
  </tr>
</table>

<table>
  <tr>
    <th scope="row" colspan="3"><h2>Screen Assembly</h2></th>
  </tr>
  <tr>
    <td scope="row" colspan="3">
      <video width="320" height="240" controls><source src="https://github.com/user-attachments/assets/be2643b0-f73f-4a08-8e7a-71b6ab15d59e" type="video/mp4"> </video>
    </td>
  </tr>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/bd514a23-9b2d-4e9c-8cab-855995ea507f" alt="Gauge pod and arm assembly"></td>
    <td><img src="https://github.com/user-attachments/assets/3f5aa137-d91d-41ed-b9ed-4473ae4afbd9" alt="Gauge assembled on vehicle"></td>
  </tr>
  <tr>
    <td scope="row" colspan="3">This assembly packages the ESP32 AMOLED module with its accessory PCB in a gauge pod which is mounted to the tachometer with an arm. The gauge pod conceals the button underneath the bezel for the user to scroll between different sensors. The screen bezel facing the driver is hinged from the top on two dowel pins, allowing it to flex and press the button similarly to some laptop trackpad buttons. The back bezel contains cutouts for the power switch, ambient light sensor, ESP32 module SD card, and ESP32 module USB-C connector. |
| The arm is adapted from a free phone holder from [Printables](https://www.printables.com/model/1270372-phone-holder-for-mini-r56r57). The original tachometer screws are too short when the mount is in place, so two 4mm-wide thread-forming screws were used (I cut mine to ~32mm long).</td>
  </tr>
</table>

