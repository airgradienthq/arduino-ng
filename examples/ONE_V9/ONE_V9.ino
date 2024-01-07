/*
Important: This code is only for the DIY PRO / AirGradient ONE PCB Version 9
with the ESP-C3 MCU.

It is a high quality sensor showing PM2.5, CO2, TVOC, NOx, Temperature and
Humidity on a small display and LEDbar and can send data over Wifi.

Build Instructions: https://www.airgradient.com/open-airgradient/instructions/

Kits (including a pre-soldered version) are available:
https://www.airgradient.com/indoor/

The codes needs the following libraries installed:
“WifiManager by tzapu, tablatronix” tested with version 2.0.11-beta
“U8g2” by oliver tested with version 2.32.15
"Sensirion I2C SGP41" by Sensation Version 0.1.0
"Sensirion Gas Index Algorithm" by Sensation Version 3.2.1
"Arduino-SHT" by Johannes Winkelmann Version 1.2.2
"Adafruit NeoPixel" by Adafruit Version 1.11.0

Configuration:
Please set in the code below the configuration parameters.

If you have any questions please visit our forum at
https://forum.airgradient.com/

If you are a school or university contact us for a free trial on the AirGradient
platform. https://www.airgradient.com/

CC BY-SA 4.0 Attribution-ShareAlike 4.0 International License

*/

#include <EEPROM.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <WiFiManager.h>

#include <AirGradient.h>
#include <U8g2lib.h>

#define FW_VER      "1.0.0"

#define DEBUG true

HTTPClient client;
AirGradient ag(BOARD_ONE_INDOOR_MONITOR_V9_0);

// for peristent saving and loading
int addr = 4;
byte value;

// Display bottom right
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

String APIROOT = "http://hw.airgradient.com/";

// set to true to switch from Celcius to Fahrenheit
boolean inF = false;

// PM2.5 in US AQI (default ug/m3)
boolean inUSAQI = false;

// Display Position
boolean displayTop = true;

// use RGB LED Bar
boolean useRGBledBar = true;

// set to true if you want to connect to wifi. You have 60 seconds to connect.
// Then it will go into an offline mode.
boolean connectWIFI = true;

int loopCount = 0;

unsigned long currentMillis = 0;

const int oledInterval = 5000;
unsigned long previousOled = 0;

const int sendToServerInterval = 10000;
unsigned long previoussendToServer = 0;

const int tvocInterval = 1000;
unsigned long previousTVOC = 0;
int TVOC = -1;
int NOX = -1;

const int co2Interval = 5000;
unsigned long previousCo2 = 0;
int Co2 = 0;

const int pmInterval = 5000;
unsigned long previousPm = 0;
int pm25 = -1;
int pm01 = -1;
int pm10 = -1;
int pm03PCount = -1;

const int tempHumInterval = 5000;
unsigned long previousTempHum = 0;
float temp;
int hum;

int buttonConfig = 0;
int lastState = LOW;
int currentState;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

void boardInit(void);
void failedHandler(void);

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /** Init EEPROM */
  EEPROM.begin(512);

  /** Init I2C */
  const int I2C_SDA = ag.getI2cSdaPin();
  const int I2C_SCL = ag.getI2cSclPin();
  Wire.begin(I2C_SDA, I2C_SCL);

  /** Display init */
  u8g2.begin();

  /** Show boot display */
  displayShowText("One V9", "Fw Ver: " + String(FW_VER), "Lib Ver: " + ag.getVersion());
  delay(2000); /** Boot display wait */

  /** Init sensor */
  boardInit();

  /** Get las configure */
  buttonConfig = String(EEPROM.read(addr)).toInt();
  if (buttonConfig > 3)
    buttonConfig = 0;

  // delay(400);
  setConfig();
  Serial.println("buttonConfig: " + String(buttonConfig));
  displayShowText("Press Button", "for LED test &", "offline mode");
  delay(2000);

  if (ag.button.getState() == PushButton::BUTTON_PRESSED) {
    ledTest();
    return;
  }

  displayShowText("Press Button", "Now for", "Config Menu");
  delay(2000);

  if (ag.button.getState() == PushButton::BUTTON_PRESSED) {
    displayShowText("Entering", "Config Menu", "");
    delay(3000);
    lastState = HIGH;
    setConfig();
    inConf();
  }

  if (connectWIFI)
    connectToWifi();
  if (WiFi.status() == WL_CONNECTED) {
    sendPing();
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  displayShowText("Warming Up", "Serial Number:", String(getNormalizedMac()));
  delay(2000);
}

void loop() {
  currentMillis = millis();
  updateTVOC();
  updateOLED();
  updateCo2();
  updatePm();
  updateTempHum();
  sendToServer();
}

