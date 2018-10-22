///Define class for MQTT
#include <stdint.h>
#include <string.h>
#include "Arduino.h"
#include "MQTT.h"

char topic[100];
char message[4000];
uint8_t i=0;

MQTT::MQTT(){
    
};

MQTT::MQTT(Stream &out){
    this->OUT = &out;   
};

bool MQTT::initialize(){
    
    if(!sim800.initialize(1, this->OUT)) return false;    
    return sim800.startTCP("m12.cloudmqtt.com",13079);
}

bool MQTT::isConnected(){
    return _isConnected;
}

bool MQTT::connect(const char* MQTTClientID, const char* MQTTUsername, const char* MQTTPassword)
{
    while(UART.available()) UART.read();
    
    unsigned long datalength;
    int X;
    unsigned char encodedByte;
    OUT->println("Connecting to MQTT Broker...");    
    UART.write(0x10);
    MQTTProtocolNameLength = strlen(MQTTProtocolName);
    MQTTClientIDLength = strlen(MQTTClientID);
    MQTTUsernameLength = strlen(MQTTUsername);
    MQTTPasswordLength = strlen(MQTTPassword);
    
    datalength = MQTTProtocolNameLength + 2 + 4 + MQTTClientIDLength + 2 + MQTTUsernameLength + 2 + MQTTPasswordLength + 2;
    OUT->print("Data Length :"); OUT->println(datalength);
    X = datalength;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }

      UART.write(encodedByte);
    }
    while ( X > 0 );
    UART.write(MQTTProtocolNameLength >> 8);
    UART.write(MQTTProtocolNameLength & 0xFF);
    UART.write(MQTTProtocolName);

    UART.write(MQTTLVL); // LVL
    UART.write(MQTTFlags); // Flags
    UART.write(MQTTKeepAlive >> 8);
    UART.write(MQTTKeepAlive & 0xFF);

    UART.write(MQTTClientIDLength >> 8);
    UART.write(MQTTClientIDLength & 0xFF);
    UART.print(MQTTClientID);
    
    UART.write(MQTTUsernameLength >> 8);
    UART.write(MQTTUsernameLength & 0xFF);
    UART.print(MQTTUsername);

    UART.write(MQTTPasswordLength >> 8);
    UART.write(MQTTPasswordLength & 0xFF);
    UART.print(MQTTPassword);    
    
    UART.flush();
    OUT->println("Connection packet sent...");
    
    return false;
}

bool MQTT::publish(char* MQTTTopic, char* MQTTMessage, uint8_t qos){    
    OUT->println("Publishing data...");
    char topic[100], packetid[100], str1[2000], str2[2000];
    
    int datalength=0, topiclength=0,packetidlength=0, X=0;
    unsigned char encodedByte;
    
    topiclength = sprintf((char*)topic, MQTTTopic);
    if(qos>0)
    {
        packetidlength = sprintf((char*)packetid,"PACKETID");
        datalength = sprintf((char*)str1, "%s%s", topic,packetid);
        
    }
    else{
        datalength = sprintf((char*)str1, "%s", topic);
    }
    //str1[datalength] = '\0';
    datalength = sprintf((char*)str2, "%s%s", str1,MQTTMessage);    
    //str2[datalength] = '\0';
    OUT->print("Datalength = ");
    OUT->println(datalength);
    OUT->print("str = ");
    OUT->println(str2);
    
    UART.write(0x30+(qos*2)); //QOS = 1
    delay(100);
    X= (qos>0)?(datalength + 2 + 2):(datalength + 2);
    
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }      
      UART.write(encodedByte);
    }
    while ( X > 0 );
    
    UART.write(topiclength >> 8);
    UART.write(topiclength & 0xFF);    
    UART.write(topic);
    
    if(qos>0){
        UART.write(packetidlength >> 8);
        UART.write(packetidlength & 0xFF);    
        UART.print(packetid);        
    }
    
    UART.print(MQTTMessage);    

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
     
    UART.write(0x82);
    X = datalength;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }
      UART.write(encodedByte);
    }
    while ( X > 0 );
    UART.write(MQTTPacketID >> 8);
    UART.write(MQTTPacketID & 0xFF);
    UART.write(topiclength >> 8);
    UART.write(topiclength & 0xFF);
    UART.print(MQTTTopic);
    UART.write(MQTTQOS);
    
}

bool MQTT::ping(){
     packetType = PINGREQ;
     UART.write(0xC0);
     UART.write(0x00);       
}

void MQTT::loop(){  
    ping();    
    //serialEvent();
}

