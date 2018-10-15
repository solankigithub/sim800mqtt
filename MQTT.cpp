///Define class for MQTT
#include <stdint.h>
#include <string.h>
#include "Arduino.h"
#include "MQTT.h"

char topic[100];
char message[4000];
uint8_t i=0;

MQTT::MQTT(SIM8000 sim800){
        
}

bool MQTT::connect(const char* MQTTClientID, const char* MQTTUsername, const char* MQTTPassword)
{
    unsigned long datalength;
    int X;
    unsigned char encodedByte;
    Serial.println("Connecting to MQTT Broker...");
    Serial1.write(0x10);
    MQTTProtocolNameLength = strlen(MQTTProtocolName);
    MQTTClientIDLength = strlen(MQTTClientID);
    MQTTUsernameLength = strlen(MQTTUsername);
    MQTTPasswordLength = strlen(MQTTPassword);
    
    datalength = MQTTProtocolNameLength + 2 + 4 + MQTTClientIDLength + 2 + MQTTUsernameLength + 2 + MQTTPasswordLength + 2;
    Serial.print("Data Length :"); Serial.println(datalength);
    X = datalength;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }

      Serial1.write(encodedByte);
    }
    while ( X > 0 );
    Serial1.write(MQTTProtocolNameLength >> 8);
    Serial1.write(MQTTProtocolNameLength & 0xFF);
    Serial1.write(MQTTProtocolName);

    Serial1.write(MQTTLVL); // LVL
    Serial1.write(MQTTFlags); // Flags
    Serial1.write(MQTTKeepAlive >> 8);
    Serial1.write(MQTTKeepAlive & 0xFF);

    Serial1.write(MQTTClientIDLength >> 8);
    Serial1.write(MQTTClientIDLength & 0xFF);
    Serial1.print(MQTTClientID);

    Serial1.write(MQTTUsernameLength >> 8);
    Serial1.write(MQTTUsernameLength & 0xFF);
    Serial1.print(MQTTUsername);

    Serial1.write(MQTTPasswordLength >> 8);
    Serial1.write(MQTTPasswordLength & 0xFF);
    Serial1.print(MQTTPassword);    
    Serial.println("Connection packet sent...");
    packetType = CONNECT;
    unsigned long previous = millis();
    uint8_t code = 0;
    /*
    while(1){
        if(millis()-previous>2000)  break;
        if(Serial1.available())
        {
            Serial.println("CONNACK received...");            
            while(Serial1.available()){               
                code=Serial1.read();
                Serial.println((char)code);
                if(code==0x20) {
                    code=Serial1.read();    //Byte2 of CONNACK
                    code=Serial1.read();    //Byte3 of CONNACK            
                    code=Serial1.read();    //Byte4 of CONNACK - Return Code
                    if(code==0x00)          //Successfully Connected to MQTT broker
                        Serial.println("Successfully Connected to MQTT broker");                  
                    else
                        Serial.println("Unable to connect to MQTT broker");          
                }                
            }
            break;
        }
    }
    */
    return true;
}

bool MQTT::publish(char* MQTTTopic, char* MQTTMessage){    
    Serial.println("Publishing data...");
    char topic[100], str[4000];
    int datalength=0, topiclength=0, X=0;
    unsigned char encodedByte;
        
    //memset(str, 0, sizeof(str));

    topiclength = sprintf((char*)topic, MQTTTopic);
    datalength = sprintf((char*)str, "%s%s", topic, MQTTMessage);
    

    delay(100);
    Serial1.write(0x30);
    X = datalength + 2;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }
      Serial1.write(encodedByte);
    }
    while ( X > 0 );

    Serial1.write(topiclength >> 8);
    Serial1.write(topiclength & 0xFF);    
    Serial1.print(str);

}

