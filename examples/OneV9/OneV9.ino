#include <EEPROM.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <WiFiManager.h>

#include <AirGradient.h>
#include <U8g2lib.h>

#define DEBUG true
#define FW_VER      "1.0.0"

AirGradient ag(BOARD_ONE_INDOOR_MONITOR_V9_0);

HTTPClient client;

// time in seconds needed for NOx conditioning
uint16_t conditioning_s = 10;

// for peristent saving and loading
int addr = 4;
byte value;

// Display bottom right

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

void failedHandle(void);
void sensorInit(void);
void dispBoot(void);

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /** Init I2C */
  const int I2C_SDA = ag.getI2cSdaPin();
  const int I2C_SCL = ag.getI2cSclPin();
  Wire.begin(I2C_SDA, I2C_SCL);

  sensorInit();

  /** Init EEPROM */
  EEPROM.begin(512);
  delay(500);

  /** Init button */
  ag.button.begin();

  /** Show config */
  buttonConfig = String(EEPROM.read(addr)).toInt();
  if (buttonConfig > 3) {
    buttonConfig = 0;
  }

  delay(400);
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
  uint16_t error = 0;
  char errorMessage[256];
  uint16_t defaultRh = 0x8000;
  uint16_t defaultT = 0x6666;
  uint16_t srawVoc = 0;
  uint16_t srawNox = 0;
  uint16_t defaultCompenstaionRh = 0x8000; // in ticks as defined by SGP41
  uint16_t defaultCompenstaionT = 0x6666;  // in ticks as defined by SGP41
  uint16_t compensationRh = 0;             // in ticks as defined by SGP41
  uint16_t compensationT = 0;              // in ticks as defined by SGP41

  delay(1000);

  if (currentMillis - previousTVOC >= tvocInterval) {
    previousTVOC += tvocInterval;
    if (error) {
      TVOC = -1;
      NOX = -1;
      Serial.println(String(TVOC));
    } else {
      TVOC = ag.sgp41.getTvocIndex();
      NOX = ag.sgp41.getNoxIndex();
      Serial.println(String(TVOC));
    }
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
      ln1 = "AQI:" + String(PM_TO_AQI_US(pm25)) + " CO2:" + String(Co2);
    } else {
      ln1 = "PM:" + String(pm25) + " CO2:" + String(Co2);
    }

    String ln2 = "TVOC:" + String(TVOC) + " NOX:" + String(NOX);

    if (inF) {
      ln3 = "F:" + String((temp * 9 / 5) + 32) + " H:" + String(hum) + "%";
    } else {
      ln3 = "C:" + String(temp) + " H:" + String(hum) + "%";
    }
    updateOLED3();
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
    ag.display.setRotation(0);
    inF = false;
    inUSAQI = false;
  }
  if (buttonConfig == 1) {
    displayShowText("Temp. in C", "PM in US AQI", "Long Press Saves");
    ag.display.setRotation(0);
    inF = false;
    inUSAQI = true;
  } else if (buttonConfig == 2) {
    displayShowText("Temp. in F", "PM in ug/m3", "Long Press Saves");
    ag.display.setRotation(0);
    inF = true;
    inUSAQI = false;
  } else if (buttonConfig == 3) {
    displayShowText("Temp. in F", "PM in US AQI", "Long Press Saves");
    ag.display.setRotation(0);
    inF = true;
    inUSAQI = true;
  }
}

void sendPing() {
  String payload =
      "{\"wifi\":" + String(WiFi.RSSI()) + ", \"boot\":" + loopCount + "}";
}

void displayShowText(String ln1, String ln2, String ln3) {
  ag.display.clear();
  ag.display.setCursor(1, 10);
  ag.display.setText(ln1);
  ag.display.setCursor(1, 30);
  ag.display.setText(ln2);
  ag.display.setCursor(1, 50);
  ag.display.setText(ln3);
  ag.display.show();
}

