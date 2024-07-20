
#include "marshaller.h"

char *marshaller(const char *operation, const char *topics, const char *message, char *payload, size_t size) {
    snprintf(payload, size, 
    "OP:'%s'\nTHING_ID:'%s'\nTOPICS:['%s']\n\n%s",
    operation, THING_ID, topics, message);

    return payload;
}

void unmarshaller(const char *operation, const char *topics, char *buffer) {
    

} 
