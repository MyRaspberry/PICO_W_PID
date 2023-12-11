
## PICO_W PID

webserver / analog in / digital out pwm / PID /

v1.0.1
_______________________________

board: PICO W
```OS: Circuit Python
adafruit-circuitpython-raspberry_pi_pico_w-en_US-20231205-no-branch-156f417.uf2
adafruit-circuitpython-bundle-9.x-mpy-20231205.zip
flash_nuke.uf2
```

### read back after nuke & flash:

####boot_out.txt

Adafruit CircuitPython 9.0.0-alpha.5-34-g156f417f70 on 2023-12-05;
 
Raspberry Pi Pico W with rp2040

Board ID:raspberry_pi_pico_w

______________________________

the in this project used Python PID code is from:
https://github.com/m-lundberg/simple-pid
MIT lic.

_______________________________

## maker

by: kll engineering

lic: CC-BY-SA 4.0

date: 11.12.23

place: CNX

blog: http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=227

_______________________________

## HOW TO

-1- get a PICO W board with WIFI 6$ and HEADERS ( if you not want to solder 7$ )

https://www.raspberrypi.com/products/raspberry-pi-pico/?variant=raspberry-pi-pico-wh

-2- connect it with a USB cable ( USB A to Micro-USB-B cable ) to a PC USB port

and see drive content INDEX.HTM, INFO_UF2.TXT ( with bootloader info )

later you will need to press the [BOOT-SEL] button while connecting to see that drive again

-3- open 2 file explorer windows and copy like drag / drop

adafruit-circuitpython-raspberry_pi_pico_w-< LANGUAGE >< DATE >< VERSION >.uf2 to it

the drive will close and the 'CIRCUITPY' drive will open ( 5sec )

pls find boot_out.txt and check

-4- delete empty code.py and settings.toml

and copy drag new files and /lib/ 

-5- open MU-Editor ( mode circuitpython )( should auto find that drive and open code.py )

activate the serial TAB aka REPL console ( use [CTRL][c] for stop / [CTRL][d] for reboot )

now develop code directly on the board

BUT: can not edit settings.toml on the board! ( reboot failure might happen )

that file only edit on the PC and copy it to the board

-6- good project code backup to your PC  

_______________________________