void ledTest() {
  displayShowText("LED Test", "running", ".....");
  setRGBledColor('r');
  delay(1000);
  setRGBledColor('g');
  delay(1000);
  setRGBledColor('b');
  delay(1000);
  setRGBledColor('w');
  delay(1000);
  setRGBledColor('n');
  delay(1000);
  // LED Test
}

void updateTVOC() {
  delay(1000);

  if (currentMillis - previousTVOC >= tvocInterval) {
    previousTVOC += tvocInterval;

    TVOC = ag.sgp41.getTvocIndex();
    NOX = ag.sgp41.getNoxIndex();

    Serial.println(String(TVOC));
    Serial.println(String(TVOC));
  }
}

void updateCo2() {
  if (currentMillis - previousCo2 >= co2Interval) {
    previousCo2 += co2Interval;
    Co2 = ag.s8.getCo2();
    Serial.println(String(Co2));
  }
}

void updatePm() {
  if (currentMillis - previousPm >= pmInterval) {
    previousPm += pmInterval;
    if (ag.pms5003.readData()) {
      pm01 = ag.pms5003.getPm01Ae();
      pm25 = ag.pms5003.getPm25Ae();
      pm10 = ag.pms5003.getPm10Ae();
      pm03PCount = ag.pms5003.getPm03ParticleCount();
    } else {
      pm01 = -1;
      pm25 = -1;
      pm10 = -1;
      pm03PCount = -1;
    }
  }
}

void updateTempHum() {
  if (currentMillis - previousTempHum >= tempHumInterval) {
    previousTempHum += tempHumInterval;
    temp = ag.sht.getTemperature();
    hum = ag.sht.getRelativeHumidity();
  }
}

void updateOLED() {
  if (currentMillis - previousOled >= oledInterval) {
    previousOled += oledInterval;

    String ln3;
    String ln1;

    if (inUSAQI) {
      ln1 = "AQI:" + String(ag.pms5003.convertPm25ToUsAqi(pm25)) +
            " CO2:" + String(Co2);
    } else {
      ln1 = "PM:" + String(pm25) + " CO2:" + String(Co2);
    }

    String ln2 = "TVOC:" + String(TVOC) + " NOX:" + String(NOX);

    if (inF) {
      ln3 = "F:" + String((temp * 9 / 5) + 32) + " H:" + String(hum) + "%";
    } else {
      ln3 = "C:" + String(temp) + " H:" + String(hum) + "%";
    }
    displayShowDashboard();
    setRGBledCO2color(Co2);
  }
}

void inConf() {
  setConfig();
  currentState = digitalRead(9);

  if (currentState) {
    Serial.println("currentState: high");
  } else {
    Serial.println("currentState: low");
  }

  if (lastState == HIGH && currentState == LOW) {
    pressedTime = millis();
  } else if (lastState == LOW && currentState == HIGH) {
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    if (pressDuration < 1000) {
      buttonConfig = buttonConfig + 1;
      if (buttonConfig > 3)
        buttonConfig = 0;
    }
  }

  if (lastState == LOW && currentState == LOW) {
    long passedDuration = millis() - pressedTime;
    if (passedDuration > 4000) {
      displayShowText("Saved", "Release", "Button Now");
      delay(1000);
      displayShowText("Rebooting", "in", "5 seconds");
      delay(5000);
      EEPROM.write(addr, char(buttonConfig));
      EEPROM.commit();
      delay(1000);
      ESP.restart();
    }
  }
  lastState = currentState;
  delay(100);
  inConf();
}

void setConfig() {
  Serial.println("in setConfig");
  if (buttonConfig == 0) {
    displayShowText("Temp. in C", "PM in ug/m3", "Long Press Saves");
    u8g2.setDisplayRotation(U8G2_R0);
    inF = false;
    inUSAQI = false;
  }
  if (buttonConfig == 1) {
    displayShowText("Temp. in C", "PM in US AQI", "Long Press Saves");
    u8g2.setDisplayRotation(U8G2_R0);
    inF = false;
    inUSAQI = true;
  } else if (buttonConfig == 2) {
    displayShowText("Temp. in F", "PM in ug/m3", "Long Press Saves");
    u8g2.setDisplayRotation(U8G2_R0);
    inF = true;
    inUSAQI = false;
  } else if (buttonConfig == 3) {
    displayShowText("Temp. in F", "PM in US AQI", "Long Press Saves");
    u8g2.setDisplayRotation(U8G2_R0);
    inF = true;
    inUSAQI = true;
  }
}

