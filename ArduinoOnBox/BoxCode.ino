#include <WiFi.h>
#include <PubSubClient.h>

int LED_mon = 25;  // the number of the LED pin
int LED_tue = 33;
int LED_wed = 32;
int LED_thu = 26;
int LED_fri = 4;
int LED_sat = 16;
int LED_sun = 17;

int bPin_mon = 18;  // the number of the pushbutton pin
int bPin_tue = 5;
int bPin_wed = 23;
int bPin_thu = 22;
int bPin_fri = 21;
int bPin_sat = 19;
int bPin_sun = 1;


/* change it with your ssid-password */
const char* ssid = "";
const char* password = "";


/* this is the IP of PC/raspberry where you installed MQTT Server 
on Wins use "ipconfig" 
on Linux use "ifconfig" to get its IP address */
const char* mqtt_server = "";
const char* mqttUser = "";
const char* mqttPassword = "";

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

/* topics */

// OpenCell Tell server i waitin to know which cell to open
// start - Tell Server that i ready
// taken - Tell Server that someone take pill

long lastMsg = 0;
char msg[20];

void ledOn(int led,int btn, int day){
	digitalWrite(led, HIGH);
	if (digitalRead(btn)== HIGH){ 
	  while(digitalRead(btn) == HIGH){} 
	}else{  
	  while(digitalRead(btn) == LOW){}
	}
	digitalWrite(led, LOW);
	char a[1];
	sprintf(a,"%d",day);
	client.publish("taken", a);
	Serial.print("\nbtn #");
	Serial.print(day);
	Serial.println(" pushed !");
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
	
	Serial.print("Message received: ");
	Serial.println(topic);
	Serial.print("payload: ");
    Serial.print((char)payload[0]);
	
    if ((char)payload[0] == '1') {
      ledOn(LED_sun,bPin_sun,1);    
    }else if ((char)payload[0] == '2') {
      ledOn(LED_mon,bPin_mon,2);
    }else if ((char)payload[0] == '3') {
      ledOn(LED_tue,bPin_tue,3);
    }else if ((char)payload[0] == '4') {
      ledOn(LED_wed,bPin_wed,4);
    }else if ((char)payload[0] == '5') {
      ledOn(LED_thu,bPin_thu,5);
    }else if ((char)payload[0] == '6') {
      ledOn(LED_fri,bPin_fri,6);
    }else if ((char)payload[0] == '7') {
      ledOn(LED_sat,bPin_sat,7);
    }
	Serial.println();
  
}


void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe("OpenCell");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() {
//  Leds
  pinMode(LED_mon, OUTPUT);
  pinMode(LED_tue, OUTPUT);
  pinMode(LED_wed, OUTPUT);
  pinMode(LED_thu, OUTPUT);
  pinMode(LED_fri, OUTPUT);
  pinMode(LED_sat, OUTPUT);
  pinMode(LED_sun, OUTPUT);
// Buttons
  pinMode(bPin_mon, INPUT);
  pinMode(bPin_tue, INPUT);
  pinMode(bPin_wed, INPUT);
  pinMode(bPin_thu, INPUT);
  pinMode(bPin_fri, INPUT);
  pinMode(bPin_sat, INPUT);
  pinMode(bPin_sun, INPUT);
  
  Serial.begin(9600);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
 
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port, 1883 is default */
  client.setServer(mqtt_server,11608);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
  while (!client.connected()){
      mqttconnect();
    }
  client.publish("start", "1");  
}
void loop() {
  /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
 client.loop();

    
    
   
   
    
  }

