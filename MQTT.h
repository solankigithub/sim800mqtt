///Define class for MQTT
// ######################################################################################################################
#define CONNECT     1   //Client request to connect to Server                Client          Server
#define CONNACK     2   //Connect Acknowledgment                             Server/Client   Server/Client
#define PUBLISH     3   //Publish message                                    Server/Client   Server/Client
#define PUBACK      4   //Publish Acknowledgment                             Server/Client   Server/Client
#define PUBREC      5   //Publish Received (assured delivery part 1)         Server/Client   Server/Client
#define PUBREL      6   //Publish Release (assured delivery part 2)          Server/Client   Server/Client
#define PUBCOMP     7   //Publish Complete (assured delivery part 3)         Server/Client   Server/Client
#define SUBSCRIBE   8   //Client Subscribe request                           Client          Server
#define SUBACK      9   //Subscribe Acknowledgment                           Server          Client
#define UNSUBSCRIBE 10  //Client Unsubscribe request                         Client          Server
#define UNSUBACK    11  //Unsubscribe Acknowledgment                         Server          Client
#define PINGREQ     12  //PING Request                                       Client          Server
#define PINGRESP    13  //PING Response                                      Server          Client
#define DISCONNECT  14  //Client is Disconnecting                            Client          Server

#include "SIM8000.h"
class MQTT
{
    private:
        unsigned short MQTTProtocolNameLength;
        unsigned short MQTTClientIDLength;
        unsigned short MQTTUsernameLength;
        unsigned short MQTTPasswordLength;
        unsigned short MQTTTopicLength;     

        const char * MQTTProtocolName = "MQIsdp";
        const char MQTTLVL = 0x03;
        const char MQTTFlags = 0xC2;
        const unsigned int MQTTKeepAlive = 60;

        const char MQTTQOS = 0x00;        
        SIM8000 sim800; 
        uint8_t packetType=0;
        
    public:   
        MQTT(SIM8000 sim800);     
        bool connect(const char* MQTTClientID, const char* MQTTUsername, const char* MQTTPassword);
        bool publish(char* MQTTTopic, char* MQTTMessage);
        bool subscribe(char* MQTTTopic);
        bool MQTT::ping();
        void loop();
        void serialEvent();
        
};

