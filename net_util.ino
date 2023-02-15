#ifndef NET_UTIL
#include "net_util.h"
#endif

#ifndef FUTURES
#include "futures.h"
#endif

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

HttpGetJobListNode* active_jobs = NULL;

void active_jobs_append(HttpGetJob* job) {
    HttpGetJobListNode* node = (HttpGetJobListNode*) malloc(sizeof(HttpGetJobListNode));
    node->job = job;
    node->next = NULL;

    if (active_jobs == NULL) {
        active_jobs = node;
    } else {
        HttpGetJobListNode* last = active_jobs;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = node;
    }
}

int net_setup() {
    if (!Serial) {
        return 1;
    }

    int status = WL_IDLE_STATUS;
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");

        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();

    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);

        status = WiFi.begin(ssid, pass);

        delay(10000);
    }

    Serial.println("Connected to wifi");
}

void print_wifi_status() {
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

future http_get(String hostname, int port, String path) {
    int status = WL_IDLE_STATUS;

    WiFiClient client;

    HttpGetJob* job = (HttpGetJob*) malloc(sizeof(HttpGetJob));
    job->client = &client;
    job->done = false;

    if (client.connect(hostname.c_str(), port)) {
        Serial.println("Established TCP connection on port " + port);
        Serial.println("\nGET " + path);

        client.println("GET " + path + " HTTP/1.1");
        client.println("Host: " + hostname);
        client.println("Connection: close");
        client.println();
    }

    future result;
    result.value = job;
    result.done = false;

    job->future = &result;

    active_jobs_append(job);

    return result;
}

void cleanup_finished_jobs() {
    if (active_jobs == NULL) {
        return;
    }

    // use a for loop like below to iterate thru the jobs list
    for (HttpGetJobListNode* node = active_jobs; node != NULL; node = node->next) {
        HttpGetJob* job = node->job;
        if (job->done) {
            // remove the job from the list
            if (node == active_jobs) {
                active_jobs = node->next;
            } else {
                HttpGetJobListNode* prev = active_jobs;
                while (prev->next != node) {
                    prev = prev->next;
                }
                prev->next = node->next;
            }

            // free the memory
            free(node);

            // resolve the future
            job->future->done = true;

            Serial.println("Job finished, resolving future.");
        }
    }
    
}

void http_get_wait_loop() {
    if (active_jobs == NULL) {
        return;
    }

    for (HttpGetJobListNode* node = active_jobs; node != NULL; node = node->next) {
        HttpGetJob* job = node->job;
        WiFiClient* client = job->client;

        while (client->available()) {
            char c = client->read();
            job->response_buf[job->response_buf_size++] = c;
            Serial.write(c);
        }

        if (!client->connected()) {
            Serial.println();
            Serial.println("Disconnected from server.");
            client->stop();

            job->done = true;
        }
    }

    cleanup_finished_jobs();
}


