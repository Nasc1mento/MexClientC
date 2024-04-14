#include <ESP8266WiFi.h>

#include "Mex.h"

Mex::Mex(IPAddress ip, uint16_t port, uint8_t thing_id) {
    _ip = ip.toString().c_str();
    _port = port;
    _thing_id = thing_id;
    _client = new WiFiClient();
}

Mex::Mex(const char *ip, uint16_t port, uint8_t thing_id) {
    _ip = ip;
    _port = port;
    _thing_id = thing_id;
    _client = new WiFiClient();
}


uint8_t Mex::connect() {
    if (!_client->connect(_ip, _port)) {
        Serial.println("Connection failed");
        return -1;
    }
    
    return MEX_CONNECTED;
}

uint8_t Mex::send(const uint8_t *payload, size_t size) {
    Serial.println("Sending");
    char size_str[11];
    Serial.print("Size: ");
    Serial.println(size);
    snprintf(size_str, 11, "%d", size);

    while (strlen(size_str) < 10) {
        memmove(size_str + 1, size_str, strlen(size_str) + 1);
        size_str[0] = '0';
    }

    size_t ret;

   ret = _client->write((const uint8_t*)size_str, 10);

    if (ret != 10) {
        Serial.println("Error sending size");
        return -1;
    }

   ret = _client->write(payload, size);

    if (ret != size) {
        Serial.println("Error sending payload");
        return -1;
    }

    Serial.println("Sent");
    
    return 0;
}

uint8_t Mex::receive() {
    
    uint16_t num_bytes_read = 0;
    char chunk[BUFFER_SIZE];

    // for (;;) {
        while (_client->available() > 0) {
            char c = _client->read();
            chunk[num_bytes_read++] = c;

            if (num_bytes_read < BUFFER_SIZE) {
                chunk[num_bytes_read] = '\0';
                break;
            }

            num_bytes_read = 0;
        }
    // }
    return 0;
}

uint8_t Mex::publish(String topic, String message) {
    return this->publish(topic.c_str(), message.c_str());
}

uint8_t Mex::publish(const char *topic, const char *message) {
    Serial.println("Publishing");
    char operation[OPERATION_SIZE] = "Publish";
    char *payload = _marshaller(operation, topic, message);
    uint8_t ret =  this->send((const uint8_t*)payload, strlen(payload));
    free(payload);
    return ret;
}

uint8_t Mex::subscribe(String topics) {
    return subscribe(topics.c_str());
}

uint8_t Mex::subscribe(const char *topics) {
    char operation[OPERATION_SIZE] = "Subscribe";
    char* payload = _marshaller(operation, topics, "");
    uint8_t ret = this->send((const uint8_t*)payload, strlen(payload));
    free(payload);
    return ret;
}

uint8_t Mex::checkMsg(String topics) {
    return this->checkMsg(topics.c_str());
}

uint8_t Mex::checkMsg(const char *topics) {
    char operation[OPERATION_SIZE] = "CheckMsg";
    char *payload = _marshaller(operation, topics, "");
    uint8_t ret = this->send((const uint8_t*)payload, strlen(payload));
    free(payload);
    return ret;
    
}

uint8_t Mex::unsubscribe(String topic) {
    return this->unsubscribe(topic.c_str());
}

uint8_t Mex::unsubscribe(const char *topic) {
    char operation[OPERATION_SIZE] = "Unsubscribe";
    char *payload = _marshaller(operation, topic, "");
    uint8_t ret = this->send((const uint8_t*)payload, strlen(payload));
    free(payload);
    return ret;
}

char* Mex::_marshaller(const char *operation, const char *topics, const char *message) {
    const size_t buffer_size = strlen(topics) + strlen(message) + OPERATION_SIZE + LITERAL_STRING_SIZE;
    char *payload = (char*)malloc(buffer_size);

    snprintf(payload, buffer_size, 
    "OP:'%s'\nTHING_ID:'%d'\nTOPICS:['%s']\n%s\n",
    operation, _thing_id, topics, message);

    return payload;
}

void Mex::_unmarshaller(char *operation, const char *topics, const char *message) {}

