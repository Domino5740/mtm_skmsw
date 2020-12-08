//***************************************include's************************************************
#include <LPC213X.H>
#include "spi.h"
#include "uart.h"
#include "command_decoder.h"
#define LED0_bm (1<<16)
#define LED1_bm (1<<17)
#define LED2_bm (1<<18)
#define LED3_bm (1<<19)
//******************************global variables**************************************
extern struct Token asToken[MAX_TOKEN_NR];
extern unsigned char ucTokenNr;
char cTransmitterBuffer[TRANSMITTER_SIZE];
char cReceiverBuffer[RECEIVER_SIZE];

//***************************************functions************************************************
void Delay(int iLatency) {
	
	int iLoopCounter;
	
	for (iLoopCounter = 0; iLoopCounter < (iLatency*5455); iLoopCounter++) {}
}

//******************************************main************************************************
int main() {
	
	Port_MCP23S09_InitCSPin();
	SPI_LPC2132_Init();
	UART_InitWithInt(9600);

	while(1) {
		if(eReceiver_GetStatus() == READY) {
			Receiver_GetStringCopy(cReceiverBuffer);
			DecodeMsg(cReceiverBuffer);
			//Port_MCP23S09_Set(ucCounter);
			
			if((asToken[0].eType == KEYWORD) && (ucTokenNr > 0) ) {
				switch(asToken[0].uValue.eKeyword) {
					case SPI_PORT_SET:
						Port_MCP23S09_Set(asToken[1].uValue.uiNumber);
					break;
					case SPI_PORT_GET:
						IO1DIR = IO1DIR | LED0_bm | LED1_bm | LED2_bm | LED3_bm;
						IO1SET = LED3_bm;
					break;
				}
			}
		}
	}
}
