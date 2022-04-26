#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <HttpClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

#define SS_PIN D8  //--> SDA // SS is connected to pinout D2
#define RST_PIN D4
MFRC522 mfrc522(SS_PIN, RST_PIN);
#define BUZZ D3

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID="";
String serverName = "http://mpap.in/rfid2/uid_insert.php";
HTTPClient http;
WiFiClient client;




void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  Serial.println("Communication Started \n\n");
  WiFi.begin("Ankan", "ankan#1230");
  Serial.print("Connecting");

  pinMode(BUZZ, OUTPUT);
  SPI.begin();      //--> Init SPI bus
  mfrc522.PCD_Init();
  delay(10);


 
   

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    
    delay(200);
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("device ok");
  delay(2000);

}

void loop() {
  Serial.print("device in loop \n");
  delay(200);
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.print("device online \n");
    int readsuccess=getid();
     if(readsuccess)
     {
      Serial.println(StrUID);
        String uid=StrUID;
        String postdata="rfid_uid="+uid;
        //String server=serverName;
        http.begin(client,serverName.c_str());  //Specify request destination
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header
        
        int httpResponseCode = http.POST(postdata);
        if (httpResponseCode>0) 
        {
          String payload = http.getString();
          Serial.println(httpResponseCode);
          Serial.println(payload);
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, payload);
          String status = doc["messssage"];
          Serial.println(status);
          digitalWrite(BUZZ, HIGH);
          http.end();  //Close connection
          delay(200);
          digitalWrite(BUZZ, LOW);
        }
      delay(1000);
     }
  }
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Not Connected To Wifi");
    digitalWrite(BUZZ, HIGH);
    delay(200);
    digitalWrite(BUZZ, LOW);
  
  }
  
}


int getid(){
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
    
  }
  
    Serial.print("THE UID OF THE SCANNED CARD IS : ");
    for (int i = 0; i < 4; i++) {
      readcard[i]= mfrc522.uid.uidByte[i];
      array_to_string(readcard, 4, str);
      StrUID = str;
      
    }
    mfrc522.PICC_HaltA();
    return 1;
    
}
//--------------------------------------- Change the result of reading an array UID into a string------------------------------------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
