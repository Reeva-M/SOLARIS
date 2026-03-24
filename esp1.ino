#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ================= WiFi =================
#define WIFI_SSID "Skyworth_3630A0"
#define WIFI_PASS "00000000"

// ================= Flask Server =================
const char* UPDATE_URL = "http://192.168.1.10:5000/update";
const char* SOLAR_URL  = "http://192.168.1.10:5000/solar";

// ================= Sensor Pins =================
#define WATER_LEVEL_PIN1 34
#define WATER_LEVEL_PIN2 33

#define DHT_PIN 21
#define DHT_TYPE DHT11

#define ONE_WIRE_BUS 15   // DS18B20 DATA PIN

// ================= Actuators =================
#define PUMP_PIN 26
#define SERVO_AZ_PIN 18
#define SERVO_EL_PIN 19

// ================= Objects =================
Servo servoAz;
Servo servoEl;
DHT dht(DHT_PIN, DHT_TYPE);

// DS18B20 objects
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTempSensor(&oneWire);

// ================= System State =================
int pumpState = 0;
int servoAzAngle = 90;
int servoElAngle = 45;

unsigned long lastPumpRun = 0;
const unsigned long pumpCooldown = 7200000; // 2 hours in milliseconds


// ================= Setup =================
void setup() {
  Serial.begin(115200);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, HIGH);

  servoAz.attach(SERVO_AZ_PIN);
  servoEl.attach(SERVO_EL_PIN);
  servoAz.write(servoAzAngle);
  servoEl.write(servoElAngle);

  dht.begin();
  waterTempSensor.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected");
}

// ================= Main Loop =================
void loop() {

  // ---------- Water Level ----------
  float waterLevel1 = map(analogRead(WATER_LEVEL_PIN1), 0, 4095, 0, 100);
  float waterLevel2 = map(analogRead(WATER_LEVEL_PIN2), 0, 4095, 0, 100);
  float waterLevel  = (waterLevel1 + waterLevel2) / 2.0;

  // ---------- Air Sensors ----------
  float airTemp  = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(airTemp) || isnan(humidity)) {
    Serial.println("❌ DHT read failed");
    delay(2000);
    return;
  }

  // ---------- Water Temperature (DS18B20) ----------
  waterTempSensor.requestTemperatures();
  float waterTemp = waterTempSensor.getTempCByIndex(0);

  if (waterTemp == DEVICE_DISCONNECTED_C) {
    Serial.println("❌ Water Temp Sensor not detected");
    waterTemp = 0;
  } else {
    Serial.print("🌊 Water Temperature: ");
    Serial.print(waterTemp);
    Serial.println(" °C");
  }

  waterLevel = constrain(waterLevel, 0, 100);
  humidity   = constrain(humidity, 0, 100);

  // ---------- Send to Flask ----------
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(UPDATE_URL);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(256);
    doc["water_level"] = waterLevel;
    doc["temperature"] = airTemp;
    doc["humidity"] = humidity;
    doc["water_temperature"] = waterTemp;

    String payload;
    serializeJson(doc, payload);

    int code = http.POST(payload);

    if (code > 0) {
      String response = http.getString();
      Serial.println("🔁 Irrigation Response: " + response);

      DynamicJsonDocument resp(256);
      if (!deserializeJson(resp, response)) {

        pumpState = resp["pump_state"] | 0;
        int intervalMin = resp["interval"] | 1;

        //digitalWrite(PUMP_PIN, pumpState);

        // ---------- Pump Cooldown Logic ----------
        if (pumpState == 1) {

          if (millis() - lastPumpRun >= pumpCooldown) {

            digitalWrite(PUMP_PIN, LOW);   // Pump ON

            int intervalSec = constrain(intervalMin, 15, 45);
            Serial.print("⏱ Pump ON delay (seconds): ");
            Serial.println(intervalSec);

            delay(intervalSec * 1000);

            digitalWrite(PUMP_PIN, HIGH);  // Pump OFF

            lastPumpRun = millis();   // start cooldown timer

          } else {
            Serial.println("🚫 Pump blocked (2 hr cooldown)");
            digitalWrite(PUMP_PIN, HIGH);  // Pump OFF during cooldown
          }

        } else {
          digitalWrite(PUMP_PIN, HIGH);    // Pump OFF if server says OFF
        }

        // ---------- Solar Tracking ----------
        HTTPClient solarHttp;
        solarHttp.begin(SOLAR_URL);
        solarHttp.addHeader("Content-Type", "application/json");

        DynamicJsonDocument solarDoc(128);
        solarDoc["panel_az"] = servoAzAngle;
        solarDoc["panel_el"] = servoElAngle;

        String solarPayload;
        serializeJson(solarDoc, solarPayload);

        int solarCode = solarHttp.POST(solarPayload);

        if (solarCode > 0) {
          String solarResp = solarHttp.getString();
          Serial.println("☀ Solar Response: " + solarResp);

          DynamicJsonDocument solarJson(128);
          if (!deserializeJson(solarJson, solarResp)) {

            servoAzAngle = solarJson["azimuth"] | servoAzAngle;
            servoElAngle = solarJson["elevation"] | servoElAngle;

            servoAz.write(servoAzAngle);
            servoEl.write(servoElAngle);
          }
        }
        solarHttp.end();

        int intervalSec = constrain(intervalMin, 15, 45);
        Serial.print("⏱ Pump ON delay (seconds): ");
        Serial.println(intervalSec);

        delay(intervalSec * 1000);
        //delay(intervalMin * 60000);   // ⏱ original logic preserved
        return;
      }
    }
    http.end();
  }

  delay(5000);
}