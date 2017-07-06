#include "swr_eeprom.h"

#include <EEPROM.h>

#define EEPROM_CRC_ADDR 0
#define EEPROM_CRC_LENGTH 4
#define EEPROM_SIZE_ADDR (EEPROM_CRC_ADDR + EEPROM_CRC_LENGTH)
#define EEPROM_SIZE_LENGTH 4
#define EEPROM_DATA_ADDR (EEPROM_SIZE_ADDR + EEPROM_SIZE_LENGTH)

struct SwrPersistedData {
  boolean calibrateOnBoot;
};

SwrPersistedData* persistedData = NULL;

uint32_t crc32(byte *data, int len) {
  const uint32_t crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  uint32_t crc = ~0L;

  //skip first 4 bytes which contain the CRC itself
  for (int index ; index < len  ; ++index) {
    crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  
  return crc;
}

uint32_t eepromCrc32() {
  SwrPersistedData eepromData;
  EEPROM.get(EEPROM_DATA_ADDR, eepromData);
  return crc32((byte*) &eepromData, sizeof(eepromData));
}

uint32_t persistedDataCrc32() {
  byte *dataAsArray = (byte*) persistedData;
  int dataSize = sizeof(*persistedData);
  return crc32(dataAsArray, dataSize);
}

boolean checkEepromCrc() {
  uint32_t storedCrc;
  EEPROM.get(EEPROM_CRC_ADDR, storedCrc);
  uint32_t calculatedCrc = eepromCrc32();
  if( storedCrc == calculatedCrc )
    return true;
  else
    return false;
}

boolean storeData() {
  uint32_t crc = persistedDataCrc32();
  EEPROM.put(EEPROM_CRC_ADDR, crc);
  EEPROM.put(EEPROM_SIZE_ADDR, sizeof(*persistedData));
  EEPROM.put(EEPROM_DATA_ADDR, *persistedData);

  return crc == eepromCrc32();
}

boolean recallData() {
  if( persistedData != NULL ) {
    free(persistedData);
    persistedData = NULL;
  }

  uint32_t dataSize;
  EEPROM.get(EEPROM_SIZE_ADDR, dataSize);
  persistedData = malloc(dataSize);
  EEPROM.get(EEPROM_DATA_ADDR, *persistedData);

  return checkEepromCrc();
}


boolean calibrateOnBoot() {
  return persistedData->calibrateOnBoot;
}

void activateCalibrateOnBoot() {
  if( persistedData->calibrateOnBoot == true )
    return;

  persistedData->calibrateOnBoot = true;
  storeData();
}

void resetCalibrateOnBoot() {
  if( persistedData->calibrateOnBoot == false )
    return;

  persistedData->calibrateOnBoot = false;
  storeData();
}

void eepromSetup() {
  recallData();
}

