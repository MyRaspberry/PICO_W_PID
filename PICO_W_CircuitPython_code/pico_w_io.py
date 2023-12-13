# PICO_W_IO
import os
import board
import digitalio
import pwmio
from analogio import AnalogIn
import microcontroller # __________________________________ needed for CPU temperature
import time  # ____________________________________________ we use time.monotonic aka seconds in float, to control the loop and NO time.sleep() any more..

from pid import PID # _____________________________________ file pid.py

DIAG = True  # False
DIAG = bool(os.getenv('DIAG')) # __________________________ now get from settings.toml


def dp(line=" ", ende="\n"):
    if DIAG:
        print(line, end=ende)

# _________________________________________________________ PICOW_IO

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT
led.value = True  # _______________________________________ after boot LED ON helps to see its working..
dp("___ +++ board LED ON")

# _________________________________________________________ old config for GP1 ON OFF
#DO1set = False
#DO1 = digitalio.DigitalInOut(board.GP1)
#DO1.switch_to_output(value=True) # _______________________ no boot blink! thanks @deʃhipu
#DO1.direction = digitalio.Direction.OUTPUT
#DO1.value = not DO1set  # ________________________________ after boot DO1 ( ext LED output high ) aka LED OFF BUT IT BLINKS
#dp("___ +++ DO1 on GP1 HIGH >> LED OFF")
# _________________________________________________________ new config for GP1 PWM to ext LED ( or a servo for real process control )
DO1val = float(100)
DO1 = pwmio.PWMOut(board.GP1, frequency=5000, duty_cycle=0)

def DO1set(pct=100.0):
    DO1.duty_cycle = 65535 - int(pct * 65535 / 100.0)

#DO1.duty_cycle = 0 #LED FULL ON # 65535
DO1set(DO1val)
dp("___ +++ DO1 on GP1 pin 2 use as PWM 'analog' out, now 100.0 pct full on")

def DO1ramp(): # __________________________________________ now unused test
    global DO1val
    DO1val -= 5.0
    if DO1val < 0.0 :
        DO1val = 100.0
    DO1set(DO1val)

AI0val = 0.0
AI1val = 0.0
AI2val = 0.0
T0val = 0.0
datas = "Ain data in one line "

# control PID, local parameter, tuning, start, limit outputs
PV=0.0
SP=5.0
OUT=0.0
GAIN=5.0
RESET=0.01
RATE=0.1
pid=PID(GAIN,RESET,RATE,setpoint=SP) # ____________________ init control
pid.auto_mode
pid.output_limits = (0,100) # _____________________________ incl internal "anti-reset-wind-up" integral limit
pids = "pid data in one line"

def get_volt(pin):
    return (pin.value * 3.3) / 65536


def get_pct(pin):
    return (pin.value * 100.0) / 65536


def getAins(): # __________________________________________ changed to use PCT
    global AI0val, AI1val, AI2val, T0val, datas
    analog_in0 = AnalogIn(board.A0)  # GP26 pin 31
    analog_in1 = AnalogIn(board.A1)  # GP27 pin 32
    analog_in2 = AnalogIn(board.A2)  # GP28 pin 34
    # use get_volt or get_pct
    AI0val = get_pct(analog_in0)
    AI1val = get_pct(analog_in1)
    AI2val = get_pct(analog_in2)
    analog_in0.deinit()
    analog_in1.deinit()
    analog_in2.deinit()
    T0val = microcontroller.cpu.temperature
    datas = f" AIN A0: {AI0val:,.2f} [pct], A1: {AI1val:,.2f} [pct], A2: {AI2val:,.2f} [pct], T0: {T0val:,.2f} [degC] / DO1: {DO1val:,.2f} [pct]"
    #dp(datas)


def runPID():
    global OUT, PV, pids, DO1val
    PV=AI0val # ___________________________________________ link hardware Ain 0 to PV of control
    if pid.auto_mode :
        OUT = pid(PV) # ___________________________________ call pid and get its reaction
    DO1val = OUT
    DO1set(DO1val) # ______________________________________ drive the hardware PWM output
    pids= f" PV: {PV:,.2f} [pct], SP: {SP:,.2f} [pct], OUT: {OUT:,.2f} [pct]"
    #dp(pids)


def get_datas(): # ________________________________________ to web_wifi.py to get the measurements
    return datas

def get_pids(): # _________________________________________ to web_wifi.py, just send the last readings / PID as string tupple for index.html
    return pids

def get_pid_details():
    return f"limits: {pid.output_limits}, auto mode: {pid.auto_mode}, tuning Kp,Ki,Kd: {pid.tunings}"

def get_pid_mode():
    return pid.auto_mode

def get_PV(): # ___________________________________________ to web_wifi.py
    return PV

def set_SP(newSP=50.0): # _________________________________ from web_wifi.py
    global SP, pid
    SP = newSP
    pid.setpoint = SP # ___________________________________ to controller
    pid.auto_mode = True

def get_SP(): # ___________________________________________ to web_wifi.py
    return SP

# MANUAL MODE ONLY
def set_OUT(newOUT=0.0): # ________________________________ from web_wifi.py
    global OUT, pid
    OUT = newOUT
    pid.set_auto_mode(False, last_output=newOUT)
    pid.output = OUT # ____________________________________ to controller
    DO1val = OUT
    DO1set(DO1val) # ______________________________________ drive the hardware PWM output

def get_OUT(): # __________________________________________ to web_wifi.py
    return OUT

def get_T0val(): # ________________________________________ to mqtt.py
    return T0val