void sendPing() {
  String payload =
      "{\"wifi\":" + String(WiFi.RSSI()) + ", \"boot\":" + loopCount + "}";
}

void displayShowText(String ln1, String ln2, String ln3) {
  char buf[9];
  // u8g2.firstPage();
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_t0_16_tf);
    u8g2.drawStr(1, 10, String(ln1).c_str());
    u8g2.drawStr(1, 30, String(ln2).c_str());
    u8g2.drawStr(1, 50, String(ln3).c_str());
  } while (u8g2.nextPage());
}

void displayShowDashboard() {
  char buf[9];

  /** Clear display dashboard */
  u8g2.firstPage();

  /** Show content to dashboard */
  do {
    u8g2.setFont(u8g2_font_t0_16_tf);

    /** Show temperature */
    if (inF) {
      if (temp > -10001) {
        float tempF = (temp * 9 / 5) + 32;
        sprintf(buf, "%.1f°F", tempF);
      } else {
        sprintf(buf, "-°F");
      }
      u8g2.drawUTF8(1, 10, buf);
    } else {
      if (temp > -10001) {
        sprintf(buf, "%.1f°C", temp);
      } else {
        sprintf(buf, "-°C");
      }
      u8g2.drawUTF8(1, 10, buf);
    }

    /** Show humidity */
    if (hum >= 0) {
      sprintf(buf, "%d%%", hum);
    } else {
      sprintf(buf, " -%%");
    }
    if (hum > 99) {
      u8g2.drawStr(97, 10, buf);
    } else {
      u8g2.drawStr(105, 10, buf);
      // there might also be single digits, not considered, sprintf might
      // actually support a leading space
    }

    /** Draw horizontal line */
    u8g2.drawLine(1, 13, 128, 13);

    /** Show CO2 label */
    u8g2.setFont(u8g2_font_t0_12_tf);
    u8g2.drawUTF8(1, 27, "CO2");

    /** Show CO2 value */
    u8g2.setFont(u8g2_font_t0_22b_tf);
    if (Co2 > 0) {
      sprintf(buf, "%d", Co2);
    } else {
      sprintf(buf, "%s", "-");
    }
    u8g2.drawStr(1, 48, buf);

    /** Show CO2 value index */
    u8g2.setFont(u8g2_font_t0_12_tf);
    u8g2.drawStr(1, 61, "ppm");

    /** Draw vertical line */
    u8g2.drawLine(45, 15, 45, 64);

    /** Draw PM2.5 label */
    u8g2.setFont(u8g2_font_t0_12_tf);
    u8g2.drawStr(48, 27, "PM2.5");

    /** Draw PM2.5 value */
    u8g2.setFont(u8g2_font_t0_22b_tf);
    if (inUSAQI) {
      if (pm25 >= 0) {
        sprintf(buf, "%d", ag.pms5003.convertPm25ToUsAqi(pm25));
      } else {
        sprintf(buf, "%s", "-");
      }
      u8g2.drawStr(48, 48, buf);
      u8g2.setFont(u8g2_font_t0_12_tf);
      u8g2.drawUTF8(48, 61, "AQI");
    } else {
      if (pm25 >= 0) {
        sprintf(buf, "%d", pm25);
      } else {
        sprintf(buf, "%s", "-");
      }
      u8g2.drawStr(48, 48, buf);
      u8g2.setFont(u8g2_font_t0_12_tf);
      u8g2.drawUTF8(48, 61, "ug/m³");
    }

    /** Draw vertical line */
    u8g2.drawLine(82, 15, 82, 64);

    /** Draw TVOC label */
    u8g2.setFont(u8g2_font_t0_12_tf);
    u8g2.drawStr(85, 27, "TVOC:");

    /** Draw TVOC value */
    if (TVOC >= 0) {
      sprintf(buf, "%d", TVOC);
    } else {
      sprintf(buf, "%s", "-");
    }
    u8g2.drawStr(85, 39, buf);

    /** Draw NOx label */
    u8g2.drawStr(85, 53, "NOx:");
    if (NOX >= 0) {
      sprintf(buf, "%d", NOX);
    } else {
      sprintf(buf, "%s", "-");
    }
    u8g2.drawStr(85, 63, buf);
  } while (u8g2.nextPage());
}

