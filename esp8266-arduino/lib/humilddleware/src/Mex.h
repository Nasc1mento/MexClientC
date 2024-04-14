
#ifndef MEX_H
#define MEX_H

#include <Arduino.h>
#include <IPAddress.h>
#include <Client.h>

#define MEX_CONNECTED           0

#define BUFFER_SIZE             536
#define OPERATION_SIZE          13
#define LITERAL_STRING_SIZE     32

class Mex{
    
private:

    Client* _client;
    char *_buffer;
    const char *_ip;
    uint16_t _port;
    uint8_t _thing_id;

    uint8_t send(const uint8_t *payload, size_t size);
    uint8_t receive();

    char* _marshaller(const char *operation, const char *topic, const char *message);
    void _unmarshaller(char *operation, const char *topic, const char *message);
    
public:
    Mex (IPAddress ip, uint16_t port, uint8_t thing_id);
    Mex (const char *ip, uint16_t port, uint8_t thing_id);
    uint8_t connect();
    uint8_t publish(const char *topic, const char* message);
    uint8_t publish(String topic, String message);
    uint8_t subscribe(const char *topics);
    uint8_t subscribe(String topics);
    uint8_t unsubscribe(const char *topic);
    uint8_t unsubscribe(String topic);
    uint8_t checkMsg(String topics);
    uint8_t checkMsg(const char *topics);
};

#endif