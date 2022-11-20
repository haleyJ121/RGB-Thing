#pragma once

#include "wled.h"

//Pin defaults for QuinLed Dig-Uno (A0)
#define PHOTORESISTOR_PIN A0

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
#ifndef USERMOD_SN_PHOTORESISTOR_RESISTOR_VALUE
#define USERMOD_SN_PHOTORESISTOR_RESISTOR_VALUE 10000.0
#endif

// only report if differance grater than offset value
#ifndef USERMOD_SN_PHOTORESISTOR_OFFSET_VALUE
#define USERMOD_SN_PHOTORESISTOR_OFFSET_VALUE 5
#endif

class Usermod_A_OW_MOD : public Usermod
{
private:
  float referenceVoltage = USERMOD_SN_PHOTORESISTOR_REFERENCE_VOLTAGE;
  float resistorValue = USERMOD_SN_PHOTORESISTOR_RESISTOR_VALUE;
  float adcPrecision = USERMOD_SN_PHOTORESISTOR_ADC_PRECISION;
  int8_t offset = USERMOD_SN_PHOTORESISTOR_OFFSET_VALUE;

  unsigned long readingInterval = USERMOD_SN_PHOTORESISTOR_MEASUREMENT_INTERVAL;
  // set last reading as "40 sec before boot", so first reading is taken after 20 sec
  unsigned long lastMeasurement = UINT32_MAX - (USERMOD_SN_PHOTORESISTOR_MEASUREMENT_INTERVAL - USERMOD_SN_PHOTORESISTOR_FIRST_MEASUREMENT_AT);
  // flag to indicate we have finished the first getTemperature call
  // allows this library to report to the user how long until the first
  // measurement
  bool getLuminanceComplete = false;
  uint16_t lastLDRValue = -1000;

  int LIGHT_BAR_R_ANALOG;
  bool LIGHT_BAR_R = false;
  bool LIGHT_BAR_G = false;
  bool LIGHT_BAR_B = false;

  bool FRONT_LIGHT_R = false;
  int FRONT_LIGHT_R_ANALOG;
  bool FRONT_LIGHT_W = false;
  int FRONT_LIGHT_W_ANALOG;

  

  // flag set at startup
  bool forward = true; //on startup assume forware movment
  bool lights_on;  // are the lights on in the app?

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
  static const char _resistorValue[];
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

    if (LIGHT_BAR_B == false){
      LIGHT_BAR_R = digitalRead(LIGHT_BAR_R_PIN);
      LIGHT_BAR_R_ANALOG = analogRead(LIGHT_BAR_R_PIN);
      LIGHT_BAR_G = digitalRead(LIGHT_BAR_G_PIN);
    }
    // if status bar rgb blue is on (in the case of white charging or blue foot pad engadement) ignore
  }


  void get_FRONT_LIGHT()
  {

    FRONT_LIGHT_W = digitalRead(FRONT_LIGHT_W_PIN);
    FRONT_LIGHT_W_ANALOG = analogRead(FRONT_LIGHT_W_PIN);

    FRONT_LIGHT_R = digitalRead(FRONT_LIGHT_R_PIN);
    FRONT_LIGHT_R_ANALOG = analogRead(LIGHT_BAR_R_PIN);

    if (FRONT_LIGHT_W == true || FRONT_LIGHT_R == true){ 
      //switch to using analog input to detect switch sooner needs testing on live board
    lights_on = true;
    if (FRONT_LIGHT_W == true){  // if white rgbw front light is on board is going forward
       forward = true;
      } else {
       forward = false;
      }

    } else {
      lights_on = false;
      forward = true;  
      // if lights are off assume forward to avoide if someone turns lights
      // off while going backwards being stuck in backwards within the program
    }
    // if status bar rgb blue is on (in the case of white charging or blue foot pad engadement) ignore
  }


  void emulate_stock()
  {
if (forward = true) {
WS2812FX::Segment& seg = strip.getSegment(0); // segment 0 is front lights
//set color (i=0 is primary, i=1 secondary i=2 tertiary)
seg.colors[0] = ((255 << 24) | ((0&0xFF) << 16) | ((0&0xFF) << 8) | ((0&0xFF)));
//set effect config
seg.mode = 0;  //effect 0 = solid

seg = strip.getSegment(1); // segment 0 is front lights
//set color (i=0 is primary, i=1 secondary i=2 tertiary)
seg.colors[0] = ((0 << 24) | ((255&0xFF) << 16) | ((0&0xFF) << 8) | ((0&0xFF)));
//set effect config
seg.mode = 0;  //effect 0 = solid

} else {

WS2812FX::Segment& seg = strip.getSegment(1); // segment 0 is front lights 1 is back
//set color (i=0 is primary, i=1 secondary i=2 tertiary)
seg.colors[0] = ((255 << 24) | ((0&0xFF) << 16) | ((0&0xFF) << 8) | ((0&0xFF)));
//set effect config
seg.mode = 0;  //effect 0 = solid

seg = strip.getSegment(0); // segment 0 is front lights 1 is back
//set color (i=0 is primary, i=1 secondary i=2 tertiary)
seg.colors[0] = ((0 << 24) | ((255&0xFF) << 16) | ((0&0xFF) << 8) | ((0&0xFF)));
//set effect config
seg.mode = 0;  //effect 0 = solid

}
  }

