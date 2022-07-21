#include "HTTPServer.h"
#include <string.h>
#include "FileSystem.h"
#include "Memory.h"
#include "json.h"
#include "TimeSpan.h"

const static char PAGE_HEADER[] =
"HTTP/1.1 200 OK\r\n\
Server: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\n\
Content-type: text/html\r\n\
Content-Length: 0\r\n\
Connection: keep-alive\r\n\r\n";

#define REBOOT_BIT (uint16_t)0x0001

//---Extern---//
extern Memory* memory;
//extern NetworkParameters networkParameters;
/*extern ClimateControlDevice climateControlDevice;
extern UP_CANClient upCanClient;*/
//---End extern ---//

#define SIZE_HEADER_TMP_BUFFER 256

static void printErrT(err_t err)
{
	printf("Error: %s\r\n",err==ERR_MEM?"ERR_MEM":err==ERR_OK?"ERR_OK"
		                                           :err==ERR_BUF?"ERR_BUF"
		                                           :err==ERR_TIMEOUT?"ERR_TIMEOUT"
		                                           :err==ERR_RTE?"ERR_RTE"
		                                           :err==ERR_INPROGRESS?"ERR_INPROGRESS"
		                                           :err==ERR_VAL?"ERR_VAL"
		                                           :err==ERR_WOULDBLOCK?"ERR_WOULDBLOCK"
		                                           :err==ERR_USE?"ERR_USE"
		                                           :err==ERR_ALREADY?"ERR_ALREADY"
		                                           :err==ERR_ISCONN?"ERR_ISCONN"
		                                           :err==ERR_CONN?"ERR_CONN"
		                                           :err==ERR_IF?"ERR_IF"
		                                           :err==ERR_ABRT?"ERR_ABRT"
		                                           :err==ERR_RST?"ERR_RST"
		                                           :err==ERR_CLSD?"ERR_CLSD"
		                                           :err==ERR_ARG?"ERR_ARG"
		                                           :"");
}

uint8_t twoCharToUint8(char* chars)
{
	uint8_t number=0;
	if(chars[0]>47&&chars[0]<58)number=(chars[0]-48)<<4;
	else if(chars[0]>64&&chars[0]<71)number=(chars[0]-55)<<4;
	
	if(chars[1]>47&&chars[1]<58)number=number|(chars[1]-48);
	else if(chars[1]>64&&chars[1]<71)number=number|(chars[1]-55);
  
	return number;
}

uint8_t hexFileParse(uint8_t* data,uint16_t len,uint8_t reset)
{
	static uint8_t bytes[45];
  static uint8_t bytesHead=0;
  static uint16_t upperAddress=0;
	
	static uint32_t pageStartAddress;
	static uint8_t errorCode;
	
	if(reset)
	{
		bytesHead=0;
		upperAddress=0x0000;
		isSectorErased(0x00000000,&pageStartAddress, &errorCode, 1);//Сбрасываем
	}
	
	volatile int i=0;
	
	if(HAL_FLASH_Unlock()!=HAL_OK)printf("Unlock flash error\r\n");
	
	while(bytesHead<45&&i<len)
	{
		if(data[i]==':')
		{
			bytes[0]=':';			
			bytesHead=1;
			i++;
		}
		else
		{	      		
			bytes[bytesHead++]=data[i++];
			if(bytesHead>1 && memcmp(&bytes[bytesHead-2],"\r\n",sizeof("\r\n")-1)==0)
			{
				//process
				uint8_t checksum=0;
				int i;
				for(i=0;i<(bytesHead-3)/2;i++)
				{
					bytes[i]=twoCharToUint8((char*)&bytes[1+i*2]);
					checksum+=bytes[i];
				}
				
				if(checksum==0)
				{
					if(bytes[3]==0x04)//set upperAddress
				  {
					  upperAddress=bytes[4]<<8|bytes[5];					
				  }
				  else if(bytes[3]==0x00)//data
				  {
					  uint16_t lowerAddress = bytes[1]<<8|bytes[2];
					
					  uint32_t programAddress = upperAddress<<16|lowerAddress;
						
						/*if(!isSectorErased(programAddress,&pageStartAddress, &errorCode, 0)&& errorCode==0)
						{
							if(eraseSector(pageStartAddress)!=HAL_OK)printf("Erase sector error\r\n");
						}*/
						
						if(programAddress>=MAIN_APP_START_ADDR && programAddress<MAIN_APP_START_ADDR+MAIN_APP_SIZE)//in the acceptable range
					  {			
							for(int i=0;i<bytes[0]/2;i++)
					    {
						    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,programAddress+i*2,*(uint16_t*)&bytes[4+i*2])!=HAL_OK)printf("Program flash error\r\n");
					    }		
					  }				
				  }
				  else if(bytes[3]==0x01)//final
				  {
					  HAL_FLASH_Lock();
					  bytesHead=0;
	          return 1;
				  }
				}
				else printf("crc error\r\n");
				
				bytesHead=0;
			}		
		}
		
		if(bytesHead==45)bytesHead=0;
	}
	
	if(HAL_FLASH_Lock()!=HAL_OK)printf("Lock flash error\r\n");
	return 0;
}

