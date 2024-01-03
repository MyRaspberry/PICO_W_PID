/********************************************************
/ install PID by Brett Beeauregard v1.2.0
/ this is from its examples
/ see also https://github.com/br3ttb/Arduino-PID-Library
/ ********************************************************
/ PID Basic Example
/ ********************************************************/
#include <PID_v1.h>

double Setpoint, Input, Output; // __________________________________ Define Variables we'll be connecting to
// __________________________________________________________________ Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

float get_Kp(){
  return (float)Kp;
}
float get_Ki(){
  return (float)Ki;
}
float get_Kd(){
  return (float)Kd;
}



void mypid_setup(double PV, double SP) { // _________________________ initialize the variables we're linked to
  Input = PV; // bumpless start? analogRead(PIN_INPUT);
  Setpoint = SP;
  myPID.SetMode(1); // AUTOMATIC ____________________________________ turn the PID on
}


void mypid_setMode(int Mode = 2, double OUT_MAN = 0.0) { // my MODEs : 0 LOCK, 1 MAN, 2 AUTO, 3 CASC
  if ( Mode == 3 ) { // CASC
    myPID.SetMode(1); // AUTOMATIC __________________________________ turn the PID on
    myPID.SetOutputLimits(0, 100.0);
    Serial.printf("___ dpid mypid_setMode Mode %d set 1\n",Mode);
  }
  if ( Mode == 2 ) { // AUTO
    myPID.SetMode(1); // AUTOMATIC __________________________________ turn the PID on
    myPID.SetOutputLimits(0, 100.0);
    Serial.printf("___ dpid mypid_setMode Mode %d set 1\n",Mode);
  }
  if ( Mode == 1 ) { // MAN
    myPID.SetMode(0); // MANUAL _____________________________________ turn the PID off
    myPID.SetOutputLimits(OUT_MAN, OUT_MAN);
    Serial.printf("___ dpid mypid_setMode Mode %d set 0\n",Mode);
  }
  if ( Mode == 0 ) { // LOCK
    myPID.SetMode(0); // MANUAL _____________________________________ turn the PID off
    myPID.SetOutputLimits(0, 0);
    Serial.printf("___ dpid mypid_setMode Mode %d set 0\n",Mode);
  }
}

double mypid_loop(double PV, double SP, double OUT_MAN, int Mode) {
  Input = PV;
  mypid_setMode(Mode, OUT_MAN); // __________________________________ this overwrite my remote setting
  myPID.Compute();
  return Output; // _________________________________________________ back to I/O operation
}

