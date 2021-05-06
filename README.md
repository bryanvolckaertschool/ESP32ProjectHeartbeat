# ESP32ProjectHeartbeat
The goal of this firmware is to calculate heartrate from an spo2 sensor and send this to an on-site server. 

# Hardware
## [Microcontroller](https://www.espressif.com/en/products/socs/esp32)
The microcontroller we use here is an ESP32.

<img src="Images/PrototypeV0.0.2UnderSide.png" height="350" width="705">

## [SPO2 Sensor](https://www.ti.com/product/AFE4404)
The used SPO2 sensor and chip used for testing where the afe4404 (adc/controller), vemd8080 (fotodiode) and 2 green leds.

<img src="Images/PrototypeV0.0.2TopSide.png" height="350" width="697">

# How to get started
## [Toolchain and esp-idf installation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#introduction)

## Connections
Assuming you use the same sensor and esp32 you will have to connect them as following:

| Sensor        | ESP32         |
| ------------- |:-------------:|
| GND           | GND           |
| 3v3           | 3v3           |
| SDA           | G12           |
| SCL           | G14           |

## Changing settings
`idf.py menuconfig` in the repo directory you can open the project specific configuration menu of the espressif library.
The most important settings to change are the following:
 - under `Example Connection Configuration` change the wifi credentials to match your network.
 - under `Component Config` -> `afe4404` -> `Patient data` change the ID to the right patient id.
exit the menu by pressing escape until you get the save settings dialog.

## How to flash
Once you have followed the before mentioned configuration, connected the afe4404 board and your ESP32. Then you can start to flash to the esp32. You can do this by typing `idf.py flash monitor` in the project repo. Once the building and flashing is done you should see the correct printouts. after about 25 seconds your heartbeat should be displayed.
Assuming you havent changed the code you should encounter an error saying http request failed. For now the URL is hardcoded but in future versions it will be added to a seperate menu. And it will also be mentioned in Changing settings.

# Block digram
## Envisioned procedure for the Firmware
![Current envisioned loop](Images/scheme1.png)

# External Links and Repositorys
## Related Repositorys
### [Server repository](https://github.com/bryanvolckaertschool/Projectheartbeat)
### [Old repository](https://github.com/SoenensBram/HorlogeESP8622)
## Links
### [Website](https://vzwheartbeats.be/)
### [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)