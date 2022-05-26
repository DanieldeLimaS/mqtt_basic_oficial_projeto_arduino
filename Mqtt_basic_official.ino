#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "DHT.h"

//Conexões
byte mac[]    = { 0xEE, 0xED, 0xBA, 0xFE, 0xFE, 0xAA };
IPAddress ip(172, 19, 0, 52);
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

const int pinData = 7;

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
    if (client.connect("grupo04","alunos","$3nh@321!")) {
      Serial.println(" Conectado");
      // Once connected, publish an announcement...
      // client.publish("grupo04","teste");
      // ... and resubscribe
      // client.subscribe("grupo04/rele");
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
  Serial.begin(9600);

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

  // client.publish("grupo04/rele", "Funcionando");
  client.subscribe("grupo04/rele");
  
  delay(3600);

}