#SH_T # please enable to break the code and allow REPL scroll UP for read and copy

# TimerCheck Ain Aout PID WebServer MQTT // CP900a5 back to CP829
import gc
import os
import time  # ___________________________________________ we use time.monotonic aka seconds in float, to control the loop and NO time.sleep() any more..
# ________________________________________________________ expect file tools.py
from tools import DIAG, DIAGM, dp, check_mem
if ( DIAGM ) : print(f"\nFREE MEM report from code.py after imports\n+ import time {gc.mem_free()}")

from pico_w_io import getAins, runPID, DO1ramp  # used from JOB1


dp("\n___ i am a PICO_W")
dp("___ KLL project 1 sec Ain, Aout as DoutPWM, PID")

from web_wifi import setup_webserver, run_webserver
from mqtt import mqtt_send

setup_webserver()  # ______________________________________ from file web_wifi.py
if ( DIAGM ) : print(f"+ setup_webserver {gc.mem_free()}")


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
updateM = 1000000
secdotprint = True


# _________________________________________________________ here can add your ( non blocking ) code to run at timer 1 and timer 2
def JOB1():
    getAins()  # ____________________________________________ measure every 1sec in pico_w_io.py
    runPID()  # ____________________________________________ PID control
    # DO1ramp() # __________________________________________ from pico_w_io JUST TEST
    check_mem(info = "JOB1 prior run_webserver",prints=False,coll=True)
    run_webserver()  # _____________________________________ from web_wifi.py / in main loop it's killing me
    check_mem(info = "JOB1 after run_webserver",prints=False,coll=True)


def JOB2():
    mqtt_send()
    check_mem(info = "JOB2 after mqtt_send",prints=DIAGM,coll=True)

check_mem(info = "startup",prints=DIAGM,coll=True)

infos = "___ Main structure info: endless triple loop: \n___ loopM counts to "
infos += f"{updateM:}"
infos += " and reports time, \n___ loop1 checks for elapse of "
infos += f"{update1:,.2f}"
infos += " sec and prints a '.' & JOB1 ( Ains, Dout, PID ), \n___ loop2 checks for elapse of "
infos += f"{update2:,.2f}"
infos += " sec & JOB2 ( MQTT )"
dp(infos)

while True:  # ___________________________________________ MAIN
    try:
        loopM += 1
        if ( loopM >= updateM ):  # ______________________ 1 million loop timer / reporter
            loopM = 0
            Mloop_s = time.monotonic()
            Mloopts = ""
            if Mlooprep: Mloopts = " {:>5.3f} sec ".format((Mloop_s - last_Mloop_s))
            last_Mloop_s = Mloop_s  # __________________ remember
            check_mem(info = "1Mloop "+Mloopts,prints=DIAGM,coll=True)

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
# 26.7 sec with HTML ( incl dynamic SVG ) and auto refresh 30 sec, RTC from NTP
# 26.4 sec even with MQTT send to RPI4 BUT check mem: 4600
# and CRASH memory allocation failed in web_server poll ADD 6 gc_collect and:
# 26.9 sec loopM and loop1 ( get 4 Ains + set 1 Dout pwm + PID ) and loop2 ( mqtt send )
# 27.3 sec with html page open
# use tools.py and CP900a6 and HTML open:
# 28.1 sec mem 17300 / low mem 15800 after mqtt_send ( 60.0x sec ) / low mem 11500 after start incl mqtt hallo /
# use CP829
# 26.8 sec mem 26000

