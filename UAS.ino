#include <WiFi.h>
#include <MQTT.h>
#include <LiquidCrystal_I2C.h>


#define pinPompa 18
#define pinKelembaban 36
#define pinPintu 32

#define kondisiBasah 500
#define kondisiKering 750

const char ssid[] = "helloworld";
const char pass[] = "helloworld";
String statuspintu = "";
WiFiClient net;
MQTTClient client;

LiquidCrystal_I2C lcd(0x27,16,2);

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("ESP32", "tugasakhiriot", "FSXrbtjYFBzTpH1o")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe("/perintah");

}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if(payload == "1"){
    digitalWrite(pinPintu,LOW);
    client.publish("/Pintu", "UNLOCK");
    } else {digitalWrite(pinPintu,HIGH);
    client.publish("/Pintu", "LOCK");}
  
}


void mulai(){
  int nilaiKelembaban = analogRead(pinKelembaban);
  lcd.setCursor(0,0);
  lcd.print("KONDISI:");
  lcd.setCursor(0,1);
  lcd.print("POMPA  :");
  String myStr;
  myStr = String(nilaiKelembaban);
  
  if (nilaiKelembaban < kondisiBasah) {
    lcd.setCursor(9,0);
    lcd.print("BASAH");
    lcd.setCursor(9,1);
    lcd.print("OFF");
    digitalWrite(pinPompa,LOW);
     
    client.publish("/SmartFlower/kelembaban", myStr);
    client.publish("/SmartFlower/kondisi", "BASAH");
    client.publish("/SmartFlower/pompa", "MATI");
  } else if (nilaiKelembaban >= kondisiBasah && nilaiKelembaban < kondisiKering) {
    lcd.setCursor(9,0);
    lcd.print("IDEAL");
    lcd.setCursor(9,1);
    lcd.print("OFF");
    digitalWrite(pinPompa,HIGH);
    
    client.publish("/SmartFlower/kelembaban", myStr);
    client.publish("/SmartFlower/kondisi", "IDEAL");
    client.publish("/SmartFlower/pompa", "MATI");
  } else {
    lcd.setCursor(9,0);
    lcd.print("KERING");
    lcd.setCursor(9,1);
    lcd.print("ON");
    digitalWrite(pinPompa,HIGH);
    
    client.publish("/SmartFlower/kelembaban", myStr);
    client.publish("/SmartFlower/kondisi", "KERING");
    client.publish("/SmartFlower/pompa", "MENYALA");
  }
}

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin("public.cloud.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();
  analogReadResolution(10);
  pinMode(pinPompa,OUTPUT);
  pinMode(pinPintu,OUTPUT);
  digitalWrite(pinPompa,HIGH);
  digitalWrite(pinPintu,HIGH);
  lcd.init();
  lcd.backlight();
}

void loop()
{
  client.loop();
  delay(10);
  if (!client.connected()) {
    connect();
  }
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    mulai();
  }
  
}
  
