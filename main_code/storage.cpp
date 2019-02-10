#include "storage.h"


namespace nustars {
Storage::Storage(char* x) {
  isInitialized = false;
  //fileName = (char*)malloc(strlen(x) + 1);
  //strcpy(fileName, x);
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("Card failed, or not present");
    while (1)
      delay(10);
    return;
  }
  String outputString;
  char outputFile[100];
  outputString = "data0.csv";
  outputString.toCharArray(outputFile, 100);
  int i = 0;
  while (SD.exists(outputFile)) {
    i++;
    outputString = "data";
    outputString += String(i);
    outputString += ".csv";
    outputString.toCharArray(outputFile, 100);
  }
  fileName = (char*) malloc(strlen(outputFile));
  strcpy(fileName, outputFile);
  isInitialized = true;
  Serial.println(isInitialized);
  dataFile = SD.open(fileName, FILE_WRITE);

}

bool Storage::write(char* msg) {
  //

  if (dataFile) {
    dataFile.println(msg);
    return true;
  }
  else return false;

}

void Storage::save() {
  dataFile.close();
  dataFile = SD.open(fileName, FILE_WRITE);
}
}
