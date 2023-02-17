#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduCAM.h>

#include "net_util.h"

#ifndef FUTURES
#include "futures.h"
#endif

#ifndef CAMERA
#include "camera.h"
#endif

future test_fut;

void setup() {
    // Initialize serial and wait for port to open:
    Serial.begin(9600);

    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }

    // Connect to wifi
    // net_setup();
    // print_wifi_status();

    // Connect to the camera
    Serial.println(F("Initializing camera..."));
    ArduCAM* myCAM = init_camera();

    // Capture a frame
    Serial.println(F("Capturing frame..."));
    size_t length;
    uint8_t* frame = capture_frame(myCAM, &length);

    // Send the frame to my EC2 server at ec2-3-101-29-170.us-west-1.compute.amazonaws.com
    Serial.println(F("Sending frame..."));
    // test_fut = http_get("ec2-3-101-29-170.us-west-1.compute.amazonaws.com", 6969, "/test");

    Serial.println("\n\n---- HALT ----");
    // spin_forever();
    spin_forever();
}

void loop() {
    //     init_camera();
    // delay(1000);
    // Serial.println("    Alive");

    // Handle updates on any in-progress HTTP Jobs
    // http_get_wait_loop();

    // if (test_fut.done) {
    //     Serial.println("Response received.");
    //     Serial.println((char*) test_fut.value);
        
    //     Serial.println("\n\n---- HALT ----");
    //     spin_forever();
    // }
}

void spin_forever() {
    while (true) {
        ;
    }
}

