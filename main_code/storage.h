#include "SD.h"
#include <SPI.h>
#include <vector>
#include <string>
namespace nustars {
    class Storage {
    public:
        Storage(std::string file);
        bool write(std::string msg);
        int* read(int& lng);
        void save();

    private:
        bool isInitialized;
        std::string fileName;
        File dataFile;
        std::vector<std::string > buffer;
    };
}
