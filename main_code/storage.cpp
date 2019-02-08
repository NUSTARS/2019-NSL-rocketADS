#include "storage.h"


namespace nustars {
    Storage::Storage(char* x) {
        isInitialized = false;
        fileName = (char*)malloc(strlen(x) + 1);
        strcpy(fileName, x);
        if (!SD.begin(BUILTIN_SDCARD)) {
            Serial.println("Card failed, or not present");
            while (1) 
              delay(10);
            return;
        }
        isInitialized = true;
        Serial.println(isInitialized);
    }

    void Storage::write(char* msg) {
        File dataFile = SD.open(fileName, FILE_WRITE); //
        if (dataFile) {
            dataFile.println(msg);
            dataFile.close();
        }
        else Serial.println("Didn't make it ma!");
    }
}
