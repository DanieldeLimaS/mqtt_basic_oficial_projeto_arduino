#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MSG_BUFFER_SIZE 50

IPAddress mqtt_server(192, 100, 247, 3);
WiFiClient espClient;
// 35.157.18.117

PubSubClient client(espClient);

// o roxo é no vu
// o marrom é no 3v


int SensorInternoCm = 0;
const int SensorInternoTrigPin = 13;
const int SensorInternoEchoPin = 15;

int SensorExternoCm = 0;
const int SensorExternoTrigPin = 14;
const int SensorExternoEchoPin = 12;

Servo servo;


const int DistanciaAberturaCm = 10;
const int distMaxToSentMessage = 20;
const int distMinToSentMessage = 10;

const char *xmqtt_broker = "broker.hivemq.com";
const int mqttPort = 11883;
const char * ssid = "ssid_wifi";
const char * password = "senha_wifi";
char msg[MSG_BUFFER_SIZE];
const char * topic = topic;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentativa MQTT para conexão...");
    if (client.connect("b547a30b-5110-4f2a-8d01-ec137257ffc6","mqttserverunisbilly","billy$#@!4321")) {
      Serial.println(" Conectado");
      // client.publish("grupo04", "teste");
      // client.subscribe(topic);
    } else {
      Serial.print(" Falha na Conexão, rc=");
      Serial.print(client.state());
      Serial.println(" tente novamente em 1 segundos");
      delay(1000);
    }
  }
}

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}


void FechaLixeira() {
  //delay(1000);
    //servo.write(105);
     for(int i=80;i<=105;i++){
      servo.write(i);
    }
}

void AbreLixeira() {
  //delay(500);
    //servo.write(80);
    for(int i=105;i>=80;i--){
      servo.write(i);
    }
    
}

void setup() {
  Serial.begin(9600);
    servo.attach(04);
    servo.write(105);
    
  setup_wifi();
  
  client.setServer(xmqtt_broker, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
//SENSOR INTERNO
  //SensorInternoCm = 0.01723 * readUltrasonicDistance(SensorInternoTrigPin, SensorInternoEchoPin);
  //Serial.print("sensor Interno cm: ");
  //Serial.println(SensorInternoCm);

  //SENSOR EXTERNO
  SensorExternoCm = 0.01723 * readUltrasonicDistance(SensorExternoTrigPin, SensorExternoEchoPin);
  Serial.print("Sensor Externo cm: ");
  Serial.println(SensorExternoCm);

  //VALIDAÇAO PARA ABRIR LIXEIRA
  if(servo.read()==105){//significa que está fechada
      if (SensorExternoCm <= 15 ) {
        Serial.println("Abrindo Lixeira!");
        AbreLixeira();
      }
  }
  else if(servo.read()==80){// a lixeira está aberta
    if(SensorExternoCm >= 20 ){
      delay(2000);
      Serial.println("Fechando Lixeira!");
    FechaLixeira();
  }
    
  }
  
  if (SensorInternoCm <= 10 || SensorInternoCm >= 20) {
    // client.subscribe("grupo04/estadoLixeira")
    client.publish("grupo04/estadoLixeira", "A Lixeira está Cheia!");
    Serial.println("CHEIA!");
  } else {
    client.publish("grupo04/estadoLixeira", "Lixeira Vazia!");
    Serial.println("VAZIA !");
  }

  delay(50);

}
