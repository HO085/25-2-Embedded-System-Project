#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define ALERT_PIN 4   // ATmega128에서 들어오는 경보 신호 입력 핀

// Wi-Fi 정보 (실제 값으로 바꿔서 사용)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram Bot 정보 (실제 값으로 바꿔서 사용, 깃허브에 올리지 말 것)
#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

bool lastAlert = false;   // 중복 알림을 막기 위한 상태 플래그

void setup() {
  pinMode(ALERT_PIN, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {   // Wi-Fi 연결 대기
    delay(500);
  }
  client.setInsecure();
}

void loop() {
  bool alert = digitalRead(ALERT_PIN);   // ATmega의 경보 신호 읽기

  if (alert && !lastAlert) {             // 경보가 켜지는 순간에만 한 번 전송
    bot.sendMessage(CHAT_ID, "경고 가스가 감지되었습니다", "");
    lastAlert = true;
  }
  else if (!alert) {                     // 경보가 꺼지면 상태 리셋
    lastAlert = false;
  }

  delay(500);
}
