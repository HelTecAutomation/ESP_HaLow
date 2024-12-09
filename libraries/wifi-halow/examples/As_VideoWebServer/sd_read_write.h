#ifndef __SD_READ_WRITE_H
#define __SD_READ_WRITE_H

#include "Arduino.h"
#include "FS.h"
#include "halow_SD.h"
#include "SPI.h"

#define SD_MOSI  11 //Please do not modify it. //MOSI
#define SD_CLK  15 //Please do not modify it. //CLK
#define SD_MISO   16 //Please do not modify it. //MISO
#define SD_CS   10 //Please do not modify it. //MISO

void sdmmcInit(void); 

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);

void writejpg(fs::FS &fs, const char * path, const uint8_t *buf, size_t size);
int readFileNum(fs::FS &fs, const char * dirname);

#endif
