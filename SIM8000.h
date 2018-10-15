///Define class for MQTT
#define debug 
#define RESET_PIN 6


class SIM8000
{
    private:  
        int8_t mode=0;   
        int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);        
        bool isRecMode;
    public:   
    SIM8000();
    bool initialize(uint8_t modemState=1);
    bool initModem();
    bool initNetwork();
    bool initGPRS();
    bool startTCP(char * servername, int port);
    bool setMode(uint8_t mode=0); 
    bool checkModem();
    bool checkNetwork();
    bool checkGPRS();
    bool getMACAddress();
    bool getIPAddress();
    short int getMode();
    void resetModem();
    char* getTCPStatus();
    void  setAPN(char* APN, char* username, char*password);
    char* sendATCommandResponse(char* ATcommand, unsigned int timeout);
};

