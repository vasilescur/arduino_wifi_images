#ifndef CAMERA
#define CAMERA
#endif

#include <ArduCAM.h>

ArduCAM* init_camera();
uint8_t* capture_frame(ArduCAM* myCAM, size_t* length);
