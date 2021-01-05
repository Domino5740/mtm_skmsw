//***************************************include's************************************************
#include <LPC213X.H>
#include "spi.h"
#include "uart.h"
#include "command_decoder.h"
#include "string.h"

//******************************global variables**************************************
extern struct Token asToken[MAX_TOKEN_NR];
extern unsigned char ucTokenNr;
unsigned int uInt;
unsigned char ucTransmitterBuffer;
char cStringToSend[9];
char cReceiverBuffer[RECEIVER_SIZE];

//******************************************main************************************************
int main() {
	
	Port_MCP23S09_InitCSPin();
	SPI_LPC2132_Init();
	UART_InitWithInt(9600);

	while(1) {
		if(eReceiver_GetStatus() == READY) {
			Receiver_GetStringCopy(cReceiverBuffer);
			DecodeMsg(cReceiverBuffer);
			
			if((asToken[0].eType == KEYWORD) && (ucTokenNr > 0) ) {
				switch(asToken[0].uValue.eKeyword) {
					case SPI_PORT_SET:
						Port_MCP23S09_Set(asToken[1].uValue.uiNumber);
					break;
					case SPI_PORT_GET:
						ucTransmitterBuffer = Port_MCP23S09_Get();
						UIntToHexStr((unsigned int) ucTransmitterBuffer, cStringToSend);
						Transmitter_SendString(cStringToSend);
					break;
				}
			}
			else {
				Transmitter_SendString("Unknown command");
			}
		}
	}
}
