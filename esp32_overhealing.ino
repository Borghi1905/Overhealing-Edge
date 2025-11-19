#include <WiFi.h>
#include <HTTPClient.h>
#include "DHTesp.h"

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// substitua pela sua Write API Key do ThingSpeak
String apiKey = "MDNI5AUVTZI059SK";

// URL base do ThingSpeak
const char* serverName = "https://api.thingspeak.com/update";

const int DHT_PIN = 15;
const int LDR_PIN = 34;
const int NOISE_PIN = 35;

DHTesp dht;

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.println(WiFi.localIP());
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();
  float temperature = data.temperature;
  float humidity = data.humidity;
  int lightRaw = analogRead(LDR_PIN);
  int noiseRaw = analogRead(NOISE_PIN);

  float light = map(lightRaw, 0, 4095, 0, 100);
  float noise = map(noiseRaw, 0, 4095, 0, 100);

  Serial.println("\n📡 Dados capturados:");
  Serial.print("Temperatura: "); Serial.println(temperature);
  Serial.print("Umidade: "); Serial.println(humidity);
  Serial.print("Luz: "); Serial.println(light);
  Serial.print("Ruído: "); Serial.println(noise);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverName) +
                 "?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(light) +
                 "&field4=" + String(noise);

    http.begin(url);
    int httpCode = http.GET();
    Serial.print("🔗 HTTP Response code: ");
    Serial.println(httpCode);
    http.end();
  }

  delay(20000); // ThingSpeak aceita 1 atualização a cada 15s
}
