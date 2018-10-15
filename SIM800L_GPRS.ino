#include "MQTT.h"

SIM8000 sim800;
MQTT mqtt(sim800);

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    //Serial.print("Buffer Size TX:");Serial.print(SERIAL_TX_BUFFER_SIZE);
    //Serial.print("Buffer Size RX:");Serial.print(SERIAL_RX_BUFFER_SIZE);
    if(sim800.initialize(1)){
        if(!sim800.startTCP("m12.cloudmqtt.com",13079))
        //if(!sim800.startTCP("broker.mqttdashboard.com",1883))
            Serial.println("Unable to start TCP Connection...");
    }
    
}

void loop()
{
    //mqtt.connect("MQTT1234", "","");
    mqtt.connect("MQTT1234", "xecjjukx","rs5_nqgr4ALw");
    mqtt.subscribe("arduinotopic");    

    while(1)
    {
        mqtt.publish("sandeep", "Message from Arduino");
        //Serial.println(sim800.getTCPStatus());    
        
        mqtt.loop();
        delay(1000);    
    }
    
}

void serialEvent1(){
    mqtt.serialEvent();
}


