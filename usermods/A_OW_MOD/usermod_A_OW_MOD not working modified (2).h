#pragma once

#include "wled.h"


//#define PHOTORESISTOR_PIN A0
//#define LIGHT_BAR_R_PIN A36  //status bar red
//#define LIGHT_BAR_G_PIN 39   //status bar green
//#define LIGHT_BAR_B_PIN 18   //status bar blue
//#define PHOTORESISTOR_PIN A0
//#define PHOTORESISTOR_PIN A0
//#define PHOTORESISTOR_PIN A0
//#define PHOTORESISTOR_PIN A0
//#define PHOTORESISTOR_PIN A0
//#define PHOTORESISTOR_PIN A0

// the frequency to check photoresistor, 5 seconds
#ifndef USERMOD_SN_PHOTORESISTOR_MEASUREMENT_INTERVAL
#define USERMOD_SN_PHOTORESISTOR_MEASUREMENT_INTERVAL 5000
#endif

// how many seconds after boot to take first measurement, 7 seconds
#ifndef USERMOD_SN_PHOTORESISTOR_FIRST_MEASUREMENT_AT
#define USERMOD_SN_PHOTORESISTOR_FIRST_MEASUREMENT_AT 7000
#endif

// supplied voltage
#ifndef USERMOD_SN_PHOTORESISTOR_REFERENCE_VOLTAGE
#define USERMOD_SN_PHOTORESISTOR_REFERENCE_VOLTAGE 5
#endif

// 10 bits
#ifndef USERMOD_SN_PHOTORESISTOR_ADC_PRECISION
#define USERMOD_SN_PHOTORESISTOR_ADC_PRECISION 1024.0
#endif

// resistor size 10K hms
#ifndef USERMOD_LIGHT_BAR_RED_ON_VALUE
#define USERMOD_LIGHT_BAR_RED_ON_VALUE 10000.0  //red analog value considered on
#endif

// only report if differance grater than offset value
#ifndef USERMOD_SN_PHOTORESISTOR_OFFSET_VALUE
#define USERMOD_SN_PHOTORESISTOR_OFFSET_VALUE 5
#endif

class Usermod_A_OW_MOD : public Usermod
{
private:
  float referenceVoltage = USERMOD_SN_PHOTORESISTOR_REFERENCE_VOLTAGE;
  float barValueRed = USERMOD_LIGHT_BAR_RED_ON_VALUE;
  float adcPrecision = USERMOD_SN_PHOTORESISTOR_ADC_PRECISION;
  int8_t offset = USERMOD_SN_PHOTORESISTOR_OFFSET_VALUE;

  unsigned long readingInterval = USERMOD_SN_PHOTORESISTOR_MEASUREMENT_INTERVAL;
  // set last reading as "40 sec before boot", so first reading is taken after 20 sec
  unsigned long lastMeasurement = UINT32_MAX - (USERMOD_SN_PHOTORESISTOR_MEASUREMENT_INTERVAL - USERMOD_SN_PHOTORESISTOR_FIRST_MEASUREMENT_AT);
  // flag to indicate we have finished the first getTemperature call
  // allows this library to report to the user how long until the first
  // measurement

  bool get_LIGHT_BAR_Complete = false;
  uint16_t LIGHT_BAR_R = -1000;
  bool Light_bar_R_BOOL = false;
  bool LIGHT_BAR_G = false;
  bool LIGHT_BAR_B = false;


  // flag set at startup
  bool Status_bar = false;
  bool battery_bar = false;
  bool stock = false;


  // strings to reduce flash memory usage (used more than twice)
  static const char _name[];
  static const char _Status_bar[];
   static const char _stock[];
  static const char _battery_bar[];
  static const char _readInterval[];
  static const char _referenceVoltage[];
  static const char _barValueRed[];
  static const char _adcPrecision[];
  static const char _offset[];

  bool checkBoundSensor(float newValue, float prevValue, float maxDiff)
  {
    return isnan(prevValue) || newValue <= prevValue - maxDiff || newValue >= prevValue + maxDiff;
  }

  void get_LIGHT_BAR()
  {
    // http://forum.arduino.cc/index.php?topic=37555.0
    // https://forum.arduino.cc/index.php?topic=185158.0
LIGHT_BAR_B = digitalRead(LIGHT_BAR_B_PIN);
if (LIGHT_BAR_B == true){
  return;
}

    LIGHT_BAR_R = analogRead(LIGHT_BAR_R_PIN);
    if (LIGHT_BAR_R > 2000){Light_bar_R_BOOL = true;} else {Light_bar_R_BOOL = false;}
    // ^ turn light bar analog red into bool ^
    LIGHT_BAR_G = digitalRead(LIGHT_BAR_G_PIN);
    
    // if status bar rgb blue is on (in the case of white charging or blue foot pad engadement) ignore
  
    get_LIGHT_BAR_Complete = true;
    lastMeasurement = millis();
  }

public:
  void setup()
  {
    // set pinmode
    pinMode(LIGHT_BAR_R_PIN, INPUT);
    pinMode(LIGHT_BAR_G_PIN, INPUT);
    pinMode(LIGHT_BAR_B_PIN, INPUT);
  }

