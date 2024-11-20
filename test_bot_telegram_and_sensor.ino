#include <ESP8266WiFi.h>           // ไลบรารีสำหรับการเชื่อมต่อ WiFi บนบอร์ด ESP8266
#include <WiFiClientSecure.h>      // ไลบรารีสำหรับการเชื่อมต่อ HTTPS
#include <UniversalTelegramBot.h> // ไลบรารีสำหรับ Telegram Bot

// **WiFi Credentials**
const char* ssid = "Mam_2.4G";        // ใส่ชื่อ WiFi ของคุณ
const char* password = "0816193125"; // ใส่รหัสผ่าน WiFi ของคุณ

// **Telegram Bot Token และ Chat ID**
const char* botToken = "7702438986:AAEeokB03nKz0Y9s7Vs4VWi-U7pzHHVO8v8"; // Bot Token จาก BotFather
const char* chatId = "6928484464";    // Chat ID ของคุณ (ใช้เพื่อส่งข้อความ)

// **สร้าง Client สำหรับ WiFi และ Telegram Bot**
WiFiClientSecure client;              // ใช้ client แบบ HTTPS เพื่อเชื่อมต่อกับ Telegram
UniversalTelegramBot bot(botToken, client); // สร้าง object Bot โดยใช้ Token และ client

// **พินสำหรับเซ็นเซอร์และ LED**
int sensorPin = A0;                   // พินเซ็นเซอร์ที่เชื่อมต่อกับ A0 (Analog)
int ledPin = D2;                      // พิน LED ที่เชื่อมต่อกับ D2
int threshold = 10;                   // ค่าเกณฑ์ (Threshold) สำหรับการตรวจจับวัตถุ
static unsigned long lastSendTime = 0; // ตัวแปรสำหรับเก็บเวลาล่าสุดที่ส่งข้อความ

void setup() {
  Serial.begin(115200);               // เริ่ม Serial Monitor สำหรับ Debug
  pinMode(ledPin, OUTPUT);            // ตั้งค่า LED เป็น Output
  
  // **เชื่อมต่อ WiFi**
  Serial.print("Connecting to WiFi"); 
  WiFi.begin(ssid, password);         // เริ่มการเชื่อมต่อ WiFi
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่าจะเชื่อมต่อ
    delay(1000);                      // หน่วงเวลา 1 วินาที
    Serial.print(".");                // แสดงสถานะการเชื่อมต่อ
  }
  Serial.println("\nWiFi connected!"); // แจ้งว่าเชื่อมต่อสำเร็จ

  // **กำหนดค่า Client ให้รองรับ HTTPS**
  client.setInsecure();               // ข้ามการตรวจสอบ SSL Certificate
}

void loop() {
  int distance = analogRead(sensorPin); // อ่านค่าจากเซ็นเซอร์ (Analog Value)
  Serial.print("Distance Value: ");    // แสดงค่าใน Serial Monitor
  Serial.println(distance);

  unsigned long currentTime = millis(); // เก็บเวลาปัจจุบัน

  // **ตรวจจับวัตถุและส่งข้อความ**
  if (distance < threshold && currentTime - lastSendTime > 5000) { // หากค่าต่ำกว่าเกณฑ์และห่างจากการส่งครั้งก่อน 5 วินาที
    digitalWrite(ledPin, HIGH);       // เปิดไฟ LED
    String message = "Object detected! Distance: " + String(distance); // สร้างข้อความสำหรับ Telegram
    bot.sendMessage(chatId, message, "Markdown"); // ส่งข้อความไปยัง Telegram
    lastSendTime = currentTime;       // อัปเดตเวลาส่งข้อความล่าสุด
    Serial.println("light on");       // แจ้งสถานะใน Serial Monitor
  } else {
    digitalWrite(ledPin, LOW);        // ปิดไฟ LED เมื่อไม่มีวัตถุ
  }
  
  delay(100);                         // หน่วงเวลา 100 ms เพื่อป้องกันการอ่านค่าถี่เกินไป
}

