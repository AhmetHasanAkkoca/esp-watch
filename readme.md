# ESP-12 SmartWatch Project
A smartwatch project using ESP-12 module.

### Tasks:
- [x] Create github page
- [ ] Complete the menu UI code
- [ ] Take feature requests
- [ ] Complete the hardware setup
- [ ] Complete BOM with links

~~**Wokwi project link:** https://wokwi.com/projects/409083785783853057~~ **NOT COMPLETED**
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
|DS1302|RTC IC|🟡[^1]|
|MH-CD42|Battery Controller|✔️|
|AMS1117 3.3v|5v-3.3v Converter|✔️|
|6x Buttons|Controlling the watch|🟡[^2]|
|400mA Battery|Storing Power|✔️|
|Pogo Connectors|Charging&Flashing|❌|
[^1]: You can choose to not use a RTC but deep sleep function cannot be used.
[^2]: You can use fewer/more buttons if you want.
