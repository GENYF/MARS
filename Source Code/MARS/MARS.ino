#include <ESP8266WiFi.h>

//서버에 전송하기 위한 초기설정
const char* server = "api.thingspeak.com";
String apiKey = "LF66N9URKIGFW9QT";
const char* MY_SSID = "Four season study cafe 1";//와이파이 이름
const char* MY_PWD = "fd9fgf3781";//와이파이 비밀번호

const int trigPin = D5;
const int echoPin = D6;
const int LEDPin = D7;

int flag = 0;

void setup()
{
	Serial.begin(115200);
	delay(10);

	//와이파이 연결
	Serial.print("Connecting to " + *MY_SSID);

	WiFi.begin(MY_SSID, MY_PWD);

	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
	}

	Serial.println("\nConnected\n");

	pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT);
}

void loop(){
  double distance = WriteDistance();

  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println("cm");

  if(distance > 400 || distance < 2){
    Serial.println("Error");
    distance = 2;
  }
  
  if(distance < 10 && flag == 0){
    flag = 1;
    digitalWrite(LEDPin, HIGH);//LED ON
    ServerUpload(flag);
  }
  else if(distance < 10 && flag == 1){
    flag = 1;
    digitalWrite(LEDPin, HIGH);//LED ON
  }
  else if(distance >= 10 && flag == 1){
    flag = 0;
    digitalWrite(LEDPin, LOW);//LED OFF
    ServerUpload(flag);
  }
  else {
    flag = 0;
    digitalWrite(LEDPin, LOW);//LED OFF
  }

  delay(3000);
}

double WriteDistance(){
  double duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);   
  digitalWrite(trigPin, LOW); 

  duration = pulseIn(echoPin, HIGH);  

  distance = duration * 0.034 / 2;

  return distance;
}

void ServerUpload(int check)
{
  WiFiClient client;//Wifi 연결

  Serial.println("check : " + String(check));
  if (client.connect(server, 80)) { // 서버에 전송
    Serial.println("WiFi Client connected");

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(check);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  Serial.print("Waiting...\n\n\n");
  client.stop();
}