void updDataProcess(uint8_t* data,uint16_t len,uint8_t reset)
{
	hexFileParse(data,len,reset);
}

void tcpDataProcess(HTTPConnection* connection, uint8_t* request, uint16_t requestLength)
{
	struct fs_file file;
	if(connection->dataToReceive>0)
	{
		if(connection->dataToReceive-requestLength==0)
		{
			updDataProcess(request,requestLength,0);
			connection->dataToReceive=0;
			
			//Вычисляем контрольную сумму, записываем ее в конец
			HAL_FLASH_Unlock();
			uint16_t additionCRCPage = MODBUS_CRC16((uint8_t*)MAIN_APP_START_ADDR, MAIN_APP_SIZE-2);
	    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,MAIN_APP_START_ADDR + MAIN_APP_SIZE - 2,additionCRCPage);
			HAL_FLASH_Lock();
			
			//Отвечаем что данные принятыuint8_t HttpHeaderBuffer[SIZE_HEADER_TMP_BUFFER];
      uint8_t HttpHeaderBuffer[SIZE_HEADER_TMP_BUFFER];
			uint16_t headerLenght = sprintf((char*)HttpHeaderBuffer
						    ,"HTTP/1.1 200 OK\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\nContent-type: text/html\r\nContent-Length: %u\r\nConnection: keep-alive\r\n\r\n"
					      ,0);

			if((connection->err = tcp_write(connection->pcb,HttpHeaderBuffer,headerLenght,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
			{
				printf("write update file received ");
				printErrT(connection->err);
				return;
			}
		}
		else if(connection->dataToReceive-requestLength>0)
		{
			updDataProcess(request,requestLength,0);
			connection->dataToReceive-=requestLength;			
		}
		else// if(connection->dataToReceive-requestLength<0)//Ошибка длинны
		{
			connection->dataToReceive-=requestLength;	
			uint8_t HttpHeaderBuffer[SIZE_HEADER_TMP_BUFFER];
			uint16_t headerLenght = sprintf((char*)HttpHeaderBuffer,"HTTP/1.1 400 Bad Request\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\n\r\n");
		  if((connection->err = tcp_write(connection->pcb,HttpHeaderBuffer,headerLenght,TCP_WRITE_FLAG_COPY))!=ERR_OK ||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		  {
			  printf("write Bad Request");
			  printErrT(connection->err);
			  return;
		  }
		}
	}
	else if(memcmp(request,"GET / HTTP/1.1",sizeof("GET / HTTP/1.1")-1)==0 || memcmp(request,"Update.html",sizeof("Update.html")-1)==0)//
	{
		if(fs_open(&file,"Update.html")==ERR_OK)
		{
			uint8_t HttpHeaderBuffer[SIZE_HEADER_TMP_BUFFER];

			uint16_t headerLenght = sprintf((char*)HttpHeaderBuffer
						    ,"HTTP/1.1 200 OK\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\nContent-type: text/html\r\nContent-Length: %u\r\nConnection: keep-alive\r\n\r\n"
					      ,file.len);

			if((connection->err = tcp_write(connection->pcb,HttpHeaderBuffer,headerLenght,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
			{
				printf("write Update http header ");
				printErrT(connection->err);
				return;
			}

			connection->nextDataTxPtr = (char*)file.data - headerLenght;
			connection->nextDataLength = file.len + headerLenght;
	  }
	}
	else if(memcmp(request,"GET /Index.html",sizeof("GET /Index.html")-1)==0
    		||memcmp(request,"GET /Control.html",sizeof("GET /Control.html")-1)==0
				||memcmp(request,"GET /Debug.html",sizeof("GET /Debug.html")-1)==0)
	{
		if(memory->magicFlag!=RUN_MAIN_APP_DEFAULT_MAGIC_FLAG) memoryWriteMagicFlag(RUN_MAIN_APP_DEFAULT_MAGIC_FLAG);
		HAL_NVIC_SystemReset();
	}
	else if(memcmp(request,"GET /",sizeof("GET /")-1)==0 && fs_open(&file, (char*)&request[sizeof("GET /")-1])==ERR_OK)//"GET /style.css"
	{
		char* contentType="text/html";

		for(int i =sizeof("GET /")-1;i<requestLength;i++)
		{
			if(request[i]=='.')
			{
			  if(memcmp(&request[i],".html",sizeof(".html")-1)==0)contentType="text/html";
			  else if(memcmp(&request[i],".css",sizeof(".css")-1)==0)contentType="text/css";
			  else if(memcmp(&request[i],".ico",sizeof(".ico")-1)==0)contentType="image/x-icon";
			  else contentType="application/json";

			  break;
			}
		}

	  uint8_t HttpHeaderBuffer[SIZE_HEADER_TMP_BUFFER];

	  uint16_t headerLenght = sprintf((char*)HttpHeaderBuffer//Content-type: text/html\r\n
								,"HTTP/1.1 200 OK\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\nContent-type: %s\r\nContent-Length: %u\r\nConnection: keep-alive\r\n\r\n"
								,contentType
								,file.len);
		if((connection->err = tcp_write(connection->pcb,HttpHeaderBuffer,headerLenght,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		{
			printf("write another request header ");
			printErrT(connection->err);
			return;
		}

		connection->nextDataTxPtr = (char*)file.data - headerLenght;
		connection->nextDataLength = file.len + headerLenght;
	}
	else if(memcmp(request, "POST /UpdateData",sizeof("POST /UpdateData")-1) == 0)
	{
		connection->dataToReceive=0;
		uint16_t i;
		for(i = sizeof("POST /UpdateData HTTP/1.1\r\n")-1;i<requestLength-sizeof("Content-Length: xxxxx")-1;i++)
		{
			if(memcmp(&request[i], "Content-Length: ",sizeof("Content-Length:")) == 0)
			{
				i+=sizeof("Content-Length:");
				connection->dataToReceive = strtol((char*)&request[i],NULL,10);
				break;
			}
		}
		
		if(connection->dataToReceive==0)
		{
			return;
		}
		
		for(;i<requestLength-(sizeof("\r\n\r\n")-1);i++)//Дальше пойдут данные
		{
			if(memcmp(&request[i], "\r\n\r\n",sizeof("\r\n\r\n")-1) == 0)
			{
				//i+=sizeof("\r\n\r\n")-1;
				break;
			}
		}
		
		if(memcmp(&request[i], "\r\n\r\n",sizeof("\r\n\r\n")-1) != 0)
		{
			connection->dataToReceive=0;
			
			for(int j =0; j< requestLength; j++)
			{
				printf("%c",request[j]);
			}
			return;
		}
		else i+=sizeof("\r\n\r\n")-1;
		
		HAL_FLASH_Unlock();
		if(eraseSector(0x08020000)!=HAL_OK)printf("Erase sector error\r\n");
		if(eraseSector(0x08040000)!=HAL_OK)printf("Erase sector error\r\n");
		if(eraseSector(0x08060000)!=HAL_OK)printf("Erase sector error\r\n");
		if(eraseSector(0x08080000)!=HAL_OK)printf("Erase sector error\r\n");
		if(eraseSector(0x080A0000)!=HAL_OK)printf("Erase sector error\r\n");
		if(eraseSector(0x080C0000)!=HAL_OK)printf("Erase sector error\r\n");
		if(eraseSector(0x080E0000)!=HAL_OK)printf("Erase sector error\r\n");
		
		
		//Обрабатываем данные пакета
		updDataProcess(&request[i],requestLength - i,1);
		
		connection->dataToReceive -=(requestLength - i);
		
//POST /UpdateData HTTP/1.1
//Host: 192.168.4.251
//Connection: keep-alive
//Content-Length: 238679
//User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.82 Safari/537.36
//Content-Type: text/plain;charset=UTF-8
//Accept: */*
//Origin: http://192.168.4.251
//Referer: http://192.168.4.251/Update.html
//Accept-Encoding: gzip, deflate
//Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7
//
//:020000040802F0
//:1000000070700020C5010208C12B0208B12502084A
//:10001000112A0208C10402089D2C0208000

	}
	/*else if(memcmp(request, "GET /NetworkParameters.property?=",sizeof("GET /NetworkParameters.property?=")-1) == 0)
	{
		char HttpAnswerBuffer[720];

		uint16_t bodyLenght = sprintf(&HttpAnswerBuffer[256]
				,"{\"IPAddress\":\"%u.%u.%u.%u\",\"SubnetMask\":\"%u.%u.%u.%u\",\"GateWay\":\"%u.%u.%u.%u\"}"
				,memory->networkParameters.ipAddr[0],memory->networkParameters.ipAddr[1],memory->networkParameters.ipAddr[2],memory->networkParameters.ipAddr[3]
				,memory->networkParameters.netMask[0],memory->networkParameters.netMask[1],memory->networkParameters.netMask[2],memory->networkParameters.netMask[3]
				,memory->networkParameters.gateWay[0],memory->networkParameters.gateWay[1],memory->networkParameters.gateWay[2],memory->networkParameters.gateWay[3]);

		uint16_t headerLenght = sprintf(HttpAnswerBuffer
				,"HTTP/1.1 200 OK\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\nContent-type: application/json\r\nContent-Length: %u\r\nConnection: keep-alive\r\n\r\n"
				,bodyLenght);

		memcpy(&HttpAnswerBuffer[headerLenght],&HttpAnswerBuffer[256],bodyLenght);

		if((connection->err = tcp_write(connection->pcb,HttpAnswerBuffer,headerLenght+bodyLenght,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		{
			printf("NetworkParameters.property?= ");
			printErrT(connection->err);
			return;
		}
	}
	else if(memcmp(request, "GET /NetworkParameters.property=",sizeof("GET /NetworkParameters.property=")-1) == 0)
	{
		uint16_t startJsonByte = sizeof("GET /NetworkParameters.property=")-1;
		uint16_t inputDataLenght = requestLength-startJsonByte;

		uint16_t urlCursor=0,jsonCursor=0;
		DecodeURL((char*)&request[startJsonByte],&urlCursor,(char*)&request[0],&jsonCursor,inputDataLenght);
		NetworkParameters networkParameters;
		jsonStrToNetParameters((char*)request,jsonCursor, &networkParameters);

		memoryWriteNetworkParameters(&networkParameters);

	  if((connection->err = tcp_write(connection->pcb,PAGE_HEADER,sizeof(PAGE_HEADER)-1,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		{
			printf("NetworkParameters.property= ");
			printErrT(connection->err);
			return;
		}
	}*/
	else if(memcmp(request, "GET /reboot",sizeof("GET /reboot")-1) == 0)
	{
		if((connection->err = tcp_write(connection->pcb,PAGE_HEADER,sizeof(PAGE_HEADER)-1,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		{
			printf("GET /cmd.reboot=1 answer error ");
			printErrT(connection->err);
			return;
		}
		connection->property|=REBOOT_BIT;
	}
	else if(fs_open(&file, "badRequest.html")==ERR_OK)
	{
		uint8_t HttpHeaderBuffer[SIZE_HEADER_TMP_BUFFER];

		uint16_t lenght = sprintf((char*)HttpHeaderBuffer
				                         ,"HTTP/1.1 400 Bad Request\r\nServer: lwIP/1.3.1 (http://savannah.nongnu.org/projects/lwip)\r\nContent-type: text/html\r\nContent-Length: %u\r\nConnection: keep-alive\r\n\r\n"
				                         ,file.len);

		if((connection->err = tcp_write(connection->pcb,HttpHeaderBuffer,lenght,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		{
			printf("write bad request header ");
			printErrT(connection->err);
			return;
		}

	  if((connection->err = tcp_write(connection->pcb,file.data, file.len,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
		{
			printf("write another request body ");
			printErrT(connection->err);
			return;
		}
	}
}

err_t connectionRecv(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err)
{
	HTTPConnection* connection = arg;

	if(p==NULL)//browser closed connection
	{
		//tcp_arg(tpcb, NULL);
		tcp_close(tpcb);

		if(arg==NULL)return ERR_BUF;
		HTTPServer* server = connection->server;

		for(uint16_t i=0;i<MEMP_NUM_TCP_PCB-1;i++)
	  {
		  if(server->connections[i]==connection)
		  {
		    server->connections[i]->connectionNumber=0;
        server->connections[i]=0x00000000;
		    free(connection);
	      server->numberOfConnections--;
			  break;
		  }
	  }

		return ERR_OK;
	}

	struct pbuf *p_temp = p;

	uint8_t requestBuffer[p_temp->tot_len+1];
	uint16_t position=0;

	while(p_temp != NULL)
  {
    memcpy(&requestBuffer[position],p_temp->payload,p_temp->len);
		position+=p_temp->len;
    p_temp = p_temp->next;
  }

	tcp_recved(tpcb,p->tot_len);
	pbuf_free(p);

	requestBuffer[p->tot_len]=0;

	if(arg!=NULL)
	{
		connection->lastReceiveTime = HAL_GetTick();
		tcpDataProcess(connection,requestBuffer,p->tot_len);
	}

	return ERR_OK;
}

static err_t connectionSend(void *arg, struct tcp_pcb *tpcb,u16_t len)//data acknowledged callback
{
	if(arg==NULL)
	{
		return ERR_OK;
	}
	HTTPConnection* connection = arg;

	if(connection->property&REBOOT_BIT)HAL_NVIC_SystemReset();
	
	//!!!Может отправлять кусками то что было приказано отправить командой tcp_write и после каждого куска будет вызвана эта функция
	if(connection->nextDataTxPtr != 0x00000000)
	{
	  connection->nextDataTxPtr+=len;
	  connection->nextDataLength-=len;
	  if(connection->nextDataLength==0)
	  {
		  if((uint32_t)connection->nextDataTxPtr<0x08000000)
		  {
			  //free(connection->nextDataTxPtr)
		  }
		  connection->nextDataTxPtr = 0x00000000;
		  //tcp_close(tpcb);
		  return ERR_OK;
	  }
  }
	else
	{
		//tcp_close(tpcb);
		return ERR_OK;
	}


	uint16_t lenToSend = connection->nextDataLength;

	uint16_t lenSndBuf = tcp_sndbuf(tpcb);
	uint16_t lenMms = tcp_mss(tpcb);// *2/////Чтобы сколько кусков отправлено - столько раз и сработал Колбэк//

	if(lenToSend>lenSndBuf)lenToSend=lenSndBuf;
	if(lenToSend>lenMms)lenToSend=lenMms;

	if((connection->err = tcp_write(connection->pcb,connection->nextDataTxPtr,lenToSend,TCP_WRITE_FLAG_COPY))!=ERR_OK||(connection->err = tcp_output(connection->pcb))!=HAL_OK)
	{
		printf("send next part ");
		printErrT(connection->err);

		connection->nextDataTxPtr = NULL;
		return connection->err;
	}

	return ERR_OK;
}

void  connectionErr(void *arg, err_t err)
{
	printErrT(err);

	if(arg==0x00000000)return;

	HTTPConnection* connection = arg;
	HTTPServer* server = connection->server;

	for(uint16_t i=0;i<MEMP_NUM_TCP_PCB-1;i++)
	{
		if(server->connections[i]==connection)
		{
	    server->connections[i]->connectionNumber=0;
		  server->connections[i]=0x00000000;
		  free(connection);

	    server->numberOfConnections--;
	    printf("UP TCP Server stop con error. Number of con: %u\r\n",server->numberOfConnections);
			break;
		}
	}
}

err_t connectionPoll(void *arg, struct tcp_pcb *tpcb)
{
	if(arg==0x00000000)return ERR_OK;
	HTTPConnection* connection = arg;
	if(GetTimeSpan(connection->lastReceiveTime, HAL_GetTick())>connection->receiveTimeout)//getTimeSpan(connection->lastReceiveTime)>connection->receiveTimeout)
	{
		//tcp_arg(tpcb, NULL);
		tcp_close(tpcb);

		HTTPServer* server = connection->server;

		for(uint16_t i=0;i<MEMP_NUM_TCP_PCB-1;i++)
	  {
		  if(server->connections[i]==connection)
		  {
		    printf("UP TCP connection receive timeout\r\n");
		    server->connections[i]->connectionNumber=0;
        server->connections[i]=0x00000000;
		    free(connection);

	      server->numberOfConnections--;

	      //printf("UP TCP Server stop con. Number of con: %u\r\n",server->numberOfConnections);
			  break;
		  }
	  }
	}

	return ERR_OK;
}

err_t HTTPServerAccept(void *arg,struct tcp_pcb *pcb,err_t err)
{
	uint16_t i=0;
	HTTPServer* httpServer = arg;
	if(httpServer->numberOfConnections>MEMP_NUM_TCP_PCB-2)
	{
		printf("HTTP number of allowed connections exceeded.\r\n");
		tcp_close(pcb);
		HAL_NVIC_SystemReset();
		return ERR_MEM;
	}

	for(i=0;i<MEMP_NUM_TCP_PCB-1;i++)
	{
		if(httpServer->connections[i]==NULL)
		{
			HTTPConnection* connection = malloc(sizeof(HTTPConnection));

			if(connection==0x00000000)
			{
				printf("connection malloc error.\r\n");
		    tcp_close(pcb);
				HAL_NVIC_SystemReset();
				return ERR_MEM;//break;
			}

			connection->receiveTimeout=300000;//10000;//0;
			connection->server=httpServer;
			connection->pcb=pcb;
			connection->err=ERR_OK;

			static uint16_t connectionNumber=0;
      if(++connectionNumber==0)connectionNumber++;

      connection->connectionNumber=connectionNumber;

			httpServer->numberOfConnections++;

			httpServer->connections[i]=connection;

			tcp_arg(pcb, connection);
			tcp_recv(pcb, connectionRecv);//receive complated
      tcp_sent(pcb, connectionSend);//transmit complated
      tcp_err(pcb, connectionErr);//error occurred
      tcp_poll(pcb, connectionPoll, 2);//It can be used by the application to
                                        //check if there are remaining application data that
                                        //needs to be sent or if there are connections that
                                        //need to be closed. Time - interval*0.5 second

      connection->lastReceiveTime=HAL_GetTick();
      connection->nextDataTxPtr=NULL;
      connection->property=0x0000;
			connection->dataToReceive=0;

			//printf("HTTP Server start con. Number of con: %u\r\n",httpServer->numberOfConnections);
			break;
		}
	}

	if(i==MEMP_NUM_TCP_PCB-1)
	{
	  tcp_close(pcb);
		printf("i==MEMP_NUM_TCP_PCB-1\r\n");
		HAL_NVIC_SystemReset();
		return ERR_MEM;
	}

  return ERR_OK;
}

void HTTPServer_Init(HTTPServer* httpServer)
{
	err_t err;
	httpServer->numberOfConnections=0;
	for(uint16_t i=0;i<MEMP_NUM_TCP_PCB-1;i++)
	{
		httpServer->connections[i]=0x00000000;
	}

	if((httpServer->mainPcb = tcp_new())==0)
	{
	  printf("UP_TCP Server main pcb create error\r\n");
		return;
	}

	if((err = tcp_bind(httpServer->mainPcb,IP_ADDR_ANY,80))!=ERR_OK)
	{
	  printf("UP_TCP Server main pcb bind.");
		printErrT(err);
		return;
	}

	tcp_arg(httpServer->mainPcb, httpServer);

	httpServer->mainPcb = tcp_listen(httpServer->mainPcb);
	tcp_accept(httpServer->mainPcb,HTTPServerAccept);

	//tcp_poll(httpServer->mainPcb, httpServerPoll, 2);
	printf("HTTP Server started. IP %u.%u.%u.%u, Mask  %u.%u.%u.%u, Gate  %u.%u.%u.%u\r\n"
	,memory->networkParameters.ipAddr[0]
	,memory->networkParameters.ipAddr[1]
	,memory->networkParameters.ipAddr[2]
	,memory->networkParameters.ipAddr[3]
	,memory->networkParameters.netMask[0]
	,memory->networkParameters.netMask[1]
	,memory->networkParameters.netMask[2]
	,memory->networkParameters.netMask[3]
	,memory->networkParameters.gateWay[0]
	,memory->networkParameters.gateWay[1]
	,memory->networkParameters.gateWay[2]
	,memory->networkParameters.gateWay[3]);
}

