#ifndef MARSHALLER_H
#define MARSHALLER_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "marshaller.h"


#define BUFFER_SIZE             536
#define OPERATION_SIZE          13
#define LITERAL_STRING_SIZE     32
#define SIZE_STR                11

#define THING_ID                1

char *marshaller(const char *operation, const char *topics, const char *message);
void unmarshaller(const char *operation, const char *topics);

#endif // MARSHALLER_H