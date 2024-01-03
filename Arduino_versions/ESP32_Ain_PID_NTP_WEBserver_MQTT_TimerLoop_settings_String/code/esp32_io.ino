// esp32_io.ino

#include "driver/temp_sensor.h"

void initTempSensor(){
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

// YD-ESP32-S3 on board RGB LED NeoPixel W2812 solder RGB jumper, talk to GPIO48
#define RGBLED_PIN 48
#define MAX_BRIGHT 127  // 0 .. 255

byte esp_cR=0; //MAX_BRIGHT;
byte esp_cG=0;
byte esp_cB=MAX_BRIGHT;

void get_RGB_set() {
  if ( mqtt_get_cFlag() ) {
    esp_cR = mqtt_get_cR();
    esp_cG = mqtt_get_cG();
    esp_cB = mqtt_get_cB();
    neopixelWrite(RGBLED_PIN,esp_cR,esp_cG,esp_cB);
    mqtt_reset_cFlag();
  }
  // other program might want play with RGB too
}

// use BUILD IN 'ledc lib'
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html

//+++ ESP32 : DO1 on GP05 pin 5 use as PWM 'analog' out
#define LEDC_CHANNEL_0 0 // _________________________________________ use first channel of 16 channels (started from zero)
#define LEDC_TIMER_12_BIT 12 // _____________________________________ use 12 bit precission for LEDC timer
#define LEDC_BASE_FREQ  5000 // _____________________________________ use 5000 Hz as a LEDC base frequency
#define LED_PIN 5 // ________________________________________________ fade LED PIN (replace with LED_BUILTIN constant for built-in LED) my board left pin 5 is GPIO05

#define A0pin 1 // __________ GPIO1 ADC1_0 pin 4 right
#define A1pin 2 // __________ GPIO2 ADC1_1 pin 5 right
#define A2pin 3 // __________ GPIO3 ADC1_2 pin 4 left

// __________________________________________________________________ Arduino like analogWrite value has to be between 0 and valueMax
//void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
//  uint32_t duty = (4095 / valueMax) * min(value, valueMax); // ____ calculate duty, 4095 from 2 ^ 12 - 1
//  ledcWrite(channel, duty); // ____________________________________ write duty to LEDC
//}

void DO5set(float dutyCycle) { // ___________________________________ set PCT to int 4095
  int thisduty_cycle = 4095 - int(dutyCycle * 4095 / 100.0);
  ledcWrite(LEDC_CHANNEL_0, thisduty_cycle); // _____________________ write duty to LEDC
}

void setup_DO5() {
  // ________________________________________________________________ Setup timer and attach timer to a led pin GPIO5 pin 5
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

  neopixelWrite(RGBLED_PIN,esp_cR,esp_cG,esp_cB);
}

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
  Serial.print("___ esp32_io set_OUT set : ");
  Serial.println(OUT);
}

float DO5val=0.0; // ______________________________________________ PWM output mapped to pid Output SEE Ains()

int get_MODE() {
  return MODE;
}

// _______________________________________________ get analog in data
int sensorA0 = A0pin;   // select the input pin for the potentiometer // mapped to PID
int sensorA0val = 0;  // variable to store the value coming from the sensor
int sensorA1 = A1pin;   // select the input pin for the potentiometer
int sensorA1val = 0;  // variable to store the value coming from the sensor
int sensorA2 = A2pin;   // select the input pin for the potentiometer
int sensorA2val = 0;  // variable to store the value coming from the sensor
float sensorT0val=0.0; // for cpu temperature

//char thisA_Reads[100]; // ________________________________________________ filled by JobNsec used in HTML

float in_pct(int ain) {
  float ainr = 100.0 * ain / 4095.0;
  return ainr;
}

float in_volt(int ain) {
  float ainr = 3.3 * ain / 4095.0;
  return ainr;
}

float get_internalT() {
  float result = 0;
  temp_sensor_read_celsius(&result);
  //Serial.print(result);
  //Serial.println(" °C");
  return result;
}

void Ains() {
  sensorA0val = analogRead(sensorA0);
  sensorA1val = analogRead(sensorA1);
  sensorA2val = analogRead(sensorA2);
  sensorT0val = get_internalT();

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
    DO5val = OUT; // _________________________________________ PWM setpoint
    DO5set(DO5val); // ______________________________________________ drive PWM
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
  A_Reads += String(sensorT0val,1);
  A_Reads += " [degC] DO5: ";
  A_Reads += String(DO5val,1);
  A_Reads += " [pct] ";
}

// use locally with "strcpy(A_Reads,get_Ains() );" // get string of readings from esp32_io.ino
//char * get_Ains() {
//  return thisA_Reads; // __________________________________ from last sampling by 15 sec job
//}

void set_IO() {
  // no builtin led, RGB serial driver to GPIO48 ?
  initTempSensor(); // ______________________________________________ init esp32-s3 temperature sensor report by analog in and MQTT as PS_temp
  //assigns pin 5 GPIO5 (external LED or servo ), with frequency of 5 KHz and a duty cycle of 0%
  setup_DO5();
  DO5set(DO5val);

  if ( enablePID ) {
    Serial.println("\n___ PID enabled");
    mypid_setup(PV, SP);
  }


}

void set_MODE(int mode=2) {
  MODE = mode; // remember web wifi mode set here
  Serial.print("___ esp32_io set_MODE call dpid mypid_setMode with MODE: ");
  Serial.print(MODE);
  Serial.print(" (double)OUT: ");
  Serial.println(OUT);
  mypid_setMode(mode,(double)OUT); //
}

void DO5ramp() { // __________________________________________ now unused test
    DO5val -= 5.0;
    if ( DO5val < 0.0 ) {
        DO5val = 100.0;
    }
    DO5set(DO5val);
    //Serial.print("___ PWM: ");
    //Serial.println(DO5val);
}


