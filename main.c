//* Create a WiFi access point and provide a web server on it. */
//** For more details see http://42bots.com.
//*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "TM1637.h"
#define CLK 5 //pins definitions for TM1637 and can be changed to other ports
#define DIO 4
TM1637 tm1637(CLK,DIO);

IPAddress    apIP(42, 42, 42, 42);  // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "ESP8266";
const char *password = "ESP8266Test";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

const int ledPin = LED_BUILTIN; // an LED is connected to NodeMCU pin D1 (ESP8266 GPIO5) via a 1K Ohm resistor

bool ledState = false;

int liczba = 0;

void handleRoot() 
{
  digitalWrite (LED_BUILTIN, 0); //turn the built in LED on pin DO of NodeMCU on
  digitalWrite (ledPin, server.arg("led").toInt());
  ledState = digitalRead(ledPin);

  tm1637.init();
  liczba = server.arg("led").toInt(); // To, co podajemy w adresie
  tm1637.display(3,liczba % 10); //(POLE, CYFRA)
  liczba = liczba / 10;
  
  if (liczba > 0)
  tm1637.display(2,liczba % 10);
  
  liczba = liczba / 10;
  if (liczba > 0)
   tm1637.display(1,liczba % 10);
  
 /* Dynamically generate the LED toggle link, based on its current state (on or off)*/
  char ledText[80];
  
  if (ledState) 
  {
    strcpy(ledText, "LED is on. <a href=\"/?led=0\">Turn it OFF!</a>");
  }
  else 
  {
    strcpy(ledText, "LED is OFF. <a href=\"/?led=1\">Turn it ON!</a>");
  }
 
  ledState = digitalRead(ledPin);

  char html[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  int brightness = analogRead(A0);
  brightness = (int)(brightness + 5) / 10; //converting the 0-1024 value to a (approximately) percentage value

// Build an HTML page to display on the web-server root address
  snprintf ( html, 1000,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='30'/>\
    <title>ESP8266 WiFi Network</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
      h1 { Color: #AA0000; }\
    </style>\
  </head>\
  <body>\
    <h1>ESP8266 Wi-Fi Access Point and Web Server Demo</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Brightness: %d%</p>\
    <p>%s<p>\
    <p>This page refreshes every 10 seconds. Click <a href=\"javascript:window.location.reload();\">here</a> to refresh the page now.</p>\
  </body>\
</html>",

    hr, min % 60, sec % 60,
    brightness,
    ledText
  );
  server.send ( 200, "text/html", html );
  digitalWrite ( LED_BUILTIN, 1 );
}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in LED on pin DO of NodeMCU off
}

void setup() {
tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;


  
  pinMode ( ledPin, OUTPUT );
  digitalWrite ( ledPin, 0 );
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot );
  server.on ( "/led=1", handleRoot);
  server.on ( "/led=0", handleRoot);
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  int8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//0~9,A,b,C,d,E,F
    int8_t ListDisp[4];
    unsigned char i = 0;
    unsigned char count = 0;
/*    delay(150);
    while(1)
    {
        i = count;
        count ++;
        if(count == sizeof(NumTab)) count = 0;
        for(unsigned char BitSelect = 0;BitSelect < 4;BitSelect ++)
        {
            ListDisp[BitSelect] = NumTab[i];
            i ++;
            if(i == sizeof(NumTab)) i = 0;
        }
        tm1637.display(0,ListDisp[0]);
        tm1637.display(1,ListDisp[1]);
        tm1637.display(2,ListDisp[2]);
        tm1637.display(3,ListDisp[3]);
        delay(300);
    }
    */
  server.handleClient();
  //tm1637.display(0,ListDisp[0]);
}
