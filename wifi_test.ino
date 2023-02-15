#include <SPI.h>
#include <WiFiNINA.h>

#include "net_util.h"

#ifndef FUTURES
#include "futures.h"
#endif

#ifndef CAMERA
#include "camera.h"
#endif

future google_fut;

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
    Serial.println("Initializing camera...");
    init_camera();

    Serial.println("\n\n---- HALT ----");
    spin_forever();
}

void loop() {
    // delay(1000);
    // Serial.println("    Alive");

    // Handle updates on any in-progress HTTP Jobs
    // http_get_wait_loop();

    // if (google_fut.done) {
    //     Serial.println("Response received.");
    //     Serial.println((char*) google_fut.value);
        
    //     spin_forever();
    // }
}

void spin_forever() {
    while (true) {
        ;
    }
}

