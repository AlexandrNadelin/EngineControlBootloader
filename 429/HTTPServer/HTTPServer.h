#ifndef __HTTPServer_H
#define __HTTPServer_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"
#include "lwip/api.h"
#include "lwip/tcp.h"

typedef struct{
	struct tcp_pcb *pcb;
	void* server;
	err_t err;
	uint32_t lastReceiveTime;
	uint32_t receiveTimeout;

	char* nextDataTxPtr;
	uint16_t nextDataLength;
	uint16_t property;
	uint16_t connectionNumber;
	int dataToReceive;
}HTTPConnection;

typedef struct{
  uint8_t numberOfConnections;
  HTTPConnection* connections[MEMP_NUM_TCP_PCB-1];
	struct tcp_pcb *mainPcb;
}HTTPServer;


void HTTPServer_Init(HTTPServer* httpServer);
/*HAL_StatusTypeDef UP_TCPServerTransmitAnswerHeader2(UP_TCPConnection* connection,uint16_t connectionNumber,uint8_t srcAddr,uint8_t destAddr,uint8_t*data,uint8_t dataLen);
HAL_StatusTypeDef UP_TCPServerTransmitAnswerHeader3(UP_TCPConnection* connection,uint16_t connectionNumber,uint8_t srcLetterAddr,uint8_t srcChanAddr,uint8_t destLetterAddr,uint8_t destChanAddr,uint8_t*data,uint8_t dataLen);

unsigned short processUPRequestDataCallback(unsigned char* packetData,unsigned short packetDataLenght, unsigned char* outBufferData);
void UP_TCPRequestForSensorCallback(UP_TCPConnection* connection
		                                 ,uint16_t connectionNumber
																		 ,uint8_t headerType
		                                 ,uint8_t srcLetterAddr
		                                 ,uint8_t srcChanAddr
		                                 ,uint8_t destLetterAddr
		                                 ,uint8_t destChanAddr
		                                 ,uint8_t*data
		                                 ,uint16_t dataLen);*/


#ifdef __cplusplus
 }
#endif
#endif

