#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define BTN_PIN 15   // Pin tombol (D8)
#define BUZZ_PIN 16  // pin tombol (D0)
#define SDA_PIN 2 // Pin SDA (D4)
#define RST_PIN 0 // Pin RST (D3)

LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C mungkin berbeda, tampilan 16x2
MFRC522 mfrc522(SDA_PIN, RST_PIN);

// SSID dan Password Jaringan WiFi
const char* ssid = "Fahri Akbar Putra";
const char* password = "bapakdiaduabelas";

// Alamat IP Server
const char* host = "172.20.10.4";

void clearAndDisplayInitialText() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEM ABSENSI");
  Serial.println("SISTEM ABSENSI");
  lcd.setCursor(0, 1);
  lcd.print("BERBASIS RFID");
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BTN_PIN, INPUT);   // Mengatur pin BTN_PIN sebagai input
  pinMode(BUZZ_PIN, OUTPUT); // Mengatur pin BUZZ_PIN sebagai output

  digitalWrite(BUZZ_PIN, HIGH); // Mematikan BUZZ_PIN

  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MENGHUBUNGKAN KE");
  lcd.setCursor(0, 1);
  lcd.print("WIFI");
  Serial.println("MENGHUBUNGKAN KE WIFI");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WIFI TERHUBUNG");
  Serial.println("WIFI TERHUBUNG");
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());
  Serial.print("IP:");
  Serial.println(WiFi.localIP());

  digitalWrite(BUZZ_PIN, LOW); // Mengaktifkan BUZZ_PIN
  delay(2000); // Menunggu 1 detik
  digitalWrite(BUZZ_PIN, HIGH); // Mematikan BUZZ_PIN

  clearAndDisplayInitialText();

}

void loop() {
  //MEMBUAT BTN_PIN DITEKAN MAKA BUZZ_PIN MENYALA

  if (digitalRead(BTN_PIN) == HIGH) {
   digitalWrite(BUZZ_PIN, LOW); // Mengaktifkan BUZZ_PIN   
  }
  else {
    digitalWrite(BUZZ_PIN, HIGH); // Mematikan BUZZ_PIN  
  }

  //JIKA BTN_PINDITEKAN MAKA MENGUBAH MODE ABSENSI DO WEBSITE

  if (digitalRead(BTN_PIN) == 1) {
    while(digitalRead(BTN_PIN) == 1);

    String getData, Link;
    WiFiClient client;
    HTTPClient http;

    Link = "http://172.20.10.4/Project%20Absensi/ubahmode.php";
    http.begin(client, Link);

    int httpCode = http.GET();
    String payload = http.getString();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESPOND WEBSITE:");
    lcd.setCursor(0, 1);
    lcd.print(payload);
    Serial.print("RESPOND WEBSITE:");
    Serial.println(payload);
    http.end();
    delay(1000);
    clearAndDisplayInitialText();

  }
  //PROSES SCAN ID PADA RFID DAN MENGUPLOAD KE DATABASE
  if (!mfrc522.PICC_IsNewCardPresent())
    return ;

  if (!mfrc522.PICC_ReadCardSerial())
    return ;
  
  String IDTAG = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    IDTAG += mfrc522.uid.uidByte[i];
  }

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
      Serial.println("Koneksi Gagal");
      return;
  }
  String Link;
  HTTPClient http;
  Link = "http://172.20.10.4/Project%20Absensi/kirimkartu.php?nomorkartu=" + IDTAG;
  http.begin(client, Link);

int httpCode = http.GET();
String Payload = http.getString();
http.end();

if (httpCode == 200) { // Jika permintaan HTTP berhasil (kode 200 OK)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SCAN RFID:");
  lcd.setCursor(0, 1);
  lcd.print(Payload);
  Serial.println("SCAN RFID");
  Serial.println(Payload);

  // Nyalakan buzzer di sini
  digitalWrite(BUZZ_PIN, HIGH);
  delay(100); // Biarkan buzzer menyala selama 1 detik
  digitalWrite(BUZZ_PIN, LOW); // Matikan buzzer
  delay(100); // Biarkan buzzer menyala selama 1 detik
  digitalWrite(BUZZ_PIN, LOW); // Matikan buzzer

  delay(1000); // Tunda selama 1 detik
} else {
  // Handle jika permintaan HTTP tidak berhasil
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EROR");
  Serial.print("HTTP Error: ");
  Serial.println(httpCode);
  delay(3000);
}

clearAndDisplayInitialText();

}
  

