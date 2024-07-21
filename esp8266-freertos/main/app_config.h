#ifndef APP_CONST_H
#define APP_CONST_H

#include <stdint.h>

#define TOPIC                           "water-level"

#define PUBLISH_TEMPLATE                "{'distance': %d, 'battery': %d, 'timer': '%s'}"
#define ADAPTER_TEMPLATE                "fluid_volume:%d\nreservoir_capacity:%d\ntemperature:%d\nbattery:%d"   

#define RECTANGLE_HEIGHT                20
#define RECTANGLE_WIDTH                 50
#define RECTANGLE_LENGTH                10

#define BATTERY                         100
#define TEMPERATURE                     30
#define RESERVOIR_CAPACITY()            (RECTANGLE_HEIGHT * RECTANGLE_WIDTH * RECTANGLE_LENGTH)
#define FLUID_VOLUME(d)                 (d * RECTANGLE_WIDTH * RECTANGLE_LENGTH)

#endif // APP_CONST_H