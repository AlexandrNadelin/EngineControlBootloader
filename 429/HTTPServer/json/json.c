#include "json.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

void DecodeURL(char *urlBuf,uint16_t* urlCursor,char* jsonBuf,uint16_t* jsonCursor, uint16_t inputDataLenght)
{
	*urlCursor=0;
	*jsonCursor=0;
	while (urlBuf[*urlCursor] != '\r'&&*urlCursor<inputDataLenght)
	{
		if (urlBuf[*urlCursor] == '%')
		{
			if ((urlBuf[*urlCursor+1] == '7') && (urlBuf[*urlCursor+2] == 'B'))
			{
				jsonBuf[*jsonCursor] = '{';
				*urlCursor += 2;
			}
			if ((urlBuf[*urlCursor+1] == '2') && (urlBuf[*urlCursor+2] == '2'))
			{
				jsonBuf[*jsonCursor] = '\"';
				*urlCursor += 2;
			}
			if ((urlBuf[*urlCursor+1] == '7') && (urlBuf[*urlCursor+2] == 'D'))
			{
				jsonBuf[*jsonCursor] = '}';
				jsonBuf[*jsonCursor+1] = '\0';
				break;
			}
		} 
		else 
		{
			jsonBuf[*jsonCursor] = urlBuf[*urlCursor];
		}
		*urlCursor+=1;
		*jsonCursor+=1;
	}
}

void jsonStrToNetParameters(char* jsonStr, uint16_t jsonStrLenght, NetworkParameters* networkParameters)
{
	char* endPtr;
	int i;
	for(i=1;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"IPAddress",sizeof("IPAddress")-1)== 0)
		{
			i=i+3+sizeof("IPAddress")-1;					
					
			networkParameters->ipAddr[0]=strtoul(&jsonStr[i],&endPtr,10);
			endPtr++;//.
			networkParameters->ipAddr[1]=strtoul(endPtr,&endPtr,10);
			endPtr++;//.
			networkParameters->ipAddr[2]=strtoul(endPtr,&endPtr,10);
			endPtr++;//.
			networkParameters->ipAddr[3]=strtoul(endPtr,&endPtr,10);
					
			i=endPtr-jsonStr+3;//","
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"SubnetMask",sizeof("SubnetMask")-1)== 0)
		{
			i=i+3+sizeof("SubnetMask")-1;					
					
			networkParameters->netMask[0]=strtoul(&jsonStr[i],&endPtr,10);
			endPtr++;//.
			networkParameters->netMask[1]=strtoul(endPtr,&endPtr,10);
			endPtr++;//.
			networkParameters->netMask[2]=strtoul(endPtr,&endPtr,10);
			endPtr++;//.
			networkParameters->netMask[3]=strtoul(endPtr,&endPtr,10);
					
			i=endPtr-jsonStr+3;//","
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"GateWay",sizeof("GateWay")-1)== 0)
		{
			i=i+3+sizeof("GateWay")-1;					
					
			networkParameters->gateWay[0]=strtoul(&jsonStr[i],&endPtr,10);
			endPtr++;//.
			networkParameters->gateWay[1]=strtoul(endPtr,&endPtr,10);
			endPtr++;//.
			networkParameters->gateWay[2]=strtoul(endPtr,&endPtr,10);
			endPtr++;//.
			networkParameters->gateWay[3]=strtoul(endPtr,&endPtr,10);
					
			i=endPtr-jsonStr+3;//","
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"ModbusPort",sizeof("ModbusPort")-1)== 0)
		{
			i=i+3+sizeof("ModbusPort")-1;//","502"			
					
			networkParameters->modbusPort=strtoul(&jsonStr[i],&endPtr,10);
					
			i=endPtr-jsonStr+3;
			break;
		}
	}
}

/*void jsonStrToModbusParameters(char* jsonStr, uint16_t jsonStrLenght,ModbusParameters* modbusParameters)//void jsonStrToConfigRS232(char* jsonStr, uint16_t jsonStrLenght, ConfigRS232* configRS232)
{
	char* endPtr;
	int i;
	
	for(i=1;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"ModbusAddress",sizeof("ModbusAddress")-1)== 0)
		{
			i=i+3+sizeof("ModbusAddress")-1;//","1"			
					
			modbusParameters->ModbusAddress=strtoul(&jsonStr[i],&endPtr,10);
					
			i=endPtr-jsonStr+3;
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"ModbusPort",sizeof("ModbusPort")-1)== 0)
		{
			i=i+3+sizeof("ModbusPort")-1;//","502"			
					
			modbusParameters->ModbusPort=strtoul(&jsonStr[i],&endPtr,10);
					
			i=endPtr-jsonStr+3;
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"BaudRate",sizeof("BaudRate")-1)== 0)
		{
			i=i+3+sizeof("BaudRate")-1;					
			modbusParameters->BaudRate=strtoul(&jsonStr[i],&endPtr,10);					
			i=endPtr-jsonStr+3;//","
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"DataBits",sizeof("DataBits")-1)== 0)
		{
			i=i+3+sizeof("DataBits")-1;
      modbusParameters->DataBits=strtoul(&jsonStr[i],&endPtr,10);					
			i=endPtr-
			jsonStr+3;//","
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"Parity",sizeof("Parity")-1)== 0)
		{
			i=i+3+sizeof("Parity")-1;
      if(memcmp(&jsonStr[i],"None",sizeof("None")-1)==0)modbusParameters->Parity=0;
      else if(memcmp(&jsonStr[i],"Even",sizeof("Even")-1)==0)modbusParameters->Parity=1;		
      else if(memcmp(&jsonStr[i],"Odd",sizeof("Odd")-1)==0)modbusParameters->Parity=2;	
			i=endPtr-jsonStr+3;//","
			break;
		}
	}
	
	for(;i<jsonStrLenght;i++)
	{
		if(memcmp(&jsonStr[i],"StopBits",sizeof("StopBits")-1)== 0)
		{
			i=i+3+sizeof("StopBits")-1;
      modbusParameters->StopBits=strtoul(&jsonStr[i],&endPtr,10);
			i=endPtr-jsonStr+3;//","
			break;
		}
	}
}*/


