#include "i2c.h"
#include "led.h"
#include "uart.h"
#include "string.h"
#include "command_decoder.h"

unsigned char ucPCF8574_Input;
unsigned char ucMC24LC64_MemByte;

char cHexStr[TRANSMITTER_SIZE - 1];
char cStringToSend[TRANSMITTER_SIZE];
char cReceivedMessage[RECEIVER_SIZE];

extern struct Token asToken[MAX_TOKEN_NR];
extern unsigned char ucTokenNr;

//****************************************************functions********************************************************************

void Delay(int iLatency) {
	
	int iLoopCounter;
	
	for (iLoopCounter = 0; iLoopCounter < (iLatency*5455); iLoopCounter++) {}
}

//******************************************************main*********************************************************************
int main(void) {
	
	unsigned char fSendID = 0;
	unsigned char fSendPort = 0;
	unsigned char fSendMem = 0;
	unsigned char fSendUnknownComm = 0;
		
	Led_init();
	I2C_Init();
	UART_InitWithInt(9600);
	
	while(1) {
		
		if (Receiver_GetStatus() == READY) {
			Receiver_GetStringCopy(cReceivedMessage);
			DecodeMsg(cReceivedMessage);
		
			if(asToken[0].eType == KEYWORD && ucTokenNr > 0) {
				switch(asToken[0].uValue.eKeyword) {
					case ID:
						fSendID = 1;
						break;
					case PORT_WR:
						if(asToken[1].eType == NUMBER) {
							PCF8574_Write((unsigned char) asToken[1].uValue.uiNumber);
						}
						else {
							fSendUnknownComm = 1;
						}
						break;
					case PORT_RD:
						PCF8574_Read();
						fSendPort = 1;
						break;
					case MEM_WR:
						if ((asToken[1].eType == NUMBER) && (asToken[2].eType == NUMBER)) {
							MC24LC64_ByteWrite(asToken[1].uValue.uiNumber, asToken[2].uValue.uiNumber);
						}
						else {
							fSendUnknownComm = 1;
						}
						break;
					case MEM_RD:
						if (asToken[1].eType == NUMBER) {
							MC24LC64_RandomRead(asToken[1].uValue.uiNumber);
							fSendMem = 1;
						}
						else {
							fSendUnknownComm = 1;
						}
						break;
				}
			}
			else {
				fSendUnknownComm = 1;
			}
		}
		
		if(Transmitter_GetStatus() == FREE) {
			if(fSendID) {
				fSendID = 0;
				Transmitter_SendString("id LPC2132");
			}
			else if(fSendPort) {
				fSendPort = 0;
				CopyString("ucPCF8574_Input: ", cStringToSend);
				UIntToHexStr((unsigned int) ucPCF8574_Input, cHexStr);
				AppendString(cHexStr, cStringToSend);
				Transmitter_SendString(cStringToSend);
			}
			else if(fSendMem) {
				fSendMem = 0;
				CopyString("ucMC24LC64_MemByte: ", cStringToSend);
				UIntToHexStr((unsigned int) ucMC24LC64_MemByte, cHexStr);
				AppendString(cHexStr, cStringToSend);
				Transmitter_SendString(cStringToSend);
			}
			else if(fSendUnknownComm) {
				fSendUnknownComm = 0;
				Transmitter_SendString("Unknown Command!");
			}
		}
	}
}
