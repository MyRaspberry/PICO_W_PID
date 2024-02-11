## Board: ESP32-S3 N16R8 ( i have YD-ESP32-23 2022-V1.3 not original ESP32 S3 Devkitc-1 Wroom-1 board)

## SYS: Arduino Ide 2.2.1

### in this version try to use MQTT wit TLS:

  ESP32_Ain_PID_NTP_WEBserver_MQTT-TLS_TimerLoop_settings_HTML-String</br>
  !now with MQTT TLS by MQTT.h & WiFiClientSecure & No CERT</br>

## code.ino

- loop to make 1sec and 1min ticks ( every 1000 loops )</br>
- loop 1sec to count ( also run a downramp on the ext PWM LED )</br>
- loop Nsec ( here 15 sec ) </br>
- loop 1min </br>
- loop 1M count to 1 000 000 and print time ( 6 sec now )</br>

## common.ino

- get time string yyyy-mm-dd hh:mm:ss

## web_wifi.ino

- router login</br>
- NTP to RTC</br>
- login local MQTT broker ( NO TLS )</br>
- - send counter, timestring, PID data</br>
- subscribe to remote set RGB LED ( ++ RPI /python_tools/ ramp function )</br>
- web server fix IP and port http://192.168.1.215:1234</br>

- dynamic index.html</br>
- - auto-refresh 15 sec ( because show time )</br>
- - style from python project</br>
- - favicon ? here have no file structure to serve files ?</br>

- dynamic data.html</br>
- - Ains and Dout</br>
- - PID data</br>
- - PID operation</br>
- - PID Faceplate</br>
- - MQTT string</br>
- - date time</br>
- - UpTime</br>

## esp32_io.ino

- read analog in A0 A1 A2 by 15 sec job and make string for HTML show</br>
- DO5 pin5 pwm output</br>
- drive board RGB led</br>

## settings.h

- wifi declares like SSID PASS PORT FIXIP</br>
- mqtt broker, port, user, password, topic ( main / device / set )</br>
- selector for 3 Broker

## pid.ino

- run the arduino lib pid</br>


</br>
