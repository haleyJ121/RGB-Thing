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

class Usermod_ANDON_STD : public Usermod
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

#ifndef PRO_VERSION
  int LIGHT_BAR_R_ANALOG;
  bool LIGHT_BAR_R = false;
  bool LIGHT_BAR_G = false;
  bool LIGHT_BAR_B = false;

  bool Status_bar = false;
  bool battery_bar = false;

#endif
  
  bool FRONT_LIGHT_R = false;
  int FRONT_LIGHT_R_ANALOG;
  bool FRONT_LIGHT_W = false;
  int FRONT_LIGHT_W_ANALOG;
  
  // flag set at startup
  bool forward = true; //upon system startup, assume forward board movement
  bool app_lights_on;  // are the head/tail lights on in the app?
  bool show_lights;  // are the lights turned on in the FM app? (not set up yet)
  int8_t choosen_preset;


  bool stock = false; // is stock lighitng override enabled?
  bool toggle = false;
  int stac;  // number of  ppl on wifi (untested may not work idk)
  
  // strings to reduce flash memory usage (used more than twice)
  static const char _name[];
  #ifndef PRO_VERSION
  static const char _Status_bar[];
  static const char _battery_bar[];
  #endif
  static const char _stock[];
  static const char _toggle[];
  static const char _show_lights[];
  static const char _choosen_preset[];
  static const char _readInterval[];
  static const char _referenceVoltage[];
  static const char _resistorValue[];
  static const char _adcPrecision[];
  static const char _offset[];

  bool checkBoundSensor(float newValue, float prevValue, float maxDiff)
  {
    return isnan(prevValue) || newValue <= prevValue - maxDiff || newValue >= prevValue + maxDiff;
  }

  //ANDON PRO-specific functions:
  /**

    void MOTOR_ENGAGED() //determine if the motor is engaged (used as trigger for board-idle animations)
    {

    }

    void GET_SPEED() //determine a rough estimate of how fast the motor is spinning
    {
      //the top speed of these boards is defined as their freespin speed, take a range of 0 to [freespin speed]
      //subdivided into N sub-ranges. These sub-ranges should be of a high enough fidelity that animations are not
      //visually stuttery as speed increases/decreases (roughly 8-10? make N dynamic/programmable if easy to do).
      //N is proportional to a scalar value, which is used to scale up/down the speed of the lighting animation
      //when the board speed is within one of the ranges, scale the animation speed accordingly 
    }

    void GET_BATT() //determine the battery level based upon battery voltage input
    {
      //most of the time, this function should be performed when the baord is idle, because when the board is engaged, 
      //voltage will drop as more amperage is drawn. This can be programmed/accounted for, but will take time to develop for
      //a feature that is seldom used when riding
    }

    void mimic_error_codes() //if the lightbar is blinking (condition 1) orange (condition 2), make the head/taillights do the same
    {
      
    }

    void OVERRIDE_LIGHTBAR() //when the lightbar is not being turned on by FM controls, enable this function
    {
      //to tell Andon that it is able to output data to the lightbar (requires additinal LEDs/hardware to light up the lightbar)
    }

    void SHOP_MODE() //enables shop diagnostics tools
    {

    }

    void SPEEDSCALE_BRIGHTNESS() //As the board goes faster, make the headlights get brighter and the taillights dimmer, so the 
    //total amount of power used remains the same. This increases headlight throw, which is more useful at speed
    {

    }

    void BATTERY_GRAPH() //when the board is idle, display the battery level as a bar graph upon the rear led pcb upon the ground
    //optionally (if easy to program) set the front led pcb to visualize cell balance levels at a very dim brightness
    {

    }

    void IMU_POSN_INPUT() //determine board orientation 
    {

    }

    void isMallgrab() //using IMU_POSN_INPUT, determine if the board is being carried vertically by the front handle
    {

    }

    void isAirborne() //using IMU_POSN_INPUT, approximate if the board is travelling in an inverted parabola, if the board's trajectory
    // matches a perfect parabola within a programmable threshold, set a boolean to represent being airborne or not 
    {

    }

    void trailRate() //use IMU data input to quantify the following:
    // 1.) trail chunkiness - a function of z-variance detected by the IMU, measured over a long timescale
    // 2.) altitude gain/loss - a function of the average slope of z acceleration measured over long distances
    // 3.) average speed - a function of how high/low the average speed of a run is
    // 4.) motor disengagement - worse trails require you to dismount more frequently, better trails do not. count 
    // how many times the motor disengages during a given run
    //how is the start/stop of a trail or segment measured?
    //all of the above calculation results in a color assignment to the trail run, like how trails are rated green, blue, black, etc
    {

    }

    void isBraking() //using IMU_POSN_INPUT, determine when the board is decellerating, blink the brake light 
    // eitherred/ or something with high visibility like F1 cars in the rain
    {

    }

    void trickTrainer() //using IMU_POSN_INPUT, take pre-defined profiles for things like curb nudges, bonks, revert 180's, etc
    //and quantify/define them as a function of motor speed/acceleration, board orientation, and cartesian position to help new
    //riders better achieve tricks. the idea is to provide feedback of "you need to push the nose down harder to nudge" or
    //you need to be going faster and sink the nose more to 180" upon attempting a trick, the LEDs are on a color-scale from
    //red to green (good to bad) of how closely your attempt matched the profile for that trick. (identifying how a trick is
    //defined could conceptuallybe as simple as recording the same metrics used to evaluate the new rider to compare against)
    {

    }

    void timer_countdown() //auxilliary countdown timer to be used within other functions, mainly for transition delays
    // between board enganged/disengaged and direction change states

  **/










  void BOOTUP_SEQUENCE() //user defined bootup light animation that only runs in the first loop of setup() upon powering the board on
  {
    //store the animation here
    //store the color channels here
    //store the brightness here
    //store the animation speed here
    //store the duration here
  }

  void get_LIGHT_BAR() //see what color the GT/pint lightbar is, if it is on at all
  // if the GT/pint lightbar is displaying blue light (in the case of white (charging/displaying battery level)
  // or blue (foot pad engagement feedback) ignore the lightbar output)  
  {
    #ifndef PRO_VERSION
    // http://forum.arduino.cc/index.php?topic=37555.0
    // https://forum.arduino.cc/index.php?topic=185158.0

    LIGHT_BAR_B = digitalRead(LIGHT_BAR_B_PIN); //see if the lightbar is outputting blue light (Y/N)

    if (LIGHT_BAR_B == false /** && the lightbar is on**/) //if there is no blue light, determine what color the lightbar is
    {
      LIGHT_BAR_R = digitalRead(LIGHT_BAR_R_PIN); //see if the lightbar is outputting any red light (Y/N)
      LIGHT_BAR_R_ANALOG = analogRead(LIGHT_BAR_R_PIN); //see how much red light the lightbar is outputting, if any
      LIGHT_BAR_G = digitalRead(LIGHT_BAR_G_PIN); // see if the lightbar is outputting any green light (Y/N)
    }
  #endif
  }

  void get_DIRECTION() //determine what direction the board is going in (forward/reverse; no stationary)
  {

    FRONT_LIGHT_W = digitalRead(FRONT_LIGHT_W_PIN); //is the front light strip white? (Y/N)
    FRONT_LIGHT_W_ANALOG = analogRead(FRONT_LIGHT_W_PIN); //how bright is the white LED on the front LED strip? (faster detection than digitalRead)

    FRONT_LIGHT_R = digitalRead(FRONT_LIGHT_R_PIN); //is the front light red? (Y/N)
    FRONT_LIGHT_R_ANALOG = analogRead(FRONT_LIGHT_R_PIN); //how bright is the white LED on the front LED strip? (faster detection than the digitalRead)

    if (FRONT_LIGHT_W == true || FRONT_LIGHT_R == true) //if the board's lights are turned on in the app
    { 
      //switch to using analog input to detect switch sooner (needs testing on live board)
    app_lights_on = true;
    if (FRONT_LIGHT_W == true) // if the board is going forward
    {  
      forward = true;
    } 
    else if (FRONT_LIGHT_R == true) //if the board is going backwards
    {
       forward = false;
    }

    } else //the head/tail lights are not on 
    { 
      app_lights_on = false;
      forward = true;
      // if the board lights are off, assume the board is going forward. Otherwise, if someone turns lights
      // off while going backwards, the program will retain the backwards direction permanently
    }
  }


  void emulate_stock() //restore stock headlight/tailight functionality
  {
    if (forward = true) //if the board is going forward
    {
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
    } else if (forward == false) //if the board is going backwards
    {
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
  void setup() //set up I/O
    {
    #ifndef PRO_VERSION
    pinMode(LIGHT_BAR_R_PIN, INPUT); //pint/GT lightbar red channel input
    pinMode(LIGHT_BAR_G_PIN, INPUT); //pint/GT lightbar red channel input
    pinMode(LIGHT_BAR_B_PIN, INPUT); //pint/GT lightbar red channel input
    //pinMode([motor phase input variable], INPUT) //motor phase signal
    //pinMode([battery voltage input variable], INPUT) //battery voltage
    //pinMode([IMU input variable], INPUT) //IMU data input
    #endif
    pinMode(FRONT_LIGHT_W_PIN, INPUT); //front headlight
    pinMode(FRONT_LIGHT_R_PIN, INPUT); //front taillight
    pinMode(REAR_LIGHT_W_PIN, INPUT); //rear headlight
    pinMode(REAR_LIGHT_R_PIN, INPUT); //rear taillight
    BOOTUP_SEQUENCE();
  }

  //*************************************************************************************************************************************************************************
  //*************************************************************************************************************************************************************************
  //********************************************************************************* MAIN **********************************************************************************
  //*************************************************************************************************************************************************************************
  //*************************************************************************************************************************************************************************

  void loop() //main
  {
    if (strip.isUpdating())
      return;

    unsigned long now = millis();
    // check to see if we are due for taking a measurement
    // lastMeasurement will not be updated until the conversion
    // is complete the the reading is finished
    if (now - lastMeasurement < readingInterval)
    {
      return;
    }

    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);
    stac = stationList.num;

    if (0 != stac)
    {
      get_DIRECTION();
      if (stock == true)
      {
        emulate_stock();
      } else 
      {
        #ifndef PRO_VERSION
        get_LIGHT_BAR();
        #endif
      }
  }   
  } // end of main loop

  void addToJsonInfo(JsonObject &root)
  {
    JsonObject user = root[F("u")];
    if (user.isNull())
    {
      user = root.createNestedObject(F("u"));
    }
    #ifndef PRO_VERSION
    JsonArray lux = user.createNestedArray(F(" lol")); //left side thing
    lux.add(LIGHT_BAR_R_ANALOG);                       //right side variable
    lux.add(F(" RED analog read"));                    //right side thing

    JsonArray battery = user.createNestedArray("blue level");  //left side thing
    battery.add(LIGHT_BAR_B);                               //right side variable
    battery.add(F(" BLUE GPIO read"));                      //right side thing
    #endif
    JsonArray clients = user.createNestedArray("Number of ap clients");  //left side thing
    clients.add(stac);                               //right side variable
    clients.add(F(" Clients"));                      //right side thing
  }

  uint16_t getId()
  {
    return USERMOD_ID_ANDON_STD;
  }

  /**
  * addToConfig() (called from set.cpp) stores persistent properties to cfg.json
  **/
  void addToConfig(JsonObject &root)
  {
    // we add JSON object.
    JsonObject top = root.createNestedObject(FPSTR(_name)); // usermodname
    #ifndef PRO_VERSION
    top[FPSTR(_Status_bar)] = !Status_bar;
    top[FPSTR(_battery_bar)] = !battery_bar;
    #endif
    top[FPSTR(_stock)] = !stock;
    top[FPSTR(_toggle)] = !toggle;
    top[FPSTR(_show_lights)] = show_lights;
    top[FPSTR(_choosen_preset)] = choosen_preset;
    top[FPSTR(_readInterval)] = readingInterval / 1000;
    top[FPSTR(_referenceVoltage)] = referenceVoltage;
    top[FPSTR(_resistorValue)] = resistorValue;
    top[FPSTR(_adcPrecision)] = adcPrecision;
    top[FPSTR(_offset)] = offset;

    DEBUG_PRINTLN(F("Andon config saved."));
  }

  /**
  * readFromConfig() is called before setup() to populate properties from values stored in cfg.json
  **/
  
  bool readFromConfig(JsonObject &root)
  {
    // we look for JSON object.
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
      DEBUG_PRINT(FPSTR(_name));
      DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
      return false;
    }
    #ifndef PRO_VERSION
    Status_bar       = !(top[FPSTR(_Status_bar)] | !Status_bar);
    battery_bar      = !(top[FPSTR(_battery_bar)] | !battery_bar);
    #endif
    stock            = !(top[FPSTR(_stock)] | !stock);
    toggle           = !(top[FPSTR(_toggle)] | !toggle);
    show_lights      = top[FPSTR(_show_lights)] | show_lights;
    choosen_preset   = top[FPSTR(_choosen_preset)] | choosen_preset;
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
const char Usermod_ANDON_STD::_name[] PROGMEM = "Enabled Features";
#ifndef PRO_VERSION
const char Usermod_ANDON_STD::_Status_bar[] PROGMEM = "Mirror Status bar error";
const char Usermod_ANDON_STD::_battery_bar[] PROGMEM = "Display battery on dismount";
#endif
const char Usermod_ANDON_STD::_stock[] PROGMEM = "Emulate stock lighting (override everything)";
const char Usermod_ANDON_STD::_toggle[] PROGMEM = "In app on/off button or toggle";
const char Usermod_ANDON_STD::_choosen_preset[] PROGMEM = "Preset animation to use while riding";
const char Usermod_ANDON_STD::_show_lights[] PROGMEM = "show_lights";

const char Usermod_ANDON_STD::_readInterval[] PROGMEM = "read-interval-s";
const char Usermod_ANDON_STD::_referenceVoltage[] PROGMEM = "supplied-voltage";
const char Usermod_ANDON_STD::_resistorValue[] PROGMEM = "resistor-value";
const char Usermod_ANDON_STD::_adcPrecision[] PROGMEM = "adc-precision";
const char Usermod_ANDON_STD::_offset[] PROGMEM = "offset";