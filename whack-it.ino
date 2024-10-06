#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>
#include <wav.h>
#include <bopItWav.h>
#include <twistItWav.h>
#include <pullItWav.h>

const char *ssid = "Juan Carlos";
const char *password = "huerfadrino";

#include <ESP_I2S.h>

const int frequency = 440;    // frequency of square wave in Hz
const int amplitude = 500;    // amplitude of square wave
const int sampleRate = 8000;  // sample rate in Hz

i2s_data_bit_width_t bps = I2S_DATA_BIT_WIDTH_16BIT;
i2s_mode_t mode = I2S_MODE_STD;
i2s_slot_mode_t slot = I2S_SLOT_MODE_STEREO;

const int halfWavelength = (sampleRate / frequency);  // half wavelength of square wave

int32_t sample = amplitude;  // current sample value
int count = 0;

const int bopItPin = 2;
const int pullItPin = 0;
const int twistItPin = 5;

I2SClass i2s;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

   pinMode(bopItPin, INPUT_PULLUP);
   pinMode(pullItPin, INPUT_PULLUP);
   pinMode(twistItPin, INPUT_PULLUP);
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");




  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.begin(115200);
  Serial.println("I2S simple tone");
  i2s.setPins(21,20,9);
  Serial.println("Paso 1");
  // start I2S at the sample rate with 16-bits per sample
  if (!i2s.begin(mode, sampleRate, bps, slot)) {
    Serial.println("Failed to initialize I2S!");
    while (1);  // do nothing
  }
  Serial.println("Paso 2");
  i2s.playWAV(wav_data, wav_data_len);
  Serial.println("Paso 3");
}

void loop() {
  ArduinoOTA.handle();

  // Read the button state (LOW means pressed due to the pull-up)


  int bopItButton = digitalRead(bopItPin);
  int pullItButton = digitalRead(pullItPin);
  int twistItButton = digitalRead(twistItPin);

  if (bopItButton == LOW)
    i2s.playWAV(bopIt_wav_data, bopIt_wav_data_len);

  if (pullItButton == LOW)
    i2s.playWAV(pullIt_wav_data, pullIt_wav_data_len);

  if (twistItButton == LOW)
    i2s.playWAV(twistIt_wav_data, twistIt_wav_data_len);

  Serial.println(bopItButton);

  delay(100); 

}
