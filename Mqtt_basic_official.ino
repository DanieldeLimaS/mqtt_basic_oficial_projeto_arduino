#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "DHT.h"

//Conexões
byte mac[]    = {  0xEE, 0xED, 0xBA, 0xFE, 0xFE, 0xCC };
IPAddress ip(172, 19, 0, 51);
IPAddress myDns(8, 8, 8, 8);
IPAddress gateway(172, 19, 0, 1);
IPAddress subnet(255, 255, 0, 0);

IPAddress server(18, 159, 219, 87);

// Sensor de temperatura
#define DHTPIN A1// pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// Sensor de Luz
#define sensor A0
int sensorValue;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentativa MQTT para conexão...");
    // Attempt to connect
    if (client.connect("grupo03")) {
      Serial.println(" Conectado");
      // Once connected, publish an announcement...

      // ... and resubscribe
      // client.subscribe("inTopic");
    } else {
      Serial.print(" Falha na Conexão, rc=");
      Serial.print(client.state());
      Serial.println(" tente novamente em 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(57600);

  client.setServer(server, 1883);
  client.setCallback(callback);

  dht.begin();

  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // temperatura

  float h = dht.readHumidity();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  float t = dht.readTemperature();
  String temp = String (t);
  String umi = String (h);
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Falha na leitura DHT -> Sem Temperatura");
    delay(1000);
    return;
  }
  Serial.print(F("Umidade: "));
  Serial.print(h);
  Serial.print(" || Temperatura: ");
  Serial.print(t);
  Serial.println(" *C");

  //Luz
  sensorValue = analogRead(A0);
  Serial.print("Valor do Sensor = ");
  Serial.println(sensorValue);

  client.publish("grupo03/temp", temp.c_str());
  client.publish("grupo03/umi", umi.c_str());
  delay(3600);

}
