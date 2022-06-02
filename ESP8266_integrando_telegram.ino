#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char * ssid = "AULA_IOT";
const char * password = "unismg123!";

#define BOTtoken "5575062680:AAF08QlTlxa1aPhYbcbp7ddkiUxg8gkjiHQ" // Bot API (Botfather)
#define CHAT_ID "-759218161" // Chat ID (IDBOT)

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//const int BUILTIN_LED = D1;
bool releEstado = LOW;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Usuário não autorizado", "");
      Serial.println(chat_id);
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Bem Vindo ao GRUPO04, " + from_name + ".\n";
      welcome += "Use os comandos a seguir para controlar suas saídas.\n\n";
      welcome += "/Ligar para ligar o led \n";
      welcome += "/Desligar para desligar o led \n";
      welcome += "/Status para ver o status do led \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/Ligar") {
      bot.sendMessage(chat_id, "O led foi ligado", "");
      releEstado = LOW;
      //digitalWrite(BUILTIN_LED, releEstado);
      digitalWrite(BUILTIN_LED, releEstado);
    }

    if (text == "/Desligar") {
      bot.sendMessage(chat_id, "O led foi desligado", "");
      releEstado = HIGH;
      //digitalWrite(BUILTIN_LED, releEstado);
      digitalWrite(BUILTIN_LED, releEstado);
    }

    if (text == "/Status") {
      if (digitalRead(BUILTIN_LED)) {
        bot.sendMessage(chat_id, "Status do led é Desligado", "");
      } else {
        bot.sendMessage(chat_id, "Status do led é Ligado", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  client.setTrustAnchors( & cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, releEstado);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  };
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("obteve resposta");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
