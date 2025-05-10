//1. Definitions
#define HTTPCLIENT_NOSECURE
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>

#define DATA_SIZE 26    
#define BAUD      9600  
#define TEST_UART 1   
#define RXPIN     4     
#define TXPIN     5   

Preferences preferences;

String ssid, password, serverIP, serverPORT;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

void WiFiConfig();
void ServerConfig();
int SendToServer(String type, String val);


void setup() {
  //2. Setup UART0 (Board USB)
  Serial.begin(9600);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  //3. Read data from flash
  preferences.begin("credentials", false);
  ssid = preferences.getString("ssid", ""); 
  password = preferences.getString("password", "");
  serverIP = preferences.getString("serverIP", ""); 
  serverPORT = preferences.getString("serverPORT", "");
  preferences.end();

  //4. Go to: WiFiConfig()
  WiFiConfig();
  
  //5. Go to: ServerConfig()
  Serial.println("Configuring Server IP and PORT");
  ServerConfig();
  
  //6. Config UART1 (for AVR uC)
  Serial1.begin(BAUD, SERIAL_8N1, RXPIN, TXPIN); 
  Serial.println("Starting listening to UART1..");
}




void loop() {
 // 7. Reading from UART1 and sending to HTTP Server by GET 
 //    every given period of time
 if(Serial1.available() && (millis() - lastTime) > timerDelay)
  {
    String type = Serial1.readStringUntil(':');
    String val = Serial1.readStringUntil('\n');

    if(WiFi.status()== WL_CONNECTED){
      int httpVal = SendToServer(type, val);
      Serial.println("HTTP Response code: " + httpVal);
      Serial1.println(httpVal);
    }
    else {
      Serial.println("WiFi Disconnected");
    }

    lastTime = millis();
  }
}



void WiFiConfig()
{
      WiFi.disconnect();
      delay(1000);
      Serial.println("\r\nNot connected to wifi, scanning..");
      delay(1000);

      //4.1 Scanning for networks and printing list
      int n = WiFi.scanNetworks();
      Serial.println("scan done");
      delay(100);
     
    if (n == 0) 
    {
        Serial.println("no networks found");
    } 
    else 
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    
    // 4.2 Trying to connect to last known WiFi
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to WiFi ..");
    int i=0;
    while(WiFi.status() != WL_CONNECTED && i<10) {
        delay(1000);
        Serial.print(".");
        i++;
    }
    
    //4.3 Trying to connect to selected WiFi until succeded
    bool connected = (WiFi.status() == WL_CONNECTED);
    while(!connected)
    {
        Serial.print("\r\nChoose WiFi: ");
        while(!Serial.available());
        int x = Serial.read() - '0';
        if(x >= 1 && x <= n)
        {
            Serial.print("\r\nWrite password for Wifi: "+WiFi.SSID(x-1)+": ");
            while(!Serial.available());
            String userPass = Serial.readString();
            userPass.remove(userPass.length()-1);
            Serial.print(userPass);
            WiFi.begin(WiFi.SSID(x-1), userPass);
            Serial.print("\r\nConnecting to WiFi ..");
            int i=0;
            while (WiFi.status() != WL_CONNECTED && i<15) 
            {
                Serial.print('.');
                delay(1000);
                i++;
            }

            if(WiFi.status() != WL_CONNECTED) Serial.println("\r\nCant connect to WiFi!\n");
            else 
            {
                //4.4 Write WiFi credentials to flash memory
                Serial.println("\r\nConnected to Wifi!");
                preferences.begin("credentials", false);
                preferences.putString("ssid", WiFi.SSID(x-1)); 
                preferences.putString("password", userPass);

                Serial.println("\r\nNetwork Credentials Saved using Preferences");

                preferences.end();
                connected=true;
            }
        }
        else Serial.println("\r\nWifi number is not correct!\r\n");
    }
       

    }
    Serial.println("");
    //4.5 Exit function

}




void ServerConfig()
{
  //5.1 Send to last known server test GET query
  bool serverOK = (SendToServer("CheckConnection", "1") == 200);

  //5.2 Trying to connect to given IP and port with test GET query unitl succeded
  while(!serverOK)
  {
     Serial.println("\r\nProvide server IP: ");
     while(!Serial.available());
     String userIP = Serial.readString();
     userIP.remove(userIP.length()-1);
     Serial.println(userIP);
     Serial.println("\r\nProvide server PORT: ");
     while(!Serial.available());
     String userPORT = Serial.readString();
     userPORT.remove(userPORT.length()-1);
     Serial.println(userPORT);

    serverIP = userIP;
    serverPORT = userPORT;

    //5.3 Write to flash if proper HTTP code returned
    if(SendToServer("CheckConnection", "1") == 200)
    {
      serverOK=true;
      serverIP = userIP;
      serverPORT = userPORT;
      preferences.begin("credentials", false);
      preferences.putString("serverIP", serverIP); 
      preferences.putString("serverPORT", serverPORT);

      Serial.println("\rServer Credentials Saved using Preferences");

      preferences.end();
    }
  } 
  //5.4 Exit function
  Serial.println("Connection to server established!");
}




int SendToServer(String type, String val)
{
  HTTPClient http;
  String serverPath = "http://" +  serverIP + ":" + serverPORT + "/update-sensor?" + type + "=" + val;
  Serial.println(serverPath);
  http.begin(serverPath.c_str());

  delay(200);
  int httpResponseCode = http.GET();
  delay(200);
  //Serial.print(httpResponseCode);
  delay(200);
  http.end();

  return httpResponseCode;
}