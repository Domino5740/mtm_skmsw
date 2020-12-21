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

void SPI_ExecuteTransaction(struct SPI_TransactionParams sSPI_TransactionParams){
	
	unsigned char ucMaxTransaction = 0;
	unsigned char ucRxCounter = 0;
	unsigned char ucTxCounter = 0;
	unsigned char ucTransactionCounter = 0;
	unsigned char ucReceivedByte = 0;
	unsigned char ucByteToTransmit = 0;
	
	unsigned char ucMaxTxTransaction = sSPI_TransactionParams.ucNrOfBytesForTx + sSPI_TransactionParams.ucTxBytesOffset;
	unsigned char ucMaxRxTransaction = sSPI_TransactionParams.ucNrOfBytesForRx + sSPI_TransactionParams.ucRxBytesOffset;

	if(ucMaxTxTransaction > ucMaxRxTransaction) {
		ucMaxTransaction = ucMaxTxTransaction;
	}
	else {
		ucMaxTransaction = ucMaxRxTransaction;
	}
	
	for(ucTransactionCounter = 0; ucTransactionCounter < ucMaxTransaction; ucTransactionCounter++) {
		
		if((ucTransactionCounter >= sSPI_TransactionParams.ucTxBytesOffset) & (ucTxCounter < sSPI_TransactionParams.ucNrOfBytesForTx)) {
			ucByteToTransmit = *(sSPI_TransactionParams.pucBytesForTx + ucTxCounter);
			ucTxCounter++;
		}
		else {
			ucByteToTransmit = 0;
		} 
		
		S0SPDR = ucByteToTransmit;
		while((S0SPSR & SPIF) == 0) {}
		ucReceivedByte = S0SPDR;
			
		if((ucTransactionCounter >= sSPI_TransactionParams.ucRxBytesOffset) & (ucRxCounter < sSPI_TransactionParams.ucNrOfBytesForRx)){
			*(sSPI_TransactionParams.pucBytesForRx + ucRxCounter) = ucReceivedByte;
			ucRxCounter++;
		}
	}
}

void DAC_MCP_4921_InitCSPin(void){
	IO0DIR = IO0DIR | CS_P010_bm;
	IO0SET = CS_P010_bm;
}

void SPI_LPC2132_Init(void) {
	
	struct SPI_FrameParams sSPI_FrameParams;
	
	//ustalenie parametrow SPI
	sSPI_FrameParams.ucCpha  = SPI_CPHA_bm;
	sSPI_FrameParams.ucCpol  = SPI_CPOL_bm;
	sSPI_FrameParams.ucClsbf = SPI_LSBF_bm;
	sSPI_FrameParams.ClkDivider = SPI_PCLK_DIVIDER;
	
	//wpisanie konfiguracji do rejestrow
	SPI_ConfigMaster(sSPI_FrameParams);
}

void DAC_MCP_4921_SetAdv(unsigned int uiVoltage) {
	
	struct SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucDACWord[2];
	
	//ustalenie parametrow transakcji
	sSPI_TransactionParams.ucNrOfBytesForTx = 2;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	sSPI_TransactionParams.pucBytesForTx = ucDACWord;
	
	//przygotowanie danych do wyslania
	ucDACWord[0] = DAC_CONFIG_BITS | ((uiVoltage & 0xf00) >> 8);
	ucDACWord[1] = uiVoltage & 0x0ff;
	
	//wykonanie transakcji
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
	
	//ustalenie parametrow ramki
	sSPI_TransactionParams.pucBytesForTx = ucDataToSend;
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	
	//przygotowanie danych do wyslania
	ucDataToSend[0] = MCP23S09_REG_WRITE_OPCODE;
	ucDataToSend[1] = MCP23S09_IO0DIR_ADDR;
	ucDataToSend[2] = MCP23S09_IO0DIR_OUT;
	
	//wykonanie transakcji
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
	//przygotowanie danych do wyslania
	ucDataToSend[0] = MCP23S09_REG_WRITE_OPCODE;
	ucDataToSend[1] = MCP23S09_GPIO_ADDR;
	ucDataToSend[2] = ucData;
	
	//wykonanie transakcji
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
}

unsigned char Port_MCP23S09_Get(void) {
	
	struct SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucDataToSend[3];
	unsigned char ucDataReceived = 0;
	
	//ustalenie parametrow ramki
	sSPI_TransactionParams.pucBytesForTx = ucDataToSend;
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	
	//przygotowanie danych do wyslania
	ucDataToSend[0] = MCP23S09_REG_WRITE_OPCODE;
	ucDataToSend[1] = MCP23S09_IO0DIR_ADDR;
	ucDataToSend[2] = MCP23S09_IO0DIR_IN;
	
	//wykonanie transakcji
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
	//ustalenie parametrow ramki nr 2
	sSPI_TransactionParams.ucNrOfBytesForTx = 2;
	sSPI_TransactionParams.pucBytesForRx = &ucDataReceived;
	sSPI_TransactionParams.ucNrOfBytesForRx = 1;
	sSPI_TransactionParams.ucRxBytesOffset = 2;
	
	//przygotowanie danych do wyslania
	ucDataToSend[0] = MCP23S09_REG_READ_OPCODE;
	ucDataToSend[1] = MCP23S09_GPIO_ADDR;
	
	//wykonanie transakcji
	IO0CLR = CS_P010_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);
	IO0SET = CS_P010_bm;
	
	return ucDataReceived;
}
