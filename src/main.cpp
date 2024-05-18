#include <Arduino.h>
#include <M5Core2.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_ssid";
const char* password = "your_password";
const char* mqtt_server = "Broker_IP"; //192.168.0.178 0 192.168.1.229;

WiFiClient espClient;
PubSubClient client(espClient);

void drawOptions();
void setupWifi();
void reConnect();
void publisOption(char option);
void SetVibration(int time);
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  M5.begin();
  
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextColor(GREEN , BLACK);
  setupWifi();
 client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 


}


void loop() {
    if (!client.connected()) {
      reConnect();
    }

    client.loop();

    M5.update();  // Read the press state of the key.  读取按键 A, B, C 的状态
    if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
      publisOption('1');
    } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
      publisOption('2');
    } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) {
      publisOption('3');
    }
}


void setupWifi() {
    
    delay(10);
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(
        WIFI_STA);  // Set the mode to WiFi station mode.  设置模式为WIFI站模式
    WiFi.begin(ssid, password);  // Start Wifi connection.  开始wifi连接

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");
}

void callback(char* topic, byte* payload, unsigned int length) {
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(1);
    
    for (int i = 0; i < length; i++) {
      char currentChar = (char)payload[i];
      if (currentChar == '@') {
            M5.Lcd.println(); // Salto de línea si el carácter es '@'
        } else {
            M5.Lcd.print(currentChar);
        }
    }
    M5.Lcd.println();
    SetVibration(180);
    drawOptions();
    
}

void drawOptions(){
  //dibuja una linea en la parte baja de la pantalla
  M5.Lcd.drawLine(0, 210, 320, 210, GREEN);
  M5.Lcd.setCursor(30, 220);
  M5.Lcd.println("Opcion 1");
  M5.Lcd.setCursor(135, 220);
  M5.Lcd.println("Opcion 2");
  M5.Lcd.setCursor(245, 220);
  M5.Lcd.println("Reiniciar");
  
}

void reConnect() {
    while (!client.connected()) {
        M5.Lcd.print("Attempting MQTT connection...");
        // Create a random client ID.  创建一个随机的客户端ID
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.  尝试重新连接
        if (client.connect(clientId.c_str())) {
            M5.Lcd.printf("\nSuccess\n");
            client.subscribe("response");
        } else {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void SetVibration(int time){
  M5.Axp.SetVibration(true);
  delay(time);
  M5.Axp.SetVibration(false);
}

void publisOption(char option){
  String message = String(option);
  client.publish("game", message.c_str());
  SetVibration(60);
  M5.Lcd.clearDisplay();
  M5.Lcd.setCursor(120, 100);
  M5.Lcd.println("Escribiendo...");
  }