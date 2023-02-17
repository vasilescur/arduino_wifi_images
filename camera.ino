#ifndef CAMERA
#include "camera.h"
#endif

#include <ArduCAM.h>
#include <SPI.h>
#include <Wire.h>

// --- Constants & Macros ---

const int CS = 7;

#define CAMERA_DEBUG

#ifdef CAMERA_DEBUG
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif

// --- Function prototypes ---

bool test_spi(ArduCAM* myCAM);

#ifdef CAMERA_DEBUG
void test_i2c();
bool test_sensor(ArduCAM *myCAM);
#endif

// --- Init ---

ArduCAM* init_camera() {
    #ifdef CAMERA_DEBUG
    test_i2c();
    #endif;

    ArduCAM* myCAM = new ArduCAM(OV5642, CS);

    // Init SPI
    DEBUG(F("Initializing SPI"));
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    SPI.begin();

    // Reset the CPLD
    DEBUG(F("Resetting CPLD"));
    myCAM->write_reg(0x07, 0x80);
    delay(100);
    myCAM->write_reg(0x07, 0x00);
    delay(100);

    // Check the IC2 interface
    #ifdef CAMERA_DEBUG
    bool spi_ok = test_spi(myCAM);
    if (!spi_ok) {
        DEBUG(F("SPI interface error! Aborting"));
        return NULL;
    }
    #endif

    // Check if the camera module type is OV5642
    #ifdef CAMERA_DEBUG
    bool sensor_ok = test_sensor(myCAM);
    if (!sensor_ok) {
        DEBUG(F("Sensor not ok. Aborting"));
        return NULL;
    }
    #endif

    // Change to JPEG capture mode and initialize the OV5642 module
    DEBUG(F("Initializing camera in RAW format"));
    myCAM->set_format(RAW);
    myCAM->InitCAM();

    // Set the frame size
    DEBUG(F("Setting frame size"));
    myCAM->write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);  // VSYNC is active HIGH
    myCAM->OV5642_set_RAW_size(OV5642_320x240);

    myCAM->clear_fifo_flag();
    myCAM->write_reg(ARDUCHIP_FRAMES, 0x00);

    return myCAM;
}

// --- Utility Functions ---

bool test_spi(ArduCAM* myCAM) {
    myCAM->write_reg(ARDUCHIP_TEST1, 0x55);
    uint8_t temp = myCAM->read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
        DEBUG(F("SPI interface error!"));
        return false;
    } else {
        DEBUG(F("SPI interface ok."));
        return true;
    }
}

uint8_t* capture_frame(ArduCAM* myCAM, size_t* length) {
    myCAM->flush_fifo();
    myCAM->clear_fifo_flag();
    myCAM->write_reg(ARDUCHIP_FRAMES, 0x00);

    DEBUG(F("Starting capture"));
    myCAM->start_capture();

    while (!myCAM->get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        delay(10);
    }

    DEBUG(F("Capture done"));
    delay(50);

    uint32_t fifo_length = myCAM->read_fifo_length();
    DEBUG(F("FIFO length: "));
    Serial.println(fifo_length);

    // Serial.print(F("JPEGSTART"));

    uint8_t* buf = (uint8_t*) malloc(fifo_length);

    for (int i = 0; i < fifo_length; i++) { 
        uint8_t data = myCAM->read_fifo();
        // Serial.write(data);
        buf[i] = data;

        // #ifdef CAMERA_DEBUG
        //     if (buf[i] < 0x10) {
        //         Serial.print("0");
        //     }
        //     Serial.print(buf[i], HEX);
        //     Serial.print(" ");
        //     if (i % 16 == 15) Serial.println();
        // #else
        //     // Serial.write(buf[i]);
        //     if (buf[i] < 0x10) {
        //         Serial.print("0");
        //     }
        //     Serial.print(buf[i], HEX);
        // #endif
    }

    // Serial.print(F("JPEGEND"));

    // free(buf);
    // Serial.println(F("$$$$$$$$$$"));

    *length = fifo_length;
    return buf;
}

// --- Debug Functions ---

#ifdef CAMERA_DEBUG
void test_i2c() {
    // Initialize I2C and then print out all the devices
    Wire.begin();
    Serial.println("Scanning for I2C devices");

    int nDevices = 0;
    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);

            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }
}
#endif

#ifdef CAMERA_DEBUG
bool test_sensor(ArduCAM *myCAM) {
    DEBUG(F("Checking camera module type"));

    uint8_t vid, pid;
    myCAM->wrSensorReg16_8(0xff, 0x01);
    myCAM->rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM->rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);

    if ((vid != 0x56) || (pid != 0x42)) {
        DEBUG(F("ACK CMD Can't find OV5642 module! END"));
        return false;
    } else {
        DEBUG(F("ACK CMD OV5642 detected. END"));
        return true;
    }
}
#endif

