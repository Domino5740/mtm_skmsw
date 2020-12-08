#include "spi.h"
#include <LPC213X.H>

void DAC_MCP_4921_Set(unsigned int uiVoltage) {
	
	PINSEL0 = P04_SCK0_PINSEL0_bm | P05_MISO0_PINSEL0_bm | P06_MOSI0_PINSEL0_bm;
	IO0DIR = IO0DIR | CS_P010_bm;
	
	S0SPCCR = SPI_PCLK_DIVIDER;
	S0SPCR = 0;
	S0SPCR = SPI_LSBF_bm | SPI_MSTR_bm | SPI_CPOL_bm | SPI_CPHA_bm;
	
	IO0SET = CS_P010_bm;
	IO0CLR = CS_P010_bm;
	
	S0SPDR = DAC_CONFIG_BITS | ((uiVoltage & 0xf00) >> 8);
	while(!(S0SPSR & SPIF)){}
	S0SPDR = (uiVoltage & 0x0ff);
	while(!(S0SPSR & SPIF)){}
		
	IO0SET = CS_P010_bm;
}

void DAC_MCP_4921_Set_mV(unsigned int uiVoltage) {
	
	unsigned int uiVref = 3300;
	unsigned int D = (uiVoltage * 4095) / uiVref;
	
	DAC_MCP_4921_Set(D);
}

void SPI_ConfigMaster(struct SPI_FrameParams sSPI_FrameParams) {
	
	PINSEL0 = P04_SCK0_PINSEL0_bm | P05_MISO0_PINSEL0_bm | P06_MOSI0_PINSEL0_bm;
	S0SPCR = sSPI_FrameParams.ucCpol | sSPI_FrameParams.ucCpha | sSPI_FrameParams.ucClsbf | SPI_MSTR_bm;
	S0SPCCR = sSPI_FrameParams.ClkDivider;
}

void SPI_ExecuteTransaction(struct SPI_TransactionParams sSPI_TransactionParams) {
	
	unsigned char  i;
	
	for(i = 0; i < sSPI_TransactionParams.ucNrOfBytesForTx; i++) {
		S0SPDR = *(sSPI_TransactionParams.pucBytesForTx + sSPI_TransactionParams.ucTxBytesOffset + i);
		while(!(S0SPSR & SPIF)){}
	}
	
	for(i = 0; i < sSPI_TransactionParams.ucNrOfBytesForRx; i++) {
		*(sSPI_TransactionParams.pucBytesForTx + sSPI_TransactionParams.ucRxBytesOffset + i) = S0SPDR;
		while(!(S0SPSR & SPIF)){}
	}
	
}

void DAC_MCP_4921_InitCSPin(void){
	IO0DIR = IO0DIR | CS_P010_bm;
	IO0SET = CS_P010_bm;
}

void SPI_LPC2132_Init(void) {
	
	struct SPI_FrameParams sSPI_FrameParams;
	
	sSPI_FrameParams.ucCpha  = SPI_CPHA_bm;
	sSPI_FrameParams.ucCpol  = SPI_CPOL_bm;
	sSPI_FrameParams.ucClsbf = SPI_LSBF_bm;
	sSPI_FrameParams.ClkDivider = SPI_PCLK_DIVIDER;
	SPI_ConfigMaster(sSPI_FrameParams);
}

void DAC_MCP_4921_SetAdv(unsigned int uiVoltage) {
	
	struct SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucDACWord[2];
	
	sSPI_TransactionParams.ucNrOfBytesForTx = 2;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	sSPI_TransactionParams.pucBytesForTx = ucDACWord;
	
	ucDACWord[0] = DAC_CONFIG_BITS | ((uiVoltage & 0xf00) >> 8);
	ucDACWord[1] = uiVoltage & 0x0ff;
	
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
}

void DAC_MCP_4921_SetAdv_mV(unsigned int uiVoltage) {
	
	unsigned int uiVref = 3300;
	unsigned int D = (uiVoltage * 4095) / uiVref;
	
	DAC_MCP_4921_SetAdv(D);
}

void Port_MCP23S09_InitCSPin(void) {
	IO0DIR = IO0DIR | CS_P010_bm;
	IO0SET = CS_P010_bm;
}

void Port_MCP23S09_Set(unsigned char ucData) {
	
	struct SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucDataToSend[3];
	
	sSPI_TransactionParams.pucBytesForTx = ucDataToSend;
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	
	ucDataToSend[0] = 0x40;
	ucDataToSend[1] = 0x00;
	ucDataToSend[2] = 0x00;
	
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
	ucDataToSend[0] = 0x40;
	ucDataToSend[1] = 0x09;
	ucDataToSend[2] = ucData;
	
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
}

unsigned char Port_MCP23S09_Get(void) {
	
	struct SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucDataToSend[5];
	unsigned char ucDataReceived;
	
	sSPI_TransactionParams.pucBytesForTx = ucDataToSend;
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	
	ucDataToSend[0] = 0x40;
	ucDataToSend[1] = 0x00;
	ucDataToSend[2] = 0xff;
	
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
	
	sSPI_TransactionParams.ucNrOfBytesForTx = 2;
	
	sSPI_TransactionParams.pucBytesForRx = &ucDataReceived;
	sSPI_TransactionParams.ucNrOfBytesForRx = 1;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	
	
	ucDataToSend[0] = 0x41;
	ucDataToSend[1] = 0x09;
	
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
	return ucDataReceived;
}
