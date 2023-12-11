
## PICO_W PID

webserver / analog in / digital out pwm / PID /

![from phone](http://kll.byethost7.com/kllfusion01/downloads/PICOW_android_beowser_page.jpg "looks nice on mobile too" )

v1.0.1
_______________________________

board: PICO W
```OS: Circuit Python
adafruit-circuitpython-raspberry_pi_pico_w-en_US-20231205-no-branch-156f417.uf2
adafruit-circuitpython-bundle-9.x-mpy-20231205.zip
flash_nuke.uf2
```

### read back after nuke & flash:

see: boot_out.txt

Adafruit CircuitPython 9.0.0-alpha.5-34-g156f417f70 on 2023-12-05;</br>
Raspberry Pi Pico W with rp2040</br>
Board ID:raspberry_pi_pico_w</br>

______________________________

the in this project used Python PID code is from:</br>
https://github.com/m-lundberg/simple-pid</br>
MIT lic.

_______________________________

## maker

by: kll engineering</br>
lic: CC-BY-SA 4.0</br>
date: 11.12.23</br>
place: CNX</br>
blog: http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=227

_______________________________

## HOW TO

-1-</br>
get a PICO W board with WIFI 6$ and HEADERS ( if you not want to solder 7$ )</br>
https://www.raspberrypi.com/products/raspberry-pi-pico/?variant=raspberry-pi-pico-wh

-2-</br>
connect it with a USB cable ( USB A to Micro-USB-B cable ) to a PC USB port</br>
and see drive content INDEX.HTM, INFO_UF2.TXT ( with bootloader info )</br>
later you will need to press the [BOOT-SEL] button while connecting to see that drive again

-3-</br>
open 2 file explorer windows and copy like drag / drop</br>
adafruit-circuitpython-raspberry_pi_pico_w-< LANGUAGE >< DATE >< VERSION >.uf2 to it</br>
the drive will close and the 'CIRCUITPY' drive will open ( 5sec )</br>
pls find boot_out.txt and check

-4-</br>
delete empty code.py and settings.toml</br>
and copy drag new files and /lib/ 

-5-</br>
open MU-Editor ( mode circuitpython )( should auto find that drive and open code.py )</br>
activate the serial TAB aka REPL console ( use [CTRL][c] for stop / [CTRL][d] for reboot )</br>
now develop code directly on the board</br>
BUT: can not edit settings.toml on the board! ( reboot failure might happen )</br>
that file only edit on the PC and copy it to the board

-6-</br>
good project code backup to your PC  

_______________________________
