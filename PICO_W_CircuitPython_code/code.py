# TimerCheck Ain Aout CP900a5
import os
import time  # ___________________________________________ we use time.monotonic aka seconds in float, to control the loop and NO time.sleep() any more..

from pico_w_io import getAins, runPID, DO1ramp # used from JOB1

DIAG = True  # False
DIAG = bool(os.getenv('DIAG')) # ______________________________ now get from settings.toml


def dp(line=" ", ende="\n"):
    if DIAG:
        print(line, end=ende)


dp("\n___ i am a PICO_W and run CP900-a5-34")
dp("___ KLL project 1 sec Ain, Aout as DoutPWM, PID")

import gc  # micropython garbage collection # use gc.mem_free() # use gc.collect()

def check_mem():
    dp("___ check mem   : {:}".format(gc.mem_free()))
    gc.collect()
    dp("___ after clear : {:}".format(gc.mem_free()))

from web_wifi import setup_webserver, run_webserver

setup_webserver() # ______________________________________ from file web_wifi.py


# ________________________________________________________ multi job timer

start_s1 = time.monotonic()
loop1 = 0
loopt1 = 1000  # _________________________________________ we can read time every loop OR every loopt loop only, makes the 1M faster/ but timer more inaccurate
update1 = 1.0  # _________________________________________ every 1 sec do

start_s2 = time.monotonic()
loop2 = 0
loopt2 = 1000  # _________________________________________ we can read time every loop OR every loopt loop only, makes the 1M faster/ but timer more inaccurate
update2 = 60.0  # ________________________________________ every 60 sec do

Mloop_s = time.monotonic()
last_Mloop_s = time.monotonic()
Mlooprep = True
loopM = 0
updateM=1000000
secdotprint = True


# _________________________________________________________ here can add your ( non blocking ) code to run at timer 1 and timer 2
def JOB1():
    getAins() #____________________________________________ measure every 1sec in pico_w_io.py
    runPID() # ____________________________________________ PID control
    #DO1ramp() # __________________________________________ from pico_w_io JUST TEST
    run_webserver() # _____________________________________ from web_wifi.py / in main loop it's killing me
    pass

def JOB2():
    pass


check_mem()

infos  ="___ Main structure info: endless triple loop: \n___ loopM counts to "
infos +=f"{updateM:}"
infos +=" and reports time, \n___ loop1 checks for elapse of "
infos +=f"{update1:,.2f}"
infos +="sec and prints a '.' & JOB1 ( Ains, Dout, PID ), \n___ loop2 checks for elapse of "
infos +=f"{update2:,.2f}"
infos +="sec & JOB2 ( future )"
dp(infos)

while True:  # ___________________________________________ MAIN
    try:
        loopM += 1
        if (
            loopM >= updateM
        ):  # _________________________ 1 million loop timer / reporter
            loopM = 0
            Mloop_s = time.monotonic()
            if Mlooprep:
                dp("\n___ 1Mloop: {:>5.3f} sec ".format((Mloop_s - last_Mloop_s)))
            last_Mloop_s = Mloop_s  # __________________ remember
            check_mem()

        loop1 += 1
        if loop1 > loopt1:
            loop1 = 0
            now_s1 = (
                time.monotonic()
            )  # _______________ JOB1 is a timed job, but use a counter to NOT read millis every loop as that's slow
            next_s1 = start_s1 + update1
            if now_s1 >= next_s1:  # _______ 1 sec
                if secdotprint:
                    dp(
                        ".", ""
                    )  # _______________________ means print(".",end="") aka NO LINEFEED, but makes it difficult to scroll the REPL UP
                start_s1 = next_s1
                JOB1()

        loop2 += 1
        if loop2 > loopt2:
            loop2 = 0
            now_s2 = (
                time.monotonic()
            )  # _______________ JOB2 is a timed job, but use a counter to NOT read millis every loop as that's slow
            next_s2 = start_s2 + update2
            if now_s2 >= next_s2:  # _______ 1 min
                dp(
                    ("\n___ loop2: {:>5.3f} sec ".format((now_s2 - start_s2)))
                )  # _______________________ check 1 min
                start_s2 = next_s2
                JOB2()

# if run_webserver() here timing is completely OFF so put into 1 sec loop??

    except OSError:
        continue


# on RPI PICO W: loopM need:
# 8.9  sec loopM only
# 17.1 sec loopM and empty loop1
# 25.3 sec loopM and empty loop1 and empty loop2
# 25.5 sec loopM and loop1 ( get 4 Ains set 1 Dout pwm) and empty loop2
# 26.4 sec loopM and loop1 ( get 4 Ains set 1 Dout pwm + PID ) and empty loop2
# 26.7 sec with HTML ( incl dynamic SVG ) and auto refresh 30 sec