  void loop()
  {
    if (strip.isUpdating()){
      return;
    }
    unsigned long now = millis();

    // check to see if we are due for taking a measurement
    // lastMeasurement will not be updated until the conversion
    // is complete the the reading is finished
    if (now - lastMeasurement < readingInterval)
    {
      return;
    }



  // check if the mirror status bar is true. If it is, set lights to preset colors
  if (Status_bar == true) {
    get_LIGHT_BAR();
  }


    //  if (offset == 1)   // how to change to a preset
     // {
     //   applyPreset(1);
     //  }
    //  else
    //  {
    //    applyPreset(2);
    //  }


  }

  void addToJsonInfo(JsonObject &root)
  {
    JsonObject user = root[F("u")];
    if (user.isNull())
      user = root.createNestedObject(F("u"));

    JsonArray light_bar = user.createNestedArray(F(" lol"));

    if (!get_LIGHT_BAR_Complete)
    {
      // if we haven't read the sensor yet, let the user know
      // that we are still waiting for the first measurement
      light_bar.add((USERMOD_SN_PHOTORESISTOR_FIRST_MEASUREMENT_AT - millis()) / 1000);
      light_bar.add(F(" sec until read"));
      return;
    }

    light_bar.add(LIGHT_BAR_R);
    light_bar.add(F(" Light bar red analog"));
  }

  uint16_t getId()
  {
    return USERMOD_ID_A_OW_MOD;
  }

  /**
     * addToConfig() (called from set.cpp) stores persistent properties to cfg.json
     */
  void addToConfig(JsonObject &root)
  {
    // we add JSON object.
    JsonObject top = root.createNestedObject(FPSTR(_name)); // usermodname
    top[FPSTR(_Status_bar)] = !Status_bar;
    top[FPSTR(_battery_bar)] = !battery_bar;
    top[FPSTR(_stock)] = !stock;
    top[FPSTR(_readInterval)] = readingInterval / 1000;
    top[FPSTR(_referenceVoltage)] = referenceVoltage;
    top[FPSTR(_barValueRed)] = barValueRed;
    top[FPSTR(_adcPrecision)] = adcPrecision;
    top[FPSTR(_offset)] = offset;

    DEBUG_PRINTLN(F("A OW MOD config saved."));
  }

  /**
  * readFromConfig() is called before setup() to populate properties from values stored in cfg.json
  */
  bool readFromConfig(JsonObject &root)
  {
    // we look for JSON object.
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
      DEBUG_PRINT(FPSTR(_name));
      DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
      return false;
    }

    Status_bar         = !(top[FPSTR(_Status_bar)] | !Status_bar);
    battery_bar        = !(top[FPSTR(_battery_bar)] | !battery_bar);
    stock        = !(top[FPSTR(_stock)] | !stock);
    readingInterval  = (top[FPSTR(_readInterval)] | readingInterval/1000) * 1000; // convert to ms
    referenceVoltage = top[FPSTR(_referenceVoltage)] | referenceVoltage;
    barValueRed    = top[FPSTR(_barValueRed)] | barValueRed;
    adcPrecision     = top[FPSTR(_adcPrecision)] | adcPrecision;
    offset           = top[FPSTR(_offset)] | offset;
    DEBUG_PRINT(FPSTR(_name));
    DEBUG_PRINTLN(F(" config (re)loaded."));

    // use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    return true;
  }
};

// strings to reduce flash memory usage (used more than twice)
//                           _veriable         "what it says on the webpage"
const char Usermod_A_OW_MOD::_name[] PROGMEM = "Enabled Features";
const char Usermod_A_OW_MOD::_Status_bar[] PROGMEM = "Mirror Status bar error";
const char Usermod_A_OW_MOD::_battery_bar[] PROGMEM = "Display battery on dismount";
const char Usermod_A_OW_MOD::_stock[] PROGMEM = "Emulate stock lighting (override everything)";
const char Usermod_A_OW_MOD::_readInterval[] PROGMEM = "read-interval-s";
const char Usermod_A_OW_MOD::_referenceVoltage[] PROGMEM = "supplied-voltage";
const char Usermod_A_OW_MOD::_barValueRed[] PROGMEM = "barValueRed what is concidered on for red";
const char Usermod_A_OW_MOD::_adcPrecision[] PROGMEM = "adc-precision";
const char Usermod_A_OW_MOD::_offset[] PROGMEM = "offset";