void MQTT::serialEvent()
{
    OUT->println("");
    OUT->println("Begin");
    int X;
    unsigned char code;
    unsigned char encodedByte;
    unsigned long multiplier = 1;
    int msglen = 0; int topiclen = 0;    
    while(UART.available())       
    {
        
        code = UART.read();                      
        
        OUT->print((char)code);
        OUT->print(",");
        OUT->print(code,HEX);        
        OUT->print(",");
        OUT->println(code,DEC);        
        
        if(code==0x10){             //CONNECT packet
            while(UART.available())
            {
                code=UART.read();
                OUT->print((char)code);
            }
        }
        
        if(code==0x20)              //CONNACK
        {
            code=UART.read();    //Byte2 of CONNACK
            code=UART.read();    //Byte3 of CONNACK            
            code=UART.read();    //Byte4 of CONNACK - Return Code
            if(code==0x00)          //Successfully Connected to MQTT broker
            {
                _isConnected = true;
                OUT->println("Successfully Connected to MQTT broker");                  
            }
            else
            {
                if(code==0x01)
                    OUT->println("Connection Refused, unacceptable protocol version");
                if(code==0x02)
                    OUT->println("Connection Refused, identifier rejected");
                if(code==0x03)
                    OUT->println("Connection Refused, Server unavailable"); 
                if(code==0x04)
                    OUT->println("Connection Refused, bad user name or password");        
                if(code==0x05)
                    OUT->println("Connection Refused, not authorized");        
                        
                _isConnected = false;
                OUT->print("Unable to connect to MQTT broker");     
                OUT->print(", CONNACK received :");
                OUT->print(code,HEX); 
            }
        }
        
        if(code==0x90)         //SUBACK
        {
            code=UART.read();    //Byte2 of SUBACK
            code=UART.read();    //Byte3 of SUBACK                                    
            code=UART.read();    //Byte4 of SUBACK
            code=UART.read();    //Byte5 of SUBACK
            if(code==0x00 || code==0x01 || code==0x02)
                OUT->println("Successfully subscribed..");
            else    // When return is 0x80
                OUT->println("Unable to subscribe..");        
        }        
        else if(code==0xD0 || code==0xC0){
            if(code==0xD0){
                code=UART.read();
                if(code==0x00)            
                    _isConnected=true;
                else
                    _isConnected=false;
            }            
        }
        else if(code==0x50){
            while(UART.available()){
                OUT->print((char)UART.read());
            }
        }
        else if(code==0x30 ) //Message Received
        {            
            OUT->println("Message Begin...");
            
            do{
                encodedByte = UART.read();     //Get the Length of the message                
                msglen = msglen + ((encodedByte & 127) * multiplier);                
                multiplier = multiplier * 128;                  
                if(multiplier > 2097152)        // 2097152 = 128 * 128 * 128
                {
                    OUT->println("Exception while calculating the Messge Length");
                    break;                    
                }                                   
            }            
            while((encodedByte & 128)!=0);
            
            code = UART.read(); // Topic Len MSB
            //OUT->print((char)code);OUT->print(",");OUT->println(code,HEX);        
            
            code = UART.read(); // Topic Len LSB
            //OUT->print((char)code);OUT->print(",");OUT->println(code,HEX);        
            topiclen = code;
            OUT->print("Msg Length:"); OUT->println(msglen,DEC);
            OUT->print("Topic Length:"); OUT->println(topiclen,DEC);
            OUT->print("Topic : ");            
            uint8_t i = 0;
            for(i=0;i<topiclen;i++){
                OUT->print((char)UART.read());
            }
            
            OUT->print("\nMessage :");
            
            int count = 0;            
            while(UART.available()){                
                code = UART.read();
                OUT->print((char)code);
                count++;                
            }                       
            OUT->print("\nMessage Length = "); OUT->print(count); 
            OUT->println("\nMessage End...");            
        }
        else if(code==0x32)
        {
            OUT->println("Message packet begin");
            while(UART.available()){
                code=UART.read(); 
                OUT->print(code);
                OUT->print(",");
                OUT->print(code,HEX);
                OUT->print(",");
                OUT->println((char)code);
            }
            OUT->println("Message packet end");
        }
        else if(code==0x34)
        {
            OUT->println("Message packet begin");
            while(UART.available()){
                code=UART.read(); 
                OUT->print(code);
                OUT->print(",");
                OUT->print(code,HEX);
                OUT->print(",");
                OUT->println((char)code);
            }
            OUT->println("Message packet end");
        }       
    }     
    OUT->println("End\n");
}



