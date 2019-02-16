#include "SD.h"
#include <SPI.h>
#include <vector>
#include <string>
namespace nustars {
    class Storage {
    public:
        Storage(String file);
        bool write(String msg);
        int* read(int& lng);
        void save();

    private:
        bool isInitialized;
        String fileName;
        File dataFile;
        std::vector<String > buffer;
    };
}
