#ifndef __MEMORY_H
#define __MEMORY_H
#include "main.h"

#define BOOTLOADER_PART1_START_ADDR (uint32_t)0x08000000
#define BOOTLOADER_PART1_SIZE       (uint32_t)0x8000

#define FIRST_PAGE_PROPERTY_ADDR    (uint32_t)0x08008000
#define FIRST_PAGE_PROPERTY_SIZE    (uint32_t)0x4000

#define SECOND_PAGE_PROPERTY_ADDR   (uint32_t)0x0800C000
#define SECOND_PAGE_PROPERTY_SIZE   (uint32_t)0x4000

#define BOOTLOADER_PART2_START_ADDR (uint32_t)0x08010000
#define BOOTLOADER_PART2_SIZE       (uint32_t)0x10000

#define MAIN_APP_START_ADDR         (uint32_t)0x08020000
#define MAIN_APP_SIZE               (uint32_t)0xA0000/*0x80000*/

/*#define UPDATER_PROGRAM_START_ADDR  (uint32_t)0x080A0000
#define UPDATER_PROGRAM_SIZE        (uint32_t)0x08020000*/

#define FILE_SYSTEM_START_ADDR      (uint32_t)0x080C0000
#define FILE_SYSTEM_SIZE            (uint32_t)0x08040000


#define RUN_MAIN_APP_DEFAULT_MAGIC_FLAG    (uint32_t)0xFFFFFFFF
#define RUN_MAIN_APP_MAGIC_FLAG            (uint32_t)0x00001A58
#define RUN_BOOTLOADER_MAGIC_FLAG          (uint32_t)0x00002A58
#define RUN_MAIN_APP_WITH_DELAY_MAGIC_FLAG (uint32_t)0x00004A58
/*typedef struct{
	uint32_t MagicFlag;//0x00001A58 - запуск приложения из загрузчика,0x00002A58 - запуск загрузчика из приложения,0x00003A58 - запуск приложения из загрузчика,0x00004A58 - запуск приложения из загрузчика с задержкой 3 секунды
}CommonData;*/

typedef struct{
	uint8_t ipAddr[4];//="192.168.4.251";
	uint8_t netMask[4];//="255.255.255.0";
	uint8_t gateWay[4];//="192.168.4.1";
  uint16_t modbusPort;//=502;
	uint16_t reserve;//uint16_t universalProtocolPort;//=10000;
}NetworkParameters;//Настройки сети

typedef struct{
	uint32_t magicFlag;
	NetworkParameters networkParameters;
	//UpdateProperty updateProperty;
}Memory;

uint16_t calculateMainAppCRC(void);
void writeMainAppCRC(uint16_t crc);

//uint16_t calculateMainAppCRCTmp(void);

void Memory_Init(void);
void memoryWriteNetworkParameters(NetworkParameters* networkParameters);
void memoryWriteMagicFlag(uint32_t magicFlag);
uint8_t isSectorErased(uint32_t address,uint32_t* pageStartAddress, uint8_t* errorCode, uint8_t reset);
HAL_StatusTypeDef eraseSector(uint32_t address);
/*void memoryWriteHoldingRegisters(uint16_t* registers,uint16_t startAddr, uint16_t regCount);*/

/*typedef struct{
  uint32_t rezerved_1;// 0
	uint32_t start_magic_flag;// 1 кодовое слово для запуска загрузчика/приложения
	uint32_t boot_mode_flags;// 2 режим загрузчика
	uint8_t lit_addr;// 3 адрес литеры устройства
	uint8_t chan_addr;// адрес канала устройства
	uint16_t rezerved_2;
	uint8_t boot_ver[2];// 4 версия загрузчика
	uint16_t boot_build;// номер билда загрузчика

	// 5 сетевые параметры
	uint8_t ip_addr[4];	// 5
	uint8_t ip_mask[4];	// 6
	uint8_t ip_gate[4];	// 7
	uint32_t ip_port;	// 8

	uint8_t mac_addr[6];//9,10
	//
	uint16_t rezerved_3;
	// 11..15
	uint32_t unused_11;
	uint32_t unused_12;
	uint32_t unused_13;
	uint32_t unused_14;
	uint32_t unused_15; // 15
}boot_ram_info_t;

typedef struct{
	char Descript_ID[7];	// “SOFT_VER”
	uint8_t Descript_ver;	// Номер версии описания (текущая версия #2)
	char MC_type[16];		// Тип микроконтроллера, для которого предназначена прошивка
	char Device_type[16];	// Название устройства, для которого предназначено ПО
	char Soft_type[16];		// Назначение ПО
	uint8_t Soft_ver[2];	// Версия ПО
	uint16_t Soft_build;	// Номер билда ПО
	char Comp_date[16];		// Дата компиляции ПО
	char Comp_time[16];		// Время компиляции ПО
	uint32_t Start_addr;	// Абсолютный адрес расположения ПО
}soft_descript_v2;//Хранится в любом месте основной программы
	//Структура дескриптора устройства
const soft_descript_v2 soft_descript =
{
	"SOFTVER",			//uint8_t Descript_ID[7];	// “SOFT_VER”
	2,					//uint8_t Descript_ver;		// Номер версии описания (текущая версия #2)
	"STM32",			//uint8_t MC_type[16];		// Тип микроконтроллера, для которого предназначена прошивка
	"Satcom_ROTATOR", 			//uint8_t Device_type[16];	// Название устройства, для которого предназначено ПО
	"application",		//uint8_t Soft_type[16];	// Название/тип ПО
	{1,0},				//uint8_t Soft_ver[2];		// Версия ПО
	1,					//uint16_t Soft_build;		// Номер билда ПО
	__DATE__,			//uint8_t Comp_date[16];//12	// Дата компиляции ПО
	__TIME__,			//uint8_t Comp_time[16];//8		// Время компиляции ПО
	0x08020000			//uint32_t Start_addr;		// Абсолютный адрес расположения ПО в памяти микроконтроллера
};*/

#endif

