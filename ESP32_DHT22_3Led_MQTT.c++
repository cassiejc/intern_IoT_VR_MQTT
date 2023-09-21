#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 22
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

#define LED_RED 23
#define LED_GREEN 18
#define LED_YELLOW 19

// WiFi
const char *ssid = "WiFiName"; // Enter your WiFi name
const char *password = "WiFiPassword";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32/led";
const char *mqtt_username = "mqtt";
const char *mqtt_password = "connect";
const int mqtt_port = 1883;

bool ledState1 = false;
bool ledState2 = false;
bool ledState3 = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setupMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port); // Mengatur detail broker target  yang digunakan
  mqttClient.setCallback(callback);           // jika kita ingin menerima pesan untuk langganan yang dibuat oleh klien
}

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    dht.begin();
    delay(1000); // Delay for stability

    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");

    // Setting LED pin as output
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);

    digitalWrite(LED_RED, LOW);  // Turn off the LED initially
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);

    // Connecting to an MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "ESP32Client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Publish and subscribe
    setupMQTT();
    client.publish(topic, "hello emqx");
    client.subscribe(topic);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
      }      
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char) payload[i];  // Convert *byte to string
    }
    Serial.print(message);
    
    if (message == "red-on" && !ledState1) {
        digitalWrite(LED_RED, HIGH);  // Turn on the LED
        ledState1 = true;
    }
    if (message == "red-off" && ledState1) {
        digitalWrite(LED_RED, LOW); // Turn off the LED
        ledState1 = false;
    }
    if (message == "green-on" && !ledState2) {
        digitalWrite(LED_GREEN, HIGH);  // Turn on the LED
        ledState2 = true;
    }
    if (message == "green-off" && ledState2) {
        digitalWrite(LED_GREEN, LOW); // Turn off the LED
        ledState2 = false;
    }
    if (message == "yellow-on" && !ledState3) {
        digitalWrite(LED_YELLOW, HIGH);  // Turn on the LED
        ledState3 = true;
    }
    if (message == "yellow-off" && ledState3) {
        digitalWrite(LED_YELLOW, LOW); // Turn off the LED
        ledState3 = false;
    }
    

    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    client.loop();
    
    if (!mqttClient.connected()){
    reconnect();   
  }
  else{
    float temperature = dht.readTemperature();
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);  
    Serial.print("Temperature: ");
    Serial.println(tempString);
    mqttClient.publish("emqx/esp32/temperature", tempString);
  
    float humidity = dht.readHumidity();;
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    mqttClient.publish("emqx/esp32/humidity", humString);
    delay(2000);
    
}
}
