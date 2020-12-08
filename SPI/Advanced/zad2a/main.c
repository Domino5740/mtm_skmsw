//***************************************include's************************************************
#include <LPC213X.H>
#include "spi.h"

//******************************global variables**************************************


//***************************************functions************************************************

void Delay(int iLatency) {
	
	int iLoopCounter;
	
	for (iLoopCounter = 0; iLoopCounter < (iLatency*5455); iLoopCounter++) {}
}

//******************************************main************************************************
int main() {
	
	unsigned char ucCounter;
	
	Port_MCP23S09_InitCSPin();
	SPI_LPC2132_Init();
	

	while(1) {
		Port_MCP23S09_Set(ucCounter);
		ucCounter++;
		Delay(500);
	}
}
