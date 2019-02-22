#include "SD.h"
#include <SPI.h>

namespace nustars {
    class Storage {
    public:
        Storage(char* file);
        bool write(char* msg);
        int* read(int& lng);
        void save();

    private:
        bool isInitialized;
        char* fileName;
        File dataFile;
    };
}