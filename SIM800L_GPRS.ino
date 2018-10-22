#include "MQTT.h"

MQTT mqtt(Serial);

unsigned long count=0;
char msg[10];
int len;

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    
    if(mqtt.initialize())
    {        
        mqtt.connect("MQTT1234", "xecjjukx","rs5_nqgr4ALw");
        mqtt.subscribe("arduinotopic"); 
        //mqtt.OUT->println("Connected & Subscribed...");
          
    }
    else{
        mqtt.OUT->println("Unable to connect to the network..");
        
    }        
}

void loop()
{    
    if(mqtt.isConnected())    
    {
        len = sprintf((char*)msg,"%u",count++);
        msg[len] = '\0';
        mqtt.publish("sandeep", msg,0);        
        
        
    }   
    mqtt.loop(); 
    delay(1000);    
}

void serialEvent1(){
    mqtt.serialEvent();
}


