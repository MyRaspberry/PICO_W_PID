
## PID-project
# PICO W Circuit Python

new  ( 3.1.2024 )

# PICO W Arduino IDE 2.2.1
# ESP32-S3 Arduino IDE 2.2.1

( add sideproject: </br>
# ESP-32-S3 PoorManScope1W

)

- webserver ( RTC from NTP )
- analog in
- digital out pwm
- PID
- MQTT to local broker

## RPI4: MQTT broker & Node-Red & SQLite for Current and Hist Trend

- fix IP 192.168.1.4 on ethernet port / cable
- Mosquitto MQTT broker ( no TLS / password protected ) ( v2.0.11 )
- SQLite3 ( v3.40.1 )
- Node ( v20.10.0 )
- Node-Red ( v3.1.0 )

## project info
see [phone browser](http://kll.byethost7.com/kllfusion01/downloads/PICOW_android_beowser_page.jpg)</br>
see [V102 from PC](http://kll.byethost7.com/kllfusion01/downloads/PICOW_PID_datapage.png)</br>
see [V110 NR&wwww](http://kll.byethost7.com/kllfusion01/downloads/PICOW_PID_WWW_NR_DASHpng.png)</br>
see [ESP32-S3 Arduino IDE](http://kll.byethost7.com/kllfusion01/downloads/ESP32_Arduino_PID_faceplate.png)</br>

- v1.0.1b PreRelease
- v1.0.2b Release
- v1.1.0 MQTT ( no TLS ) Release
- v1.1.1 update to CP900a6 work on PICO_W code
- v1.1.2 back to CP829 and use mem_free report for each import "DIAGM" in settings.toml
- v1.1.3 delete datetime lib use time lib only ( +26kB mem )
_______________________________

board: PICO W</br>
used system files:
```OS: Circuit Python
adafruit-circuitpython-raspberry_pi_pico_w-en_US-20231205-no-branch-156f417.uf2
adafruit-circuitpython-bundle-9.x-mpy-20231205.zip
flash_nuke.uf2
```

### read back after nuke & flash:

see: boot_out.txt

```
Adafruit CircuitPython 9.0.0-alpha.5-34-g156f417f70 on 2023-12-05;
Raspberry Pi Pico W with rp2040
Board ID:raspberry_pi_pico_w
```
______________________________

the in this project used Python PID code is from:</br>
[m-lundberg/simple-pid](https://github.com/m-lundberg/simple-pid) </br>
MIT lic.

_______________________________

## maker

by: kll engineering</br>
lic: CC-BY-SA 4.0</br>
date: 11.12.23</br>
place: CNX</br>
blog: [myBlog PICO_W PID artcle](http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=227)

_______________________________

## HOW TO

- 1</br>
get a PICO W board with WIFI 6$ and HEADERS ( if you not want to solder 7$ )</br>
[PICO W board](https://www.raspberrypi.com/products/raspberry-pi-pico/?variant=raspberry-pi-pico-wh)

- 2</br>
connect it with a USB cable ( USB A to Micro-USB-B cable ) to a PC USB port</br>
and see drive content INDEX.HTM, INFO_UF2.TXT ( with bootloader info )</br>
later you will need to press the [BOOT-SEL] button while connecting to see that drive again

- 3</br>
open 2 file explorer windows and copy like drag / drop</br>
-a- flash_nuke.uf2 ( required only if you change CPxxx version )</br>
-b- adafruit-circuitpython-raspberry_pi_pico_w-< LANGUAGE >< DATE >< VERSION >.uf2 to it</br>
after copy the drive will close and after 5sec drive show up again: 'CIRCUITPY'</br>
pls find there boot_out.txt and check

- 4</br>
you could start now edit 'code.py' with MU-Editor, but here we</br>
delete the empty code.py and settings.toml</br>
and copy / drag our new files and /lib/

- 5</br>
open [MU-Editor](https://codewith.mu/) ( mode circuitpython )( should auto find that drive and open code.py )</br>
activate the serial TAB aka REPL console ( use [CTRL][c] for stop / [CTRL][d] for reboot )</br>
now develop code directly on the board</br>
BUT: can not edit settings.toml on the board! ( reboot failure might happen )</br>
that file only edit on the PC ( like with notepad )( like add your wifi login SSID PASSWORD )</br>
and copy it to the board

 - 6</br>
good project code backup / upload to your PC</br>
keep zips for each version

- 7</br>
[System files download](https://circuitpython.org/board/raspberry_pi_pico_w/)</br>
might want to check if there is a update on the core files,</br>
download CPxxx.uf2 and adafruit-circuitpython-bundle_lib.zip
- 7a- backup all your code</br>
- 7b- nuke the board</br>
- 7c- flash the board with the new CPxxx.uf2</br>
- 7d- copy back your code ( not the /lib/ )</br>
- 7e- with the new lib.zip</br>
copy / unzip from it new versions of the same libs used in that project</br>
( here /adafruit_httpserver/ )</br>
- 7f- expect to now find code errors old code .vs. new libs </br>
and find new lib doc online to fix

- 8</br>
after first edit settings.toml for SSID & SSID password and wanted FIX IP</br>
open any browser ( inside your LAN ) http://192.168.1.215 and test operation</br>
( OUT & SP number or mouse input and [ENTER] as submit )</br> 
( see also MU-REPL for diagnostic prints "www ...." )

- 9</br>
after website operation checked and coding work finished ( incl backup )</br> 
you can disconnect the USB cable from PC and use a USB charger to power the board</br>
( place anywhere inside your LANs WIFI area)

- 10</br>
hardware: PID needs a</br>
*input for PV: connect a SENSOR on ADC0 </br>
but you could also use 'linked' sensor boards..</br>
*output from OUT: here use a LED to see the 'dimmed' control output</br>
later can connect like a SERVO ... for real process control</br>


_______________________________