bool MQTT::subscribe(char* MQTTTopic)
{
    int X=0; 
    unsigned char encodedByte;  
    unsigned short topiclength;
    unsigned long datalength;
    const char MQTTPacketID = 0x0001;
    
    topiclength = strlen(MQTTTopic);
    datalength = 2 + 2 + topiclength + 1;
     
    Serial1.write(0x82);
    X = datalength;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }
      Serial1.write(encodedByte);
    }
    while ( X > 0 );
    Serial1.write(MQTTPacketID >> 8);
    Serial1.write(MQTTPacketID & 0xFF);
    Serial1.write(topiclength >> 8);
    Serial1.write(topiclength & 0xFF);
    Serial1.print(MQTTTopic);
    Serial1.write(MQTTQOS);
    packetType=SUBSCRIBE;     
}

bool MQTT::ping(){
     packetType = PINGREQ;
     Serial1.write(0xC0);
     Serial1.write(0x00);       
}

void MQTT::loop(){  
    ping();    
    serialEvent();
}

void MQTT::serialEvent(){
    Serial.println("");
    Serial.println("Begin");
    int X;
    unsigned int code;
    unsigned char encodedByte;
    unsigned long multiplier = 1;
    int msglen = 0; int topiclen = 0;
    while(Serial1.available())
    {
        //Serial.println("Begin");
        //Serial.println((char)Serial1.readString());
        //Serial.println("End");
        
        code = Serial1.read();                      
        
        Serial.print((char)code);
        Serial.print(",");
        Serial.print(code,HEX);        
        Serial.print(",");
        Serial.println(code,DEC);        
        
        if(code==0x20)              //CONNACK
        {
            code=Serial1.read();    //Byte2 of CONNACK
            code=Serial1.read();    //Byte3 of CONNACK            
            code=Serial1.read();    //Byte4 of CONNACK - Return Code
            if(code==0x00)          //Successfully Connected to MQTT broker
                Serial.println("Successfully Connected to MQTT broker");                  
            else
                Serial.println("Unable to connect to MQTT broker");      
        }
        
        if(code==0x90)         //SUBACK
        {
            code=Serial1.read();    //Byte2 of SUBACK
            code=Serial1.read();    //Byte3 of SUBACK                                    
            code=Serial1.read();    //Byte4 of SUBACK
            code=Serial1.read();    //Byte5 of SUBACK
            if(code==0x00 || code==0x01 || code==0x02)
                Serial.println("Successfully subscribed..");
            else    // When return is 0x80
                Serial.println("Unable to subscribe..");        
        }        
        else if(code==0xD0 || code==0xC0){
            code=Serial1.read();
            //if(code==0x00)            
                //Serial.println("Ping success..");                        
        }
        
        else if(code==0x30 ) //Message Received
        {            
            Serial.println("Message Begin...");
            
            do{
                encodedByte = Serial1.read();     //Get the Length of the message                
                msglen = msglen + ((encodedByte & 127) * multiplier);                
                multiplier = multiplier * 128;                  
                if(multiplier > 2097152)        // 2097152 = 128 * 128 * 128
                {
                    Serial.println("Exception while calculating the Messge Length");
                    break;                    
                }                                   
            }            
            while((encodedByte & 128)!=0);

            
            code = Serial1.read(); // Topic Len MSB
            //Serial.print((char)code);Serial.print(",");Serial.println(code,HEX);        
            
            code = Serial1.read(); // Topic Len LSB
            //Serial.print((char)code);Serial.print(",");Serial.println(code,HEX);        
            topiclen = code;
            Serial.print("Msg Length:"); Serial.println(msglen,DEC);
            Serial.print("Topic Length:"); Serial.println(topiclen,DEC);
            Serial.print("Topic : ");            
            uint8_t i = 0;
            for(i=0;i<topiclen;i++){
                Serial.print((char)Serial1.read());
            }
            
            Serial.print("\nMessage :");
            
            int count = 0;            
            while(Serial1.available()){                
                code = Serial1.read();
                Serial.print((char)code);
                count++;                
            }           
            
            Serial.print("\nMessage Length = "); Serial.print(count); 
            Serial.println("\nMessage End...");
            
        }
        
  }
  Serial.println("End\n");
}



