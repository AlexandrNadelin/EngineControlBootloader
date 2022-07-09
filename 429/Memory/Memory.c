#include "Memory.h"

static uint16_t MODBUS_CRC16(uint8_t *buf, uint32_t len )
{
	uint16_t crc = 0xFFFF;
	unsigned int i = 0;
	char bit = 0;

	for( i = 0; i < len; i++ )
	{
		crc ^= buf[i];

		for( bit = 0; bit < 8; bit++ )
		{
			if( crc & 0x0001 )
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else crc >>= 1;
		}
	}

	return crc;
}

uint8_t isSectorErased(uint32_t address,uint32_t* pageStartAddress, uint8_t* errorCode, uint8_t reset)
{
	static uint32_t lastErasedBlock =0x00000000;
	
	if(reset)lastErasedBlock=0x00000000;
	
	if(address <MAIN_APP_START_ADDR || address >= MAIN_APP_START_ADDR + MAIN_APP_SIZE)
	{
		*errorCode=1;//Ошибка - вне диапазона
		return 1;//Чтобы случайно не стереть
	}
	if(address>=0x08000000 && address<0x08004000 && lastErasedBlock < 0x08000000){*pageStartAddress=0x08000000;lastErasedBlock=0x08000000; *errorCode=0;return 0;}
	else if(address>=0x08004000 && address<0x08008000 && lastErasedBlock < 0x08004000){*pageStartAddress=0x08004000;lastErasedBlock=0x08004000; *errorCode=0;return 0;}
	else if(address>=0x08008000 && address<0x0800C000 && lastErasedBlock < 0x08008000){*pageStartAddress=0x08008000;lastErasedBlock=0x08008000; *errorCode=0;return 0;}
	else if(address>=0x0800C000 && address<0x08010000 && lastErasedBlock < 0x0800C000){*pageStartAddress=0x0800C000;lastErasedBlock=0x0800C000; *errorCode=0;return 0;}
	else if(address>=0x08010000 && address<0x08020000 && lastErasedBlock < 0x08010000){*pageStartAddress=0x08010000;lastErasedBlock=0x08010000; *errorCode=0;return 0;}
	else if(address>=0x08020000 && address<0x08040000 && lastErasedBlock < 0x08020000){*pageStartAddress=0x08020000;lastErasedBlock=0x08020000; *errorCode=0;return 0;}
	else if(address>=0x08040000 && address<0x08060000 && lastErasedBlock < 0x08040000){*pageStartAddress=0x08040000;lastErasedBlock=0x08040000; *errorCode=0;return 0;}
	else if(address>=0x08060000 && address<0x08080000 && lastErasedBlock < 0x08060000){*pageStartAddress=0x08060000;lastErasedBlock=0x08060000; *errorCode=0;return 0;}
	else if(address>=0x08080000 && address<0x080A0000 && lastErasedBlock < 0x08080000){*pageStartAddress=0x08080000;lastErasedBlock=0x08080000; *errorCode=0;return 0;}
	else if(address>=0x080A0000 && address<0x080C0000 && lastErasedBlock < 0x080A0000){*pageStartAddress=0x080A0000;lastErasedBlock=0x080A0000; *errorCode=0;return 0;}
	else if(address>=0x080C0000 && address<0x080E0000 && lastErasedBlock < 0x080C0000){*pageStartAddress=0x080C0000;lastErasedBlock=0x080C0000; *errorCode=0;return 0;}
	else if(address>=0x080E0000 && address<0x08100000 && lastErasedBlock < 0x080E0000){*pageStartAddress=0x080E0000;lastErasedBlock=0x080E0000; *errorCode=0;return 0;}
	
	*pageStartAddress=0;
	*errorCode=0;
	return 1;//Чтобы случайно не стереть
}

