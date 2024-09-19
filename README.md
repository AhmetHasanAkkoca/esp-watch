# ESP-12 SmartWatch Project
A smartwatch project using ESP-12 module.

## Aim of this project:
The aim of the project is to create a smartwatch that;
- is daily-usable,
- is cheap,
- is WI-FI capable,
- uses OLED screen,
- is modular,
- is open-source.

## BOM (Bill Of the Materials):
| Name | Purpose |Mandatory|
|------|---------|-------------|
|ESP-12|WI-FI and MC|✔️|
|128*64 OLED|Screen|✔️|
|DS1302|RTC IC|🟡|
|MH-CD42|Battery Controller|✔️|
|AMS1117 3.3v|5v-3.3v Converter|✔️|
|6x Buttons|Controlling the watch|🟡|
|400mA Battery|Storing Power|✔️|
|Pogo Connectors|Charging&Flashing|❌|
### Notes:
- You can choose to not use a RTC but deep sleep function cannot be used.
- You can use fewer/more buttons if you want.