void sendToServer() {
  if (currentMillis - previoussendToServer >= sendToServerInterval) {
    previoussendToServer += sendToServerInterval;
    String payload =
        "{\"wifi\":" + String(WiFi.RSSI()) +
        (Co2 < 0 ? "" : ", \"rco2\":" + String(Co2)) +
        (pm01 < 0 ? "" : ", \"pm01\":" + String(pm01)) +
        (pm25 < 0 ? "" : ", \"pm02\":" + String(pm25)) +
        (pm10 < 0 ? "" : ", \"pm10\":" + String(pm10)) +
        (pm03PCount < 0 ? "" : ", \"pm003_count\":" + String(pm03PCount)) +
        (TVOC < 0 ? "" : ", \"tvoc_index\":" + String(TVOC)) +
        (NOX < 0 ? "" : ", \"nox_index\":" + String(NOX)) +
        ", \"atmp\":" + String(temp) +
        (hum < 0 ? "" : ", \"rhum\":" + String(hum)) +
        ", \"boot\":" + loopCount + "}";

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(payload);
      String POSTURL = APIROOT +
                       "sensors/airgradient:" + String(getNormalizedMac()) +
                       "/measures";
      Serial.println(POSTURL);
      WiFiClient client;
      HTTPClient http;
      http.begin(client, POSTURL);
      http.addHeader("content-type", "application/json");
      int httpCode = http.POST(payload);
      String response = http.getString();
      Serial.println(httpCode);
      Serial.println(response);
      http.end();
      resetWatchdog();
      loopCount++;
    } else {
      Serial.println("WiFi Disconnected");
    }
  }
}

void countdown(int from) {
  debug("\n");
  while (from > 0) {
    debug(String(from--));
    debug(" ");
    delay(1000);
  }
  debug("\n");
}

void resetWatchdog() { ag.watchdog.reset(); }

// Wifi Manager
void connectToWifi() {
  WiFiManager wifiManager;
  // WiFi.disconnect(); //to delete previous saved hotspot
  String HOTSPOT = "AG-" + String(getNormalizedMac());
  displayShowText("180s to connect", "to Wifi Hotspot", HOTSPOT);
  wifiManager.setTimeout(180);
  if (!wifiManager.autoConnect((const char *)HOTSPOT.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(6000);
  }
}

void debug(String msg) {
  if (DEBUG)
    Serial.print(msg);
}

void debug(int msg) {
  if (DEBUG)
    Serial.print(msg);
}

void debugln(String msg) {
  if (DEBUG)
    Serial.println(msg);
}

void debugln(int msg) {
  if (DEBUG)
    Serial.println(msg);
}

String getNormalizedMac() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toLowerCase();
  return mac;
}

void setRGBledCO2color(int co2Value) {
  if (co2Value >= 300 && co2Value < 800)
    setRGBledColor('g');
  if (co2Value >= 800 && co2Value < 1000)
    setRGBledColor('y');
  if (co2Value >= 1000 && co2Value < 1500)
    setRGBledColor('o');
  if (co2Value >= 1500 && co2Value < 2000)
    setRGBledColor('r');
  if (co2Value >= 2000 && co2Value < 3000)
    setRGBledColor('p');
  if (co2Value >= 3000 && co2Value < 10000)
    setRGBledColor('z');
}

void setRGBledColor(char color) {
  if (useRGBledBar) {
    switch (color) {
    case 'g':
      ag.ledBar.setColor(0, 255, 0);
      break;
    case 'y':
      ag.ledBar.setColor(255, 255, 0);
      break;
    case 'o':
      ag.ledBar.setColor(255, 128, 0);
      break;
    case 'r':
      ag.ledBar.setColor(255, 0, 0);
      break;
    case 'b':
      ag.ledBar.setColor(0, 0, 255);
      break;
    case 'w':
      ag.ledBar.setColor(255, 255, 255);
      break;
    case 'p':
      ag.ledBar.setColor(153, 0, 153);
      break;
    case 'z':
      ag.ledBar.setColor(102, 0, 0);
      break;
    case 'n':
      ag.ledBar.setColor(0, 0, 0);
      break;
    default:
      break;
    }
  }
}

void boardInit(void) {
  /** Init LED Bar */
  ag.ledBar.begin();

  /** Button init */
  ag.button.begin();

  /** Init sensor SGP41 */
  if (ag.sgp41.begin(Wire) == false) {
    Serial.println("SGP41 begin failed");
    failedHandler();
  }

  /** INit SHT */
  if (ag.sht.begin(Wire) == false) {
    Serial.println("SHT begin failed");
    failedHandler();
  }

  /** Init watchdog */
  ag.watchdog.begin();

  /** Init S8 CO2 sensor */
  if (ag.s8.begin(Serial1) == false) {
    Serial.println("S8 begin failed");
    failedHandler();
  }

  /** Init PMS5003 */
  if (ag.pms5003.begin(Serial0) == false) {
    Serial.println("PMS5003 begin failed");
    failedHandler();
  }
}

void failedHandler(void) {
  while (true) {
    Serial.println("failed");
    vTaskDelay(1000);
  }
}