HAL_StatusTypeDef eraseSector(uint32_t address)
{
	uint32_t PageErr;
	FLASH_EraseInitTypeDef EraseInitStruct;
		
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
  EraseInitStruct.Sector = address==0x08000000?FLASH_SECTOR_0:address==0x08004000?FLASH_SECTOR_1:address==0x08008000?FLASH_SECTOR_2:
                           address==0x0800C000?FLASH_SECTOR_3:address==0x08010000?FLASH_SECTOR_4:address==0x08020000?FLASH_SECTOR_5:
                           address==0x08040000?FLASH_SECTOR_6:address==0x08060000?FLASH_SECTOR_7:address==0x08080000?FLASH_SECTOR_8:
                           address==0x080A0000?FLASH_SECTOR_9:address==0x080C0000?FLASH_SECTOR_10:/*address==0x080E0000?*/FLASH_SECTOR_11;	
	EraseInitStruct.NbSectors=1;		
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	return HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
}

uint16_t calculateMainAppCRC(void)
{
	return MODBUS_CRC16((uint8_t*)MAIN_APP_START_ADDR, MAIN_APP_SIZE);
}

void writeMainAppCRC(uint16_t crc)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,MAIN_APP_START_ADDR + MAIN_APP_SIZE - 2,crc);
	HAL_FLASH_Lock();
}

void unlockAndEraseFirstPage()
{
	uint32_t PageErr;
	FLASH_EraseInitTypeDef EraseInitStruct;
			
	HAL_FLASH_Unlock();
		
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
  EraseInitStruct.Sector = FIRST_PAGE_PROPERTY_ADDR==0x08000000?FLASH_SECTOR_0:FIRST_PAGE_PROPERTY_ADDR==0x08004000?FLASH_SECTOR_1:FIRST_PAGE_PROPERTY_ADDR==0x08008000?FLASH_SECTOR_2:
                           FIRST_PAGE_PROPERTY_ADDR==0x0800C000?FLASH_SECTOR_3:FIRST_PAGE_PROPERTY_ADDR==0x08010000?FLASH_SECTOR_4:FIRST_PAGE_PROPERTY_ADDR==0x08020000?FLASH_SECTOR_5:
                           FIRST_PAGE_PROPERTY_ADDR==0x08040000?FLASH_SECTOR_6:FIRST_PAGE_PROPERTY_ADDR==0x08060000?FLASH_SECTOR_7:FIRST_PAGE_PROPERTY_ADDR==0x08080000?FLASH_SECTOR_8:
                           FIRST_PAGE_PROPERTY_ADDR==0x080A0000?FLASH_SECTOR_9:FIRST_PAGE_PROPERTY_ADDR==0x080C0000?FLASH_SECTOR_10:/*FIRST_PAGE_PROPERTY_ADDR==0x080E0000?*/FLASH_SECTOR_11;	
	EraseInitStruct.NbSectors=1;		
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
}

void writeCRCToFirstPageCopyFirstToSecondPageAndLock()
{
	uint32_t PageErr;
	FLASH_EraseInitTypeDef EraseInitStruct;
	volatile int i=0;
	
	uint16_t additionCRCPage = MODBUS_CRC16((uint8_t*)FIRST_PAGE_PROPERTY_ADDR,FIRST_PAGE_PROPERTY_SIZE-2);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FIRST_PAGE_PROPERTY_ADDR + FIRST_PAGE_PROPERTY_SIZE - 2,additionCRCPage);
	
	//-----------------------//
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
  EraseInitStruct.Sector = SECOND_PAGE_PROPERTY_ADDR==0x08000000?FLASH_SECTOR_0:SECOND_PAGE_PROPERTY_ADDR==0x08004000?FLASH_SECTOR_1:SECOND_PAGE_PROPERTY_ADDR==0x08008000?FLASH_SECTOR_2:
                           SECOND_PAGE_PROPERTY_ADDR==0x0800C000?FLASH_SECTOR_3:SECOND_PAGE_PROPERTY_ADDR==0x08010000?FLASH_SECTOR_4:SECOND_PAGE_PROPERTY_ADDR==0x08020000?FLASH_SECTOR_5:
                           SECOND_PAGE_PROPERTY_ADDR==0x08040000?FLASH_SECTOR_6:SECOND_PAGE_PROPERTY_ADDR==0x08060000?FLASH_SECTOR_7:SECOND_PAGE_PROPERTY_ADDR==0x08080000?FLASH_SECTOR_8:
                           SECOND_PAGE_PROPERTY_ADDR==0x080A0000?FLASH_SECTOR_9:SECOND_PAGE_PROPERTY_ADDR==0x080C0000?FLASH_SECTOR_10:/*SECOND_PAGE_PROPERTY_ADDR==0x080E0000?*/FLASH_SECTOR_11;
	EraseInitStruct.NbSectors=1;		
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
							
  for(i=0;i<FIRST_PAGE_PROPERTY_SIZE/4;i++)HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,SECOND_PAGE_PROPERTY_ADDR+4*i,*(uint32_t*)(FIRST_PAGE_PROPERTY_ADDR+4*i));
		
	HAL_FLASH_Lock();
}

