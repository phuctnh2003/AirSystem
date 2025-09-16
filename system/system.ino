#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
//wifi
const char* ssid = "nhom4";           
const char* password = "12345678";      

// mqtt
#define PORT_MQTT 2005
const char* broker = "server.vohoangnam.id.vn";
const char* mqtt_client_name = "MQTT_clkk";
const char* mqtt_user = "neosoftmqtt";
const char* mqtt_pass = "Learning2009!";
const char* topic_data1 = "sensor/temperature";
const char* topic_data2 = "sensor/humidity";
const char* topic_data3 = "sensor/airquality";

const int led1 = 19; // LED 1
const int led2 = 17; // LED 2
const int led3 = 18; // LED 3

#define DHTTYPE DHT11
const int pin = 4;
DHT dht(pin, DHTTYPE);

const int smokeSensor = 32;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  dht.begin();

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     Serial.println("WiFi Status: " + String(WiFi.status()));
    if (millis() - startAttemptTime > 30000) {
      Serial.println("\nFailed to connect to WiFi, restarting...");
      ESP.restart();
    }
  }
  mqttClient.setServer(broker, PORT_MQTT);
}

void loop() {

  if (!mqttClient.connected()) {
    if (connectMQTT()) {
      Serial.println("MQTT connected.");
    } else {
      Serial.println("Failed to connect to MQTT, retrying...");
      delay(5000);  
      return;
    }
  }

  // Read DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

 
  // Publish data to MQTT
  char tempStr[10], humStr[10];
  sprintf(tempStr, "%.2f", temperature);
  sprintf(humStr, "%.2f", humidity);

  if (mqttClient.publish(topic_data1, tempStr)) {
    sendLog("Nhiệt độ: " + String(temperature));
  } else {
    Serial.println("Failed to send temperature.");
  }

  if (mqttClient.publish(topic_data2, humStr)) {
    
      sendLog("Độ ẩm: " + String(humidity));
  } else {
    Serial.println("Failed to send humidity.");
  }
long total = 0;
for (int i = 0; i < 10; i++) {
  total += analogRead(smokeSensor);
  delay(100);
}
int airQuality = total / 10;
int air = (total / 10)*100/4096;


// Phân loại chất lượng không khí
String airQualityStatus;
if (airQuality < 1500) {
  airQualityStatus = "Tốt";
} else if (airQuality >= 1500 && airQuality <= 3000) {
  airQualityStatus = "Bình thường";
} else {
  airQualityStatus = "Xấu";
}

// Gửi trạng thái chất lượng không khí qua MQTT
char airStr[50];
sprintf(airStr, "%d %% (%s)", air, airQualityStatus.c_str());

if (mqttClient.publish(topic_data3, airStr)) {
  sendLog("Chất lượng không khí: " + String(airStr));
} else {
  Serial.println("Gửi dữ liệu CLKK thất bại.");
}
  mqttClient.loop();
  delay(5000); 
}


void sendLog(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure httpsClient;
    httpsClient.setInsecure(); 
    HTTPClient https;

   
    String encodedMessage = urlencode(message);
    String url = "https://javaserver.vohoangnam.id.vn/air_system/write_log?message=" + encodedMessage;

    Serial.println("Sending log to URL: " + url);

    https.begin(httpsClient, url); // Sử dụng HTTPS client
    int httpResponseCode = https.GET();

    if (httpResponseCode > 0) {
      String response = https.getString();
    } else {
      Serial.println("HTTP Request failed with code: " + String(httpResponseCode));
    }

    https.end();
  } else {
    Serial.println("WiFi not connected. Cannot send log.");
  }
}

// Hàm mã hóa URL
String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encodedString += c; // Ký tự chữ và số giữ nguyên
    } else {
      encodedString += '%'; // Ký tự đặc biệt thay bằng %
      code1 = (c >> 4) & 0xF; // Lấy 4 bit cao
      code2 = c & 0xF;        // Lấy 4 bit thấp
      code0 = code1 + '0';
      if (code1 > 9) {
        code0 = code1 - 10 + 'A';
      }
      encodedString += code0;
      code0 = code2 + '0';
      if (code2 > 9) {
        code0 = code2 - 10 + 'A';
      }
      encodedString += code0;
    }
  }
  return encodedString;
}

bool connectMQTT() {
  Serial.print("Connecting to MQTT: ");
  Serial.println(broker);
  if (mqttClient.connect(mqtt_client_name, mqtt_user, mqtt_pass)) {
    Serial.println("MQTT connected.");
    return true;
  }
  Serial.println("MQTT connection failed.");
  return false;
}
