// pico_w_io.ino

// get lib https://github.com/khoih-prog/RP2040_PWM and copy the 2 .h files in

#define _PWM_LOGLEVEL_        1
#include "RP2040_PWM.h"

//creates pwm instance
RP2040_PWM* PWM_Instance;

//+++ PICO W : DO1 on GP1 pin 2 use as PWM 'analog' out
#define pinToUse 1 
float DO1val=0.0; // ______________________________________________ PWM output mapped to pid Output

bool enablePID = true; // ___________________________________________ see pid.ino call <PID_v1.h> lib
double PV=0,SP=5,OUT=0,OUT_MAN=0;
int MODE=2; // // my MODEs : 0 LOCK, 1 MAN, 2 AUTO, 3 CASC

// __________________________________________________________________ later will need that info like in web page or MQTT
float get_PV() {
  return (float)PV;
}

float get_SP() {
  return (float)SP;
}

void set_SP(float sp=0.0) {
  SP = (double)sp;
}

float get_OUT() {
  return (float)OUT;
}

void set_OUT(float out=0.0) {
  OUT = out;
  Serial.print("___ pico_w_io set_OUT set : ");
  Serial.println(OUT);
}

int get_MODE() {
  return MODE;
}


// _______________________________________________ get analog in data
int sensorA0 = A0;   // select the input pin for the potentiometer // mapped to PID
int sensorA0val = 0;  // variable to store the value coming from the sensor
int sensorA1 = A1;   // select the input pin for the potentiometer
int sensorA1val = 0;  // variable to store the value coming from the sensor
int sensorA2 = A2;   // select the input pin for the potentiometer
int sensorA2val = 0;  // variable to store the value coming from the sensor
int sensorT0 = A3;
int sensorT0val = 0;

//char thisA_Reads[70]; // ________________________________________________ filled by JobNsec used in HTML

float in_pct(int ain) {
  float ainr = 100.0 * ain / 1023.0;
  return ainr;
}

float in_volt(int ain) {
  float ainr = 3.3 * ain / 1023.0;
  return ainr;
}

float get_cpu_temp() {
  sensorT0val = analogRead(sensorT0); // temperature
  float ADC_Voltage = in_volt(sensorT0val);
  return (27 - (ADC_Voltage - 0.706) / 0.001721);
}

void Ains() {
  sensorA0val = analogRead(sensorA0);
  sensorA1val = analogRead(sensorA1);
  sensorA2val = analogRead(sensorA2);

  PV = in_pct(sensorA0val);
  //SP = 0; // get later from operation
  // MODE can be overwritten by operation
  // OUT_MAN can be overwritten by operation
  if ( enablePID ) {
    OUT_MAN = OUT;
    if ( MODE > 1 ) {
      OUT = (float)mypid_loop(PV,SP,OUT_MAN,MODE); // ________________________ run PID only in AUTO and CASC?
    }
    Serial.print("___ esp32_io Ains OUT after call mypid_loop : ");
    Serial.println(OUT);
    DO1val = OUT; // _________________________________________ PWM setpoint
    DO1set(DO1val); // ______________________________________________ drive PWM
  }
  
  //snprintf(thisA_Reads,100, " ___ Job15sec: Ain: A0: %.2f A1: %.2f A2: %.2f [pct] T0: %.2f [degC] DO5: %.2f [pct] ", in_pct(sensorA0val), in_pct(sensorA1val), in_pct(sensorA2val), sensorT0val, DO5val );
  // global A_Reads feed here ( with the 15 sec job timing ) and used by web_wifi
  A_Reads  = " ___ Job15sec: Ain: A0: ";
  A_Reads += String(in_pct(sensorA0val),1);
  A_Reads += " A1: ";
  A_Reads += String(in_pct(sensorA1val),1);
  A_Reads += " A2: ";
  A_Reads += String(in_pct(sensorA2val),1);
  A_Reads += " [pct] T0: ";
  A_Reads += String(get_cpu_temp(),1);
  A_Reads += " [degC] DO1: ";
  A_Reads += String(DO1val,1);
  A_Reads += " [pct] ";
}


void set_IO() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  //digitalWrite(LED_BUILTIN, HIGH);

  //assigns pin 1 (external LED or servo ), with frequency of 5 KHz and a duty cycle of 0%
  PWM_Instance = new RP2040_PWM(pinToUse, 5000, 0);
  DO1set(DO1val);

  if ( enablePID ) {
    Serial.println("\n___ PID enabled");
    mypid_setup(PV, SP);
  }

}

bool thisLED_status = 0;

void LED_toggle() {
  thisLED_status = !thisLED_status;
  digitalWrite(LED_BUILTIN, thisLED_status);
  }

void DO1set(float dutyCycle) {
  //int thisduty_cycle = 65535 - int(dutyCycle * 65535 / 100.0);
  PWM_Instance->setPWM(pinToUse, 5000, dutyCycle);
}

void set_MODE(int mode=2) {
  MODE = mode; // remember web wifi mode set here
  Serial.print("___ esp32_io set_MODE call dpid mypid_setMode with MODE: ");
  Serial.print(MODE);
  Serial.print(" (double)OUT: ");
  Serial.println(OUT);
  mypid_setMode(mode,(double)OUT); //
}

void DO1ramp() { // __________________________________________ now unused test
    DO1val -= 5.0;
    if ( DO1val < 0.0 ) {
        DO1val = 100.0;
    }
    DO1set(DO1val);
    //Serial.print("___ PWM: ");
    //Serial.println(DO1val);
}