void memoryWriteNetworkParameters(NetworkParameters* networkParameters)
{
	Memory *memory = (void*)FIRST_PAGE_PROPERTY_ADDR;
	volatile int i=0;
	unlockAndEraseFirstPage();
	
	for(i=0;i<(((uint32_t)&memory->networkParameters)-FIRST_PAGE_PROPERTY_ADDR)/2;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FIRST_PAGE_PROPERTY_ADDR+2*i,(*(uint16_t*)(SECOND_PAGE_PROPERTY_ADDR+2*i)));
	}
		
	for(i=0;i<sizeof(NetworkParameters)/4;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)&(memory->networkParameters)+4*i,*(uint32_t*)(((uint32_t)networkParameters)+4*i));
	}
	
	for(i=((((uint32_t)&memory->networkParameters)-FIRST_PAGE_PROPERTY_ADDR)/2)+(sizeof(NetworkParameters)/2);i<FIRST_PAGE_PROPERTY_SIZE/2-1;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FIRST_PAGE_PROPERTY_ADDR+2*i,(*(uint16_t*)(SECOND_PAGE_PROPERTY_ADDR+2*i)));
	}
	
	writeCRCToFirstPageCopyFirstToSecondPageAndLock();
}

void memoryWriteMagicFlag(uint32_t magicFlag)
{
	Memory *memory = (void*)FIRST_PAGE_PROPERTY_ADDR;
	volatile int i=0;
	unlockAndEraseFirstPage();
	
	for(i=0;i<(((uint32_t)&memory->magicFlag)-FIRST_PAGE_PROPERTY_ADDR)/2;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FIRST_PAGE_PROPERTY_ADDR+2*i,(*(uint16_t*)(SECOND_PAGE_PROPERTY_ADDR+2*i)));
	}
		
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)&(memory->magicFlag),magicFlag);
	
	for(i=((((uint32_t)&memory->magicFlag)-FIRST_PAGE_PROPERTY_ADDR)/2)+(sizeof(uint32_t)/2);i<FIRST_PAGE_PROPERTY_SIZE/2-1;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FIRST_PAGE_PROPERTY_ADDR+2*i,(*(uint16_t*)(SECOND_PAGE_PROPERTY_ADDR+2*i)));
	}
	
	writeCRCToFirstPageCopyFirstToSecondPageAndLock();
}

