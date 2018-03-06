/*
  Lightsense Client for MKR1000

  This sketch connects to a Lightsense server every minute and downloads the command 0 or 1.
  Turns light on if 1. Doesn't do anything if 0.
  If button is pressed, sends command to server that it should change status to 1.
*/
#include <SPI.h>
#include <WiFi101.h>
#include <RTCZero.h>

char ssid[] = "SSID";      //  your network SSID (name)
char pass[] = "********";       // your network password

const int LED = 6;
const int BUTTON = 7;

int message_recieved = 0;
int button = 0;

bool should_read_button = true;

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiSSLClient client;

// server address:
//char server[] = "www.arduino.cc";
char server[] = "lightsense-api.herokuapp.com";

bool sendRequest = true; // used to understand if the http request must be sent

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 50;
const byte minutes = 00;
const byte hours = 17;

/* Change these values to set the current initial date */
const byte day = 02;
const byte month = 01;
const byte year = 18;

void setup() {
  //Initialize Serial and wait for port to open:
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

  connectToAP();    // connect the board to the access point
  printWifiStatus();

  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(0, 0, 0);    //in this way the request is sent every minute at 0 seconds
  rtc.enableAlarm(rtc.MATCH_SS);

  rtc.attachInterrupt(alarmMatch);
}

void loop() {
  // READING SERVER every minute - sendRequest is set to true at the start of each minute.
  // Also check here whether wifi is OK or not!
  if (sendRequest) {
    sendRequest = false;
    // every minute we allow the reading of the button again.
    should_read_button = true;
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wifi down!");
      digitalWrite(LED, LOW);
      button = LOW;
      message_recieved = 0;
      wifiConnectJist();
    }
    httpRequest("GET /qZ9ye7JhQgeZCDQ9tQWTgXPGwy7EQaGsjSSC HTTP/1.1");
    message_recieved = listenToClient();
  }

  if (should_read_button && message_recieved == 0) {
    button = digitalRead(BUTTON);
  }

  // Turn light on if either happened
  if (button == HIGH || message_recieved) {

    // Turn light on as fast as possible
    digitalWrite(LED, HIGH);

    // Set server to "1" if button happened
    if (button == HIGH) {
      Serial.println("Button was high. Sending request!");
      httpRequest("GET /qZ9ye7JhQgeZCDQ9tQWTgXPGwy7EQaGsjSSC/on HTTP/1.1");
      // If I pressed the button I also pretend that I recieved a message.
      message_recieved = 1;
      should_read_button = false;
      button = LOW;
    }
  }
  else {
    // If server's status is 0 (i.e. message_recieved==0), then turn light off.
    digitalWrite(LED, LOW);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void alarmMatch() {
  sendRequest = true;
}

void connectToAP() {
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  Serial.println("I got into the connectToAP function! :) ");
  while ( status != WL_CONNECTED) {
    wifiConnectJist();
    // wait 1 second for connection:
    delay(1000);
  }
}

void wifiConnectJist() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  status = WiFi.begin(ssid, pass);

  // wait 1 second for connection:
  delay(1000);

  if (status == WL_CONNECTED) {
    Serial.println("Setup complete.\n");
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
  } else {
    Serial.println("FAIL");
    for (int i=0; i <= 5; i++){
        digitalWrite(LED, HIGH);
        delay(1000);
        digitalWrite(LED, LOW);
        delay(1000); 
      }
  }
}

// this method makes a HTTP connection to the server:
void httpRequest(char httpGETtext[]) {
  sendRequest = false;

  // Print request time
  Serial.println();
  Serial.print("Request sent @ ");
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
  Serial.println();
  // port: 433
  if (client.connect(server, 443)) {
    // Serial.println("Connected!!!!!");
    // Serial.println(server);
    // Make a HTTP request:
    // https://lightsense-api.herokuapp.com/qZ9ye7JhQgeZCDQ9tQWTgXPGwy7EQaGsjSSC
    // httpGETtext should be "GET /qZ9ye7JhQgeZCDQ9tQWTgXPGwy7EQaGsjSSC HTTP/1.1"
    // or "GET /qZ9ye7JhQgeZCDQ9tQWTgXPGwy7EQaGsjSSC/on HTTP/1.1"
    client.println(httpGETtext);
    client.println("Host: lightsense-api.herokuapp.com");
    //client.println("GET /asciilogo.txt HTTP/1.1");
    //client.println("Host: www.arduino.cc");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed");
  }
}

int listenToClient()
{
  unsigned long startTime = millis();
  bool received = false;
  char c2;

  while ((millis() - startTime < 5000) && !received) { //try to listen for 5 seconds
    while (client.available()) {
      received = true;
      char c = client.read();
      c2 = (int)c - 48;
      Serial.write(c);
    }
  }
  client.stop();
  Serial.println();
  return c2;
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}
