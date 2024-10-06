#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>
#include <ESP_I2S.h>
#include <SFX.h>

// WiFi credentials
const char *ssid = "Juan Carlos";
const char *password = "huerfadrino";

// Audio and I2S parameters
const int frequency = 440;   // Frequency of square wave in Hz
const int amplitude = 500;   // Amplitude of square wave
const int sampleRate = 8000; // Sample rate in Hz
const int halfWavelength = (sampleRate / frequency);

i2s_data_bit_width_t bps = I2S_DATA_BIT_WIDTH_16BIT;
i2s_mode_t mode = I2S_MODE_STD;
i2s_slot_mode_t slot = I2S_SLOT_MODE_STEREO;

int32_t sample = amplitude; // Current sample value

// Button pin definitions
const int bopItPin = 2;
const int pullItPin = 0;
const int twistItPin = 5;

I2SClass i2s;

void setupWiFi()
{
  Serial.println("Booting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("WiFi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupOTA()
{
  ArduinoOTA.onStart([]()
                     {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("Start updating " + type); });

  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });

  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    switch (error) {
      case OTA_AUTH_ERROR: Serial.println("Auth Failed"); break;
      case OTA_BEGIN_ERROR: Serial.println("Begin Failed"); break;
      case OTA_CONNECT_ERROR: Serial.println("Connect Failed"); break;
      case OTA_RECEIVE_ERROR: Serial.println("Receive Failed"); break;
      case OTA_END_ERROR: Serial.println("End Failed"); break;
    } });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void setupI2S()
{
  Serial.println("Initializing I2S...");
  i2s.setPins(21, 20, 9);

  if (!i2s.begin(mode, sampleRate, bps, slot))
  {
    Serial.println("Failed to initialize I2S!");
    while (true)
      ; // Stop if I2S setup fails
  }

  Serial.println("Playing initial sound...");
  i2s.playWAV((uint8_t *)bopIt_voice_wav_data, bopIt_voice_wav_data_len); // Play initial sound
}

void setupButtons()
{
  pinMode(bopItPin, INPUT_PULLUP);
  pinMode(pullItPin, INPUT_PULLUP);
  pinMode(twistItPin, INPUT_PULLUP);
}

void setup()
{
  Serial.begin(115200);
  setupWiFi();
  setupOTA();
  setupI2S();
  setupButtons();

  // Create a separate task for handling OTA
  xTaskCreate(
      otaTask,    // Task function
      "OTA Task", // Name of the task
      10000,      // Stack size (bytes)
      NULL,       // Task input parameter
      1,          // Priority
      NULL);      // Task handle
}

void otaTask(void *pvParameters)
{
  for (;;)
  {
    ArduinoOTA.handle(); // Handle OTA
    vTaskDelay(100);     // Delay for a bit
  }
}

void checkButtons()
{
  // Read the button states (LOW means pressed due to INPUT_PULLUP)
  if (digitalRead(bopItPin) == LOW)
  {
    i2s.playWAV((uint8_t *)bopIt_wav_data, bopIt_wav_data_len);
  }

  if (digitalRead(pullItPin) == LOW)
  {
    i2s.playWAV((uint8_t *)pullIt_wav_data, pullIt_wav_data_len);
  }

  if (digitalRead(twistItPin) == LOW)
  {
    i2s.playWAV((uint8_t *)twistIt_wav_data, twistIt_wav_data_len);
  }
}

SFX get_random_voice()
{
  SFX voice = (SFX)(random(3) + BOP_IT_VOICE);
  return voice;
}

void play_sfx(SFX sfx)
{
  switch (sfx)
  {
  case BOP_IT:
    i2s.playWAV((uint8_t *)bopIt_wav_data, bopIt_wav_data_len);
    break;
  case PULL_IT:
    i2s.playWAV((uint8_t *)pullIt_wav_data, pullIt_wav_data_len);
    break;
  case TWIST_IT:
    i2s.playWAV((uint8_t *)twistIt_wav_data, twistIt_wav_data_len);
    break;
  case BOP_IT_VOICE:
    i2s.playWAV((uint8_t *)bopIt_voice_wav_data, bopIt_voice_wav_data_len);
    break;
  case PULL_IT_VOICE:
    i2s.playWAV((uint8_t *)pullIt_voice_wav_data, pullIt_voice_wav_data_len);
    break;
  case TWIST_IT_VOICE:
    i2s.playWAV((uint8_t *)twistIt_voice_wav_data, twistIt_voice_wav_data_len);
    break;
  }
}

void wait_until_button_for_voice_is_pressed(int buttonPin)
{
  while (digitalRead(buttonPin) == HIGH)
  {
    delay(1);
  }
}

int get_button_for_voice(SFX voice)
{
  switch (voice)
  {
  case BOP_IT:
    return bopItPin;
  case PULL_IT:
    return pullItPin;
  case TWIST_IT:
    return twistItPin;
  case BOP_IT_VOICE:
    return bopItPin;
  case PULL_IT_VOICE:
    return pullItPin;
  case TWIST_IT_VOICE:
    return twistItPin;
  }
}

sfx get_sfx_for_button(int buttonPin)
{
  switch (buttonPin)
  {
  case bopItPin:
    return BOP_IT;
  case pullItPin:
    return PULL_IT;
  case twistItPin:
    return TWIST_IT;
  }
}

void loop()
{
  // checkButtons(); // Check button states in the main loop

  SFX voice = get_random_voice();
  play_sfx(voice);

  int button = get_button_for_voice(voice);
  wait_until_button_for_voice_is_pressed(button);
  play_sfx(get_sfx_for_button(button));
}
