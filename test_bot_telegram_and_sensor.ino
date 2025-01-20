#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ข้อมูล WiFi
const char* WIFI_SSID = "Owxn"; // SSID ของ Wi-Fi
const char* WIFI_PASSWORD = "Owxn2409"; // รหัสผ่านของ Wi-Fi

// ข้อมูลของ Telegram Bot Token และ Chat ID
const String BOT_TOKEN = "8175471471:AAG3IpS62xQb_2pR-ZwfZnH_aVMy5ekjukw"; // Token ของ Bot ที่สร้างจาก BotFather
const String CHAT_ID = "6928484464"; // Chat ID ของคุณที่ต้องการให้ส่งข้อความไป

// กำหนดพินของเซ็นเซอร์ IR (พินที่ใช้ตรวจจับเอกสาร)
const int sensorIR = D2; // กำหนดพิน D1 สำหรับเซ็นเซอร์ IR
String message1 = "เอกสารใหม่ถูกตรวจจับ!"; // ข้อความที่จะแจ้งเตือนเมื่อมีการตรวจพบเอกสารใหม่
bool beep_state = false; // ตัวแปรตรวจสอบสถานะการส่งข้อความแจ้งเตือน
uint32_t ts, ts1; // ตัวแปรเวลาใช้ในการตั้งเวลาส่งข้อความ

// สร้างอ็อบเจ็กต์ของ Telegram Bot สำหรับการส่งข้อความ
WiFiClientSecure client; // ใช้เชื่อมต่อกับเซิร์ฟเวอร์ Telegram ผ่าน HTTPS
UniversalTelegramBot bot(BOT_TOKEN, client); // สร้างอ็อบเจ็กต์ของ Bot ด้วย Token ที่ได้จากการสร้าง Bot

void setup() {
  Serial.begin(115200); // เริ่มต้น Serial Monitor เพื่อแสดงข้อมูลการทำงาน
  pinMode(sensorIR, INPUT); // ตั้งค่าเซ็นเซอร์ IR เป็น Input
  pinMode(LED_BUILTIN, OUTPUT); // ตั้งค่า LED_BUILTIN (LED บนบอร์ด) เป็น Output
  digitalWrite(LED_BUILTIN, HIGH); // เปิดไฟ LED ให้แสดงสถานะ

  // เชื่อมต่อกับ WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA); // ตั้งโหมด Wi-Fi เป็น Station (เชื่อมต่อกับ Wi-Fi Router)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // เริ่มเชื่อมต่อกับ Wi-Fi
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่า Wi-Fi จะเชื่อมต่อ
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!"); // แสดงข้อความเมื่อเชื่อมต่อ Wi-Fi สำเร็จ
  Serial.println("IR Sensor Ready!"); // แสดงข้อความเมื่อพร้อมใช้งานเซ็นเซอร์ IR

  ts = ts1 = millis(); // กำหนดเวลาเริ่มต้นเพื่อใช้งานในเงื่อนไขต่างๆ
}

void loop() {
  ts = millis(); // บันทึกเวลาในตัวแปร ts

  // ตรวจสอบสถานะการเชื่อมต่อ Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW); // ถ้าเชื่อมต่อ Wi-Fi สำเร็จให้ปิดไฟ LED
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // ถ้าไม่เชื่อมต่อ Wi-Fi ให้เปิดไฟ LED
  }

  // รีเซ็ต beep_state ทุก 5 วินาที (เมื่อ 5 วินาทีผ่านไป)
  if ((ts - ts1 >= 5000) && (beep_state == true)) {
    beep_state = false; // รีเซ็ตสถานะ beep_state
  }

  // ตรวจจับเซ็นเซอร์ IR เมื่อมีการวางเอกสาร
  if ((digitalRead(sensorIR) == LOW) && (beep_state == false)) {
    Serial.println("ตรวจพบเอกสารใหม่!"); // แสดงข้อความใน Serial Monitor เมื่อมีเอกสารใหม่
    sendTelegramMessage(message1); // เรียกฟังก์ชันส่งข้อความไปยัง Telegram
    beep_state = true; // ตั้งสถานะ beep_state เป็น true เพื่อไม่ให้ส่งข้อความซ้ำ
    ts1 = ts; // รีเซ็ตเวลา
  }

  delay(10); // หน่วงเวลาเล็กน้อยเพื่อไม่ให้ทำงานหนักเกินไป
}

void sendTelegramMessage(String message) {
  // ตรวจสอบการเชื่อมต่อ Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connecting to Telegram...");
    if (bot.sendMessage(CHAT_ID, message, "Markdown")) { // ใช้ Markdown format สำหรับข้อความ
      Serial.println("ส่งข้อความสำเร็จ!"); // แสดงข้อความใน Serial Monitor เมื่อส่งสำเร็จ
    } else {
      Serial.println("ส่งข้อความไม่สำเร็จ!"); // แสดงข้อความเมื่อการส่งไม่สำเร็จ
      Serial.println("ตรวจสอบบอทหรือการตั้งค่า Telegram Token และ Chat ID");
    }
  } else {
    Serial.println("ไม่ได้เชื่อมต่อ Wi-Fi");
  }
}

