#include <Arduino.h>
#include <WiFi.h>
#include <iostream>
#include <sstream>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#define up 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define STOP 0

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define FORWARD 1
#define BACKWARD -1

const int PWMFreq = 1000; //1khz
const int PWMResolution = 8;
const int PWMSpeedChannel = 4;

const char* ssid = "tsungmin-iot-car";
const char* password = "123456789";

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

struct MOTOR_PINS
{
  int pinEn;
  int pinIN1;
  int pinIN2;
};   //  ";" 勿忘

std::vector<MOTOR_PINS> motorPins =
{
  {22,16,17},   //right motor (EnA,IN1,IN2)
  {23,18,19},   //left motor (EnB,IN3,IN4)
};


void setUpPinModes()
{
  ledcSetup(PWMSpeedChannel,PWMFreq,PWMResolution);   //使用到的頻道 頻率 解析度

  for (int i=0; i< motorPins.size();i++)
  {
    pinMode(motorPins[i].pinEn,OUTPUT);
    pinMode(motorPins[i].pinIN1,OUTPUT);
    pinMode(motorPins[i].pinIN2,OUTPUT);

    ledcAttachPin(motorPins[i].pinEn,PWMSpeedChannel);
  }

  pinMode(22,OUTPUT);
}

void handleRoot(AsyncWebServerRequest *request)
{
  request->send(200,"text/html",htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404,"text/plain","File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server,
                              AsyncWebSocketClient *client, 
                              AwsEventType type,
                              void *arg, 
                              uint8_t *data,
                              size_t len)
{
  switch(type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
    case WS_EVT_DATA:
      
    break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR: 
    break;
    default:
    break;
  }
}


void setup() {
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address:");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);
  server.begin();
  Serial1.println("HTTP Server Started");

}

void loop() {

}

