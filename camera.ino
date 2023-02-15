#ifndef CAMERA
#include "camera.h"
#endif

#include <SPI.h>
#include <Wire.h>
#include <ArduCAM.h>

const int SLAVE_SELECT_PIN = 7;

void init_camera() {
    test_i2c();

    ArduCAM myCAM(OV5642, SLAVE_SELECT_PIN);


    Serial.println("Setting the SLAVE_SELECT_PIN to HIGH");
    pinMode(SLAVE_SELECT_PIN, OUTPUT);
    // set the slave select pin high
    digitalWrite(SLAVE_SELECT_PIN, HIGH);

    SPI.setDataMode(0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.begin();

    delay(1000);

    // Write 1 to register 7 to reset to CPLD
    Serial.println("Writing 0x80 to reg 0x07");
    myCAM.write_reg(0x07, 0x80);

    int seven = myCAM.read_reg(0x07);
    Serial.println("Read 0x07: ");


    delay(1000);
    Serial.println("Writing 0x00 to reg 0x07");
    myCAM.write_reg(0x07, 0x00);
    delay(1000);

    // Write to reg 0 and read it back
    Serial.println("Writing 0x55 to reg 0x00");
    myCAM.write_reg(0x00, 0x55);
    delay(1000);

    Serial.println("Reading from reg 0x00");
    uint8_t result = myCAM.read_reg(0x00);

    Serial.println("Result: ");
    Serial.println(result);
}

void test_i2c() {
    // Initialize I2C and then print out all the devices
    Wire.begin();
    Serial.println("Scanning for I2C devices...");

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



void reg_write(uint8_t address, uint8_t value) {
    // take the SS pin low to select the chip:
    digitalWrite(SLAVE_SELECT_PIN, LOW);
    // delay(100);
    //  send in the address and value via SPI:
    SPI.transfer(address);
    SPI.transfer(value);
    // delay(100);
    // take the SS pin high to de-select the chip:
    digitalWrite(SLAVE_SELECT_PIN, HIGH);
}

uint8_t reg_read(uint8_t address) {
    digitalWrite(SLAVE_SELECT_PIN, LOW);
    // delay(100);

    uint8_t result = SPI.transfer(0);
    // delay(100);

    digitalWrite(SLAVE_SELECT_PIN, HIGH);

    return result;
}