void Memory_Init(void)//Memory* memory)
{
	uint16_t crcPage1=MODBUS_CRC16((uint8_t*)FIRST_PAGE_PROPERTY_ADDR,FIRST_PAGE_PROPERTY_SIZE);
  uint16_t crcPage2=MODBUS_CRC16((uint8_t*)SECOND_PAGE_PROPERTY_ADDR,SECOND_PAGE_PROPERTY_SIZE);

	if(crcPage1!=0&&crcPage2!=0)
	{
		Memory *memory = (void*)FIRST_PAGE_PROPERTY_ADDR;
		uint32_t PageErr;
		FLASH_EraseInitTypeDef EraseInitStruct;
		volatile int i=0;
			
	  HAL_FLASH_Unlock();
		
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
    EraseInitStruct.Sector = FIRST_PAGE_PROPERTY_ADDR==0x08000000?FLASH_SECTOR_0:FIRST_PAGE_PROPERTY_ADDR==0x08004000?FLASH_SECTOR_1:FIRST_PAGE_PROPERTY_ADDR==0x08008000?FLASH_SECTOR_2:
                             FIRST_PAGE_PROPERTY_ADDR==0x0800C000?FLASH_SECTOR_3:FIRST_PAGE_PROPERTY_ADDR==0x08010000?FLASH_SECTOR_4:FIRST_PAGE_PROPERTY_ADDR==0x08020000?FLASH_SECTOR_5:
                             FIRST_PAGE_PROPERTY_ADDR==0x08040000?FLASH_SECTOR_6:FIRST_PAGE_PROPERTY_ADDR==0x08060000?FLASH_SECTOR_7:FIRST_PAGE_PROPERTY_ADDR==0x08080000?FLASH_SECTOR_8:
                             FIRST_PAGE_PROPERTY_ADDR==0x080A0000?FLASH_SECTOR_9:FIRST_PAGE_PROPERTY_ADDR==0x080C0000?FLASH_SECTOR_10:/*FIRST_PAGE_PROPERTY_ADDR==0x080E0000?*/FLASH_SECTOR_11;
	  EraseInitStruct.NbSectors=1;		
	  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	  HAL_StatusTypeDef eraseResult = HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
		
    //isMainProgrammMustStart = 0xFFFFFFFF because flash erased
		
		/*NetworkParameters networkParameters ={
			.ipAddr={192,168,4,251},
			.netMask={255,255,255,0},
	    .gateWay={192,168,4,001},
			.modbusPort=502,
		};*/
		NetworkParameters networkParameters ={
			.ipAddr={172,16,2,52},
			.netMask={255,255,0,0},
	    .gateWay={172,16,0,13},
			.modbusPort=502,
		};
		
		for(i=0;i<sizeof(NetworkParameters)/4;i++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)&(memory->networkParameters)+4*i,*(uint32_t*)(((uint32_t)&networkParameters)+4*i));
		}
		
		/*ModbusParameters modbusParametersDefault = {
			.ModbusAddress=0x01,
			.BaudRate=9600,//9600-default
	    .DataBits=8,//8-default //8,9
	    .Parity=0,//0-default //0-none,1-even,2-odd
      .StopBits=1,//1,2
			.ModbusPort=502,
			.reserve=0xFFFF,
		};
		
		for(i=0;i<sizeof(ModbusParameters)/4;i++)//first RS232
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)&(memory->modbusParameters)+4*i,*(uint32_t*)(((uint32_t)&modbusParametersDefault)+4*i));
		}		*/
									
		uint16_t additionCRCPage = MODBUS_CRC16((uint8_t*)FIRST_PAGE_PROPERTY_ADDR,FIRST_PAGE_PROPERTY_SIZE-2);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FIRST_PAGE_PROPERTY_ADDR + FIRST_PAGE_PROPERTY_SIZE - 2,additionCRCPage);
		
		//-----------------------//
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
		EraseInitStruct.Sector = SECOND_PAGE_PROPERTY_ADDR==0x08000000?FLASH_SECTOR_0:SECOND_PAGE_PROPERTY_ADDR==0x08004000?FLASH_SECTOR_1:SECOND_PAGE_PROPERTY_ADDR==0x08008000?FLASH_SECTOR_2:
                             SECOND_PAGE_PROPERTY_ADDR==0x0800C000?FLASH_SECTOR_3:SECOND_PAGE_PROPERTY_ADDR==0x08010000?FLASH_SECTOR_4:SECOND_PAGE_PROPERTY_ADDR==0x08020000?FLASH_SECTOR_5:
                             SECOND_PAGE_PROPERTY_ADDR==0x08040000?FLASH_SECTOR_6:SECOND_PAGE_PROPERTY_ADDR==0x08060000?FLASH_SECTOR_7:SECOND_PAGE_PROPERTY_ADDR==0x08080000?FLASH_SECTOR_8:
                             SECOND_PAGE_PROPERTY_ADDR==0x080A0000?FLASH_SECTOR_9:SECOND_PAGE_PROPERTY_ADDR==0x080C0000?FLASH_SECTOR_10:/*SECOND_PAGE_PROPERTY_ADDR==0x080E0000?*/FLASH_SECTOR_11;
	  EraseInitStruct.NbSectors=1;		
	  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	  HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
							
		for(i=0;i<FIRST_PAGE_PROPERTY_SIZE/4;i++)HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,SECOND_PAGE_PROPERTY_ADDR+4*i,*(uint32_t*)(FIRST_PAGE_PROPERTY_ADDR+4*i));
		
		HAL_FLASH_Lock();
	}
	else if(crcPage1!=0)
	{
		uint32_t PageErr;
		FLASH_EraseInitTypeDef EraseInitStruct;
		volatile int i=0;
			
	  HAL_FLASH_Unlock();
		
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
    EraseInitStruct.Sector = FIRST_PAGE_PROPERTY_ADDR==0x08000000?FLASH_SECTOR_0:FIRST_PAGE_PROPERTY_ADDR==0x08004000?FLASH_SECTOR_1:FIRST_PAGE_PROPERTY_ADDR==0x08008000?FLASH_SECTOR_2:
                             FIRST_PAGE_PROPERTY_ADDR==0x0800C000?FLASH_SECTOR_3:FIRST_PAGE_PROPERTY_ADDR==0x08010000?FLASH_SECTOR_4:FIRST_PAGE_PROPERTY_ADDR==0x08020000?FLASH_SECTOR_5:
                             FIRST_PAGE_PROPERTY_ADDR==0x08040000?FLASH_SECTOR_6:FIRST_PAGE_PROPERTY_ADDR==0x08060000?FLASH_SECTOR_7:FIRST_PAGE_PROPERTY_ADDR==0x08080000?FLASH_SECTOR_8:
                             FIRST_PAGE_PROPERTY_ADDR==0x080A0000?FLASH_SECTOR_9:FIRST_PAGE_PROPERTY_ADDR==0x080C0000?FLASH_SECTOR_10:/*FIRST_PAGE_PROPERTY_ADDR==0x080E0000?*/FLASH_SECTOR_11;
	  EraseInitStruct.NbSectors=1;		
	  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	  HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
							
		for(i=0;i<FIRST_PAGE_PROPERTY_SIZE/4;i++)HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,FIRST_PAGE_PROPERTY_ADDR+4*i,*(uint32_t*)(SECOND_PAGE_PROPERTY_ADDR+4*i));
		
		HAL_FLASH_Lock();
	}
	else if(crcPage2!=0)
	{
		uint32_t PageErr;
		FLASH_EraseInitTypeDef EraseInitStruct;
		volatile int i=0;
			
	  HAL_FLASH_Unlock();
		
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;		
    EraseInitStruct.Sector = SECOND_PAGE_PROPERTY_ADDR==0x08000000?FLASH_SECTOR_0:SECOND_PAGE_PROPERTY_ADDR==0x08004000?FLASH_SECTOR_1:SECOND_PAGE_PROPERTY_ADDR==0x08008000?FLASH_SECTOR_2:
                             SECOND_PAGE_PROPERTY_ADDR==0x0800C000?FLASH_SECTOR_3:SECOND_PAGE_PROPERTY_ADDR==0x08010000?FLASH_SECTOR_4:SECOND_PAGE_PROPERTY_ADDR==0x08020000?FLASH_SECTOR_5:
                             SECOND_PAGE_PROPERTY_ADDR==0x08040000?FLASH_SECTOR_6:SECOND_PAGE_PROPERTY_ADDR==0x08060000?FLASH_SECTOR_7:SECOND_PAGE_PROPERTY_ADDR==0x08080000?FLASH_SECTOR_8:
                             SECOND_PAGE_PROPERTY_ADDR==0x080A0000?FLASH_SECTOR_9:SECOND_PAGE_PROPERTY_ADDR==0x080C0000?FLASH_SECTOR_10:/*SECOND_PAGE_PROPERTY_ADDR==0x080E0000?*/FLASH_SECTOR_11;
	  EraseInitStruct.NbSectors=1;		
	  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		
	  HAL_FLASHEx_Erase(&EraseInitStruct,&PageErr);
							
		for(i=0;i<FIRST_PAGE_PROPERTY_SIZE/4;i++)HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,SECOND_PAGE_PROPERTY_ADDR+4*i,*(uint32_t*)(FIRST_PAGE_PROPERTY_ADDR+4*i));
		
		HAL_FLASH_Lock();
	}
}