public:
  void setup()
  {
    // set pinmode
    pinMode(LIGHT_BAR_R_PIN, INPUT);
    pinMode(LIGHT_BAR_G_PIN, INPUT);
    pinMode(LIGHT_BAR_B_PIN, INPUT);
    pinMode(FRONT_LIGHT_W_PIN, INPUT);
    pinMode(FRONT_LIGHT_R_PIN, INPUT);
  }

  void loop()
  {
    if (Status_bar || strip.isUpdating())
      return;

    unsigned long now = millis();

    // check to see if we are due for taking a measurement
    // lastMeasurement will not be updated until the conversion
    // is complete the the reading is finished
    if (now - lastMeasurement < readingInterval)
    {
      return;
    }

      //set effect parameters

      /**
  if (updateVal(&req, "FX=", &effectCurrent, 0, strip.getModeCount()-1) && request != nullptr) unloadPlaylist();  //unload playlist if changing FX using web request
  updateVal(&req, "SX=", &effectSpeed);
  updateVal(&req, "IX=", &effectIntensity);
  updateVal(&req, "FP=", &effectPalette, 0, strip.getPaletteCount()-1);
*/



    //  if (offset == 1)   // how to change to a preset
     // {
     //   applyPreset(1);
     //  }
    //  else
    //  {
    //    applyPreset(2);
    //  }

    //effectSpeed
    //effectPalette = 7;







   get_FRONT_LIGHT();
   emulate_stock();
  
   get_LIGHT_BAR();

  } // end of main loop

  void addToJsonInfo(JsonObject &root)
  {
    JsonObject user = root[F("u")];
    if (user.isNull())
      user = root.createNestedObject(F("u"));

    JsonArray lux = user.createNestedArray(F(" lol")); //left side thing
    lux.add(LIGHT_BAR_R_ANALOG);                       //right side variable
    lux.add(F(" RED analog read"));                    //right side thing


      JsonArray battery = user.createNestedArray("blue level");  //left side thing
      battery.add(LIGHT_BAR_B);                               //right side variable
      battery.add(F(" BLUE GPIO read"));                      //right side thing
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
    top[FPSTR(_resistorValue)] = resistorValue;
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
    resistorValue    = top[FPSTR(_resistorValue)] | resistorValue;
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
const char Usermod_A_OW_MOD::_resistorValue[] PROGMEM = "resistor-value";
const char Usermod_A_OW_MOD::_adcPrecision[] PROGMEM = "adc-precision";
const char Usermod_A_OW_MOD::_offset[] PROGMEM = "offset";