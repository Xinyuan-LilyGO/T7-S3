#define BAT_ADC        2
#define LED_PIN        17
#define BUTTON_PIN     0
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20        /* Time ESP32 will go to sleep (in seconds) */

#include <WiFi.h>
#include "esp_adc_cal.h"
#include <Arduino.h>
float Voltage = 0.0;
uint32_t readADC_Cal(int ADC_Raw);
uint32_t        blinkMillis = 0;
RTC_DATA_ATTR int bootCount = 0;

bool readkey()
{

    if (digitalRead(BUTTON_PIN) == 0) {
        delay(100);
        //Wait for release
        while (digitalRead(BUTTON_PIN) == 0);
        return true;
    }
    return false;

}

void setup()
{
    Serial.begin(115200);
    delay(10);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);


    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");

}

void loop()
{

    if (millis() - blinkMillis > 1000) {
        blinkMillis = millis();
        Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
        Serial1.printf("%.2fV", Voltage / 1000.0); // Print Voltage (in V)
        Serial1.println();
    }

    if (readkey()) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(200);
    }

}

uint32_t readADC_Cal(int ADC_Raw)
{
    esp_adc_cal_characteristics_t adc_chars;

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}



