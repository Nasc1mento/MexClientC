#ifndef APP_CONST_H
#define APP_CONST_H

#include <stdint.h>

struct parameters {
    unsigned short int fluid_volume;
    unsigned short int reservoir_capacity;
    unsigned short int temperature;
    unsigned short int battery;
    unsigned short int distance;
};



#define BATTERY                         "100"
#define TEMPERATURE                     "30"



#define RECTANGLE                       1 
#define CILINDER                        0    

#define DEEP_HEIGHT                     100 // [cm]

// #define RECTANGLE_HEIGHT   1
#define RECTANGLE_HEIGHT                20
#define RECTANGLE_WIDTH                 10
#define RECTANGLE_LENGTH                10

// #define CILINDER DIAMETER               1
#define CYLINDER_RADIUS                 100
// #define CYLINDER_HEIGHT                 1
#define PI                              3.1415927

#define MINUMUM_SENSOOR_RANGE           2 // [cm]
#define MAXIMUM_SENSOOR_RANGE           400 // [cm]


#endif // APP_CONST_H