void updateOLED3() {
  char buf[9];
  ag.display.clear();

  if (inF) {
    if (temp > -10001) {
      float tempF = (temp * 9 / 5) + 32;
      sprintf(buf, "%.1f*F", tempF);
    } else {
      sprintf(buf, "-°F");
    }
  } else {
    if (temp > -10001) {
      sprintf(buf, "%.1f*C", temp);
    } else {
      sprintf(buf, "-*C");
    }
  }
  ag.display.setCursor(1, 10 - 9);
  ag.display.setText(buf);
  // ag.display.show();

  // Show hummidity
  if (hum >= 0) {
    sprintf(buf, "%d%%", hum);
  } else {
    sprintf(buf, " -%%");
  }
  if (hum > 99) {
    ag.display.setCursor(97, 10 - 9);
  } else {
    ag.display.setCursor(105, 10 - 9);
  }
  ag.display.setText(buf);


  ag.display.drawLine(1, 13, 128, 13, 1);

  /** CO2 label */
  ag.display.setCursor(1, 27 - 9);
  ag.display.setText("CO2");

  /** CO2 value */
  if (Co2 > 0) {
    sprintf(buf, "%d", Co2);
  } else {
    sprintf(buf, "%s", "-");
  }
  ag.display.setFont(Display::FontFreeMono9Bold);
  ag.display.setCursor(1, 48 - 9);
  ag.display.setText(buf);
  ag.display.setFont(Display::FontDefault);

  ag.display.setCursor(1, 61 - 9);
  ag.display.setText("ppm");
  // ag.display.show();

  ag.display.drawLine(45, 15, 45, 64, 1);
  // ag.display.show();
  ag.display.setCursor(48, 27 - 9);
  ag.display.setText("PM2.5");
  // ag.display.show();

  if (inUSAQI) {
    if (pm25 >= 0) {
      sprintf(buf, "%d", PM_TO_AQI_US(pm25));
    } else {
      sprintf(buf, "%s", "-");
    }
    ag.display.setCursor(48, 48 - 9);
    ag.display.setText(buf);
    // ag.display.show();
    ag.display.setCursor(48, 61 - 9);
    ag.display.setText("AQI");
  } else {
    if (pm25 >= 0) {
      sprintf(buf, "%d", pm25);
    } else {
      sprintf(buf, "%s", "-");
    }
    ag.display.setCursor(48, 48 - 9);
    ag.display.setText(buf);
    ag.display.setCursor(48, 61 - 9);
    ag.display.setText("ug/m3");
  }

  ag.display.drawLine(82, 15, 82, 64, 1);

  ag.display.setCursor(85, 27 - 9);
  ag.display.setText("TVOC:");

  if (TVOC >= 0) {
    sprintf(buf, "%d", TVOC);
  } else {
    sprintf(buf, "%s", "-");
  }
  ag.display.setCursor(85, 39 - 9);
  ag.display.setText(buf);

  ag.display.setCursor(85, 53 - 9);
  ag.display.setText("NOx:");

  if (NOX >= 0) {
    sprintf(buf, "%d", NOX);
  } else {
    sprintf(buf, "%s", "-");
  }
  ag.display.setCursor(85, 63 - 9);
  ag.display.setText(buf);
  ag.display.show();
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

void resetWatchdog() {
  Serial.println("Watchdog reset");
  ag.watchdog.reset();
}

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
  if (DEBUG) {
    Serial.print(msg);
  }
}

void debug(int msg) {
  if (DEBUG) {
    Serial.print(msg);
  }
}

void debugln(String msg) {
  if (DEBUG) {
    Serial.println(msg);
  }
}

void debugln(int msg) {
  if (DEBUG) {
    Serial.println(msg);
  }
}

String getNormalizedMac() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toLowerCase();
  return mac;
}

void setRGBledCO2color(int co2Value) {
  if (co2Value >= 300 && co2Value < 800) {
    setRGBledColor('g');
  }
  if (co2Value >= 800 && co2Value < 1000) {
    setRGBledColor('y');
  }
  if (co2Value >= 1000 && co2Value < 1500) {
    setRGBledColor('o');
  }
  if (co2Value >= 1500 && co2Value < 2000) {
    setRGBledColor('r');
  }
  if (co2Value >= 2000 && co2Value < 3000) {
    setRGBledColor('p');
  }
  if (co2Value >= 3000 && co2Value < 10000) {
    setRGBledColor('z');
  }
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

void failedHandle(void) {
  while (1) {
    Serial.println("failed");
    delay(1000);
  }
}

void sensorInit(void) {
  /** Init LED Bar */
  ag.ledBar.begin();

  /** Init Display */
  ag.display.begin(Wire);
  ag.display.setTextColor(1);

  /** Show display warning */
  dispBoot();
  delay(2000);
  displayShowText("Warming Up", "Serial Number:", String(getNormalizedMac()));

  /** Init sensor SGP41 */
  if (ag.sgp41.begin(Wire) == false) {
    Serial.println("SGP41 begin failed");
    failedHandle();
  }
  delay(300);

  /** INit SHT */
  if (ag.sht.begin(Wire) == false) {
    Serial.println("SHT begin failed");
    failedHandle();
  }
  delay(300);

  /** Init watchdog */
  ag.watchdog.begin();

  /** Init S8 CO2 sensor */
  if (ag.s8.begin(Serial1) == false) {
    Serial.println("S8 begin failed");
    failedHandle();
  }

  /** Init PMS5003 */
  if (ag.pms5003.begin(Serial0) == false) {
    Serial.println("PMS5003 begin failed");
    failedHandle();
  }
}

void dispBoot(void)
{
  displayShowText("One V9", "Fw Ver: " + String(FW_VER), "Lib Ver: " + ag.getVersion());
}
