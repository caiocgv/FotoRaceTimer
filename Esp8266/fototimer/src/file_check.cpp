#include <Arduino.h>
#include <LittleFS.h>

int i = 0;

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }

    // List all files
    Serial.println("Listing files:");
    File root = LittleFS.open("/", "r");
    File file = root.openNextFile();
    while (file) {
        Serial.print("File: ");
        Serial.println(file.name());
        file = root.openNextFile();
        i++;
    }
    if (i == 0) {
        Serial.println("No files found");
    } else {
        Serial.println("Files listed successfully");
    }
}

void loop() {
    // Do nothing
}