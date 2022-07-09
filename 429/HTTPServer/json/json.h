#ifndef __JSON_H
#define __JSON_H
#include "stm32f4xx_hal.h"
#include "Memory.h"

//void DecodeURL(char *urlBuf,uint16_t* urlCursor,char* jsonBuf,uint16_t* jsonCursor);
void DecodeURL(char *urlBuf,uint16_t* urlCursor,char* jsonBuf,uint16_t* jsonCursor, uint16_t inputDataLenght);
void jsonStrToNetParameters(char* jsonStr, uint16_t jsonStrLenght, NetworkParameters* networkParameters);
//void jsonStrToModbusParameters(char* jsonStr, uint16_t jsonStrLenght,ModbusParameters* modbusParameters);



#endif


