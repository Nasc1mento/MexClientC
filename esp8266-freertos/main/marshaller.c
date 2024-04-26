
#include "marshaller.h"

char *marshaller(const char *operation, const char *topics, const char *message) {
    const size_t buffer_size = strlen(topics) + strlen(message) + OPERATION_SIZE + LITERAL_STRING_SIZE;
    char *payload = (char*)malloc(buffer_size);

    snprintf(payload, buffer_size, 
    "OP:'%s'\nTHING_ID:'%d'\nTOPICS:['%s']\n\n%s",
    operation, THING_ID, topics, message);

    return payload;
}

void unmarshaller(const char *operation, const char *topics) {} 
