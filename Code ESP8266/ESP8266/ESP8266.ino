#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

//#define Token "pH1jsYdUiLBHBe4H9fsV"

WiFiManager wifiManager;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
SoftwareSerial STM32Serial(D5, D6);// D5 - RX, D6 -TX

//char token[20], token_temp[20];
int status = WL_IDLE_STATUS;
char ThingsboardServer[] = "demo.thingsboard.io";
const char Token[]="pH1jsYdUiLBHBe4H9fsV";
//WiFiManagerParameter custom_token_access("apikey", "Token Access", token, 20);


void setup() {
  
      WiFi.mode(WIFI_STA);// ESP mac dinh ket noi voi tram phat wifi
    Serial.begin(115200);
    STM32Serial.begin(9600);
 // wifiManager.resetSettings();


     wifiManager.setConnectTimeout(15);

     wifiManager.setMinimumSignalQuality(20);
       
     wifiManager.setAPCallback(configModeCallback);
     if (wifiManager.autoConnect())
  {
    Serial.println("da ket noi thanh cong :");
    STM32Serial.write('1');
    //Connect to server demo.thingsboard.io with default port 1883
    client.setServer( ThingsboardServer, 1883);
    

  }
  //Nếu kết nối wifi thành công, in thông báo ra màn hình
  else 
  {
  Serial.println("vui long kiem tra wifi");
  
  }

}


void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  STM32Serial.write('0');
}



void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
      reconnect();   
    }
  if (STM32Serial.available() > 0)
  {
    String SHT30_Temp1,SHT30_Hum1,SHT30_Temp2,SHT30_Hum2,BH1750_Lux1,BH1750_Lux2,HM3301_PM25;
    String datastr = STM32Serial.readString();
    
    Serial.println("datastr: " + datastr);
    int kiemtra = datastr.indexOf("H");
    
    if( kiemtra > 0)
    {
      int A = datastr.indexOf("A");
      int B = datastr.indexOf("B");
      int C = datastr.indexOf("C");
      int D = datastr.indexOf("D");
      int E = datastr.indexOf("E");
      int F = datastr.indexOf("F");
      int G = datastr.indexOf("G");
      int H = datastr.indexOf("H");
      SHT30_Temp1 = datastr.substring(A + 1 , B);
      SHT30_Hum1 = datastr.substring(B + 1 , C);
      SHT30_Temp2 = datastr.substring(C + 1, D);
      SHT30_Hum2 = datastr.substring(D +1, E);
      BH1750_Lux1 = datastr.substring(E + 1, F);
      BH1750_Lux2 = datastr.substring(F + 1, G);
      HM3301_PM25 = datastr.substring(G + 1, H);
      Serial.println("SHT30_Temp1:" + SHT30_Temp1 + " SHT30_Hum1:" + SHT30_Hum1 + " SHT30_Temp2:" + SHT30_Temp2 + " SHT30_Hum2:" + SHT30_Hum2 + "BH1750_Lux1:" + BH1750_Lux1 + "BH1750_Lux2:" + BH1750_Lux2 + "HM3301_PM25:" + HM3301_PM25);
      


      String payload = "{" ;
      payload += "\"SHT30_Temp1\":"; payload += SHT30_Temp1; payload += ",";
      payload += "\"SHT30_Hum1\":"; payload += SHT30_Hum1; payload += ",";
      payload += "\"SHT30_Temp2\":"; payload += SHT30_Temp2; payload += ",";
      payload += "\"SHT30_Hum2\":"; payload += SHT30_Hum2; payload += ",";
      payload += "\"BH1750_Lux1\":"; payload += BH1750_Lux1; payload += ",";
      payload += "\"BH1750_Lux2\":"; payload += BH1750_Lux2; payload += ",";
      payload += "\"HM3301_PM2.5\":"; payload += HM3301_PM25; payload += "}";

      char attributes[200];
      payload.toCharArray( attributes, 200 );
      Serial.println( attributes );
      if(client.publish( "v1/devices/me/telemetry", attributes) != 0)
      {
        Serial.println( "[DONE]" );
        }
        else Serial.println( "[Fall]" );
      }
      
  }  
delay(1000);
    client.loop();
}

void reconnect() {
   int error;

    if (!client.connected()) {
   
    status = WiFi.status();
    if (status != WL_CONNECTED)
    {
      wifiManager.setConnectTimeout(60);
      if (wifiManager.autoConnect()) {
        Serial.println("da ket noi lai thanh cong");
      }
      else {
        Serial.println("Config WiFi running");
      }
    } 
          Serial.print("Connecting to ThingsBoard node ...");
          if ( client.connect(ThingsboardServer, Token,NULL) ) {
          Serial.println( "[DONE]" );}
      else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : please config again]" );
      //Try to connect after 5s
    //  delay(5000);
      error++;
      if (error > 10)
      {
        wifiManager.resetSettings(); //
      }
    }
    }
    
   
}
