#pragma once

/*
 * Welcome! 
 * You can use the file "my_config.h" to make changes to the way WLED is compiled!
 * It is possible to enable and disable certain features as well as set defaults for some runtime changeable settings.
 * 
 * How to use:
 * PlatformIO: Just compile the unmodified code once! The file "my_config.h" will be generated automatically and now you can make your changes.
 * 
 * ArduinoIDE: Make a copy of this file and name it "my_config.h". Go to wled.h and uncomment "#define WLED_USE_MY_CONFIG" in the top of the file.
 * 
 * DO NOT make changes to the "my_config_sample.h" file directly! Your changes will not be applied.
 */

// force the compiler to show a warning to confirm that this file is included
#warning **** my_config.h: Settings from this file are honored ****

/* Uncomment to use your WIFI settings as defaults
  //WARNING: this will hardcode these as the default even after a factory reset
*/

#define CLIENT_SSID "TRENDnet828_2.4GHz_3FDB"
#define CLIENT_PASS "8280RH90029"
#define DEFAULT_LED_COUNT 11
#define LEDPIN 26
#define LIGHT_BAR_R_PIN 36  //status bar red
#define LIGHT_BAR_G_PIN 39   //status bar green
#define LIGHT_BAR_B_PIN 18   //status bar blue

#define FRONT_LIGHT_R_PIN 36   //status bar blue
#define FRONT_LIGHT_W_PIN 39   //status bar blue

#define DEFAULT_AP_PASS     "andon123"
#define DEFAULT_OTA_PASS    "andon123"

//#define MAX_LEDS 1500       //Maximum total LEDs. More than 1500 might create a low memory situation on ESP8266.