#ifndef MARSHALLER_H
#define MARSHALLER_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "marshaller.h"


#define BUFFER_SIZE             536
#define OPERATION_SIZE          13
#define LITERAL_STRING_SIZE     32
#define SIZE_STR                11



uint8_t marshaller(const char *operation, const char *topics, const char *message, char *payload, size_t size);
void unmarshaller(const char *operation, const char *topics, char *buffer, const size_t len);

#endif // MARSHALLER_H