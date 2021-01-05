#include "i2c.h"
#include <LPC213X.H>

//****************************************************PINSEL0_bm*****************************************************************
#define SCL0_bm 1 << 4
#define SDA0_bm 1 << 6

//********************************************************I2C********************************************************************
#define I2EN_bm 1 << 6  //I2C interface 1 = enable, 0 = disable
#define STA_bm  1 << 5	//START flag
#define STO_bm  1 << 4  //STOP flag
#define SI_bm   1 << 3  //I2C Interrupt flag
#define AA_bm   1 << 2  //Assert Acknowledge flag

#define I2C_SCL_HIGH_TIME 0x80
#define I2C_SCL_LOW_TIME  0x80

#define W_bm 0
#define R_bm 1

#define PCF8574_SLAVE_ADDR 0x20
#define MC24LC64_SLAVE_ADDR 0x50

//******************************************************VIC********************************************************************
#define VIC_I2C_bm    1 << 9
#define VIC_IRQ_EN_bm 1 << 5
#define VIC_I2C_INT 9

//******************************************************I2STAT********************************************************************
#define START_TRANSMITTED 0X08
#define REPEATED_START_TRANSMITTED 0x10
#define SLAW_ACK_RECEIVED 0x18
#define SLAW_NOT_ACK_RECEIVED 0X20
#define DATA_TRANSMITTED_ACK_RECEIVED 0x28
#define DATA_TRANSMITTED_NOT_ACK_RECEIVED 0x30
#define ARBITRATION_LOST 0x38
#define SLAR_ACK_RECEIVED 0x40
#define SLAR_NOT_ACK_RECEIVED 0x48
#define DATA_REC_ACK_RETURNED 0x50
#define DATA_REC_NOT_ACK_RETURNED 0x58

//**************************************************global variables***************************************************************

struct I2C_Params sI2C_Params;
unsigned char ucTxBytesCounter;
unsigned char ucRxBytesCounter;
extern unsigned char ucPCF8574_Input;
extern unsigned char ucMC24LC64_MemByte;

//****************************************************functions********************************************************************

void I2C_MASTER_TX_MODE(void) {

	int iReadI2Stat = 0;
	iReadI2Stat = I2C0STAT;

	switch(iReadI2Stat) {
			case START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | W_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case REPEATED_START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | W_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAW_ACK_RECEIVED:
			I2C0DAT = *(sI2C_Params.pucBytesForTx + ucTxBytesCounter);
			ucTxBytesCounter++;
			I2C0CONSET = AA_bm;
			break;
		case SLAW_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_TRANSMITTED_ACK_RECEIVED:
			if(ucTxBytesCounter < sI2C_Params.ucNrOfBytesForTx) {
				I2C0DAT = *(sI2C_Params.pucBytesForTx + ucTxBytesCounter);
				ucTxBytesCounter++;
			}
			else {
				I2C0CONSET = STO_bm;
				sI2C_Params.ucDone = 1;
			}
			I2C0CONSET = AA_bm;
			break;
		case DATA_TRANSMITTED_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case ARBITRATION_LOST:
			I2C0CONSET = STA_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		default:
			IO1SET = iReadI2Stat << 16;
			break;
	}
}

void I2C_MASTER_RX_MODE(void) {

	int iReadI2Stat = 0;
	iReadI2Stat = I2C0STAT;

	switch(iReadI2Stat) {
		case START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | R_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case REPEATED_START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | R_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAR_ACK_RECEIVED:
			I2C0CONSET = AA_bm;
			break;
		case SLAR_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_REC_ACK_RETURNED:
			*(sI2C_Params.pucBytesForRx + ucRxBytesCounter) = I2C0DAT;
			ucRxBytesCounter++;
			if(ucRxBytesCounter < sI2C_Params.ucNrOfBytesForRx) {
				I2C0CONSET = AA_bm;
			}
			else {
				I2C0CONCLR = AA_bm;
			}
			break;
		case DATA_REC_NOT_ACK_RETURNED:
			*(sI2C_Params.pucBytesForRx + ucRxBytesCounter) = I2C0DAT;
			I2C0CONSET = STO_bm | AA_bm;
			sI2C_Params.ucDone = 1;
			break;
		default:
			IO1SET = iReadI2Stat << 16;
			break;
	}
}

void I2C_MASTER_TX_AFTER_RX_MODE(void) {

	int iReadI2Stat = 0;
	iReadI2Stat = I2C0STAT;

	switch(iReadI2Stat) {
		case START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | R_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAR_ACK_RECEIVED:
			I2C0CONSET = AA_bm;
			break;
		case SLAR_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_REC_ACK_RETURNED:
			*(sI2C_Params.pucBytesForRx + ucRxBytesCounter) = I2C0DAT;
			ucRxBytesCounter++;
			if(ucRxBytesCounter < sI2C_Params.ucNrOfBytesForRx) {
				I2C0CONSET = AA_bm;
			}
			else {
				I2C0CONCLR = AA_bm;
			}
			break;
		case DATA_REC_NOT_ACK_RETURNED:
			*(sI2C_Params.pucBytesForRx + ucRxBytesCounter) = I2C0DAT;
			I2C0CONSET = STA_bm | AA_bm;
			break;
		case REPEATED_START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | W_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAW_ACK_RECEIVED:
			I2C0DAT = *(sI2C_Params.pucBytesForTx + ucTxBytesCounter);
			ucTxBytesCounter++;
			I2C0CONSET = AA_bm;
			break;
		case SLAW_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_TRANSMITTED_ACK_RECEIVED:
			if(ucTxBytesCounter < sI2C_Params.ucNrOfBytesForTx) {
				I2C0DAT = *(sI2C_Params.pucBytesForTx + ucTxBytesCounter);
				ucTxBytesCounter++;
			}
			else {
				I2C0CONSET = STO_bm;
				sI2C_Params.ucDone = 1;
			}
			I2C0CONSET = AA_bm;
			break;
		case DATA_TRANSMITTED_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case ARBITRATION_LOST:
			I2C0CONSET = STA_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		default:
			IO1SET = iReadI2Stat << 16;
			break;
	}
}

void I2C_MASTER_RX_AFTER_TX_MODE(void) {

	int iReadI2Stat = 0;
	iReadI2Stat = I2C0STAT;

	switch(iReadI2Stat) {
			case START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | W_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAW_ACK_RECEIVED:
			I2C0DAT = *(sI2C_Params.pucBytesForTx + ucTxBytesCounter);
			ucTxBytesCounter++;
			I2C0CONSET = AA_bm;
			break;
		case SLAW_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_TRANSMITTED_ACK_RECEIVED:
			if(ucTxBytesCounter < sI2C_Params.ucNrOfBytesForTx) {
				I2C0DAT = *(sI2C_Params.pucBytesForTx + ucTxBytesCounter);
				ucTxBytesCounter++;
				I2C0CONSET = AA_bm;
			}
			else {
				I2C0CONSET = STA_bm;
			}
			break;
		case DATA_TRANSMITTED_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case ARBITRATION_LOST:
			I2C0CONSET = STO_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case REPEATED_START_TRANSMITTED:
			I2C0DAT = (sI2C_Params.ucSlaveAddress << 1) | R_bm;
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAR_ACK_RECEIVED:
			I2C0CONSET = AA_bm;
			break;
		case SLAR_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_REC_ACK_RETURNED:
			*(sI2C_Params.pucBytesForRx + ucRxBytesCounter) = I2C0DAT;
			ucRxBytesCounter++;
			if(ucRxBytesCounter < sI2C_Params.ucNrOfBytesForRx) {
				I2C0CONSET = AA_bm;
			}
			else {
				I2C0CONCLR = AA_bm;
			}
			break;
		case DATA_REC_NOT_ACK_RETURNED:
			*(sI2C_Params.pucBytesForRx + ucRxBytesCounter) = I2C0DAT;
			I2C0CONSET = STO_bm | AA_bm;
			sI2C_Params.ucDone = 1;
			break;
		default:
			IO1SET = iReadI2Stat << 16;
			break;
	}
}

void I2C_Init(void) {
	
	PINSEL0 = PINSEL0 | SCL0_bm | SDA0_bm;
	
	VICIntSelect = 0x00;
	VICVectCntl0 = VIC_IRQ_EN_bm | VIC_I2C_INT;
	VICVectAddr0 = (unsigned long) I2C_Interrupt;
	VICIntEnable = VIC_I2C_bm;
	
	I2C0CONCLR = I2EN_bm | STA_bm | STO_bm | SI_bm | AA_bm;
	I2C0CONSET = I2EN_bm;
	I2C0SCLH = I2C_SCL_HIGH_TIME;
	I2C0SCLL = I2C_SCL_LOW_TIME;
}

void ExecuteTransaction() {
	ucTxBytesCounter = 0;
	ucRxBytesCounter = 0;
	sI2C_Params.ucDone = 0;
	I2C0CONSET = STA_bm;
}

__irq void I2C_Interrupt(void) {
	
	switch(sI2C_Params.eI2CTransmissionMode) {
		case TX:
			I2C_MASTER_TX_MODE();
		break;
		case RX:
			I2C_MASTER_RX_MODE();
		break;
		case TX_AFTER_RX:
			I2C_MASTER_TX_AFTER_RX_MODE();
		break;
		case RX_AFTER_TX:
			I2C_MASTER_RX_AFTER_TX_MODE();
		break;
		}

	I2C0CONCLR = SI_bm;
	VICVectAddr = 0x00000000;	
}

enum I2CStatus I2C_GetStatus(void){
	if (sI2C_Params.ucDone == 0){
		return WORKING;
	}
	else {
		return DONE;
	}
}

void I2C_waitForDoneStatus(void) {
	while(I2C_GetStatus() == WORKING) {}
}

//write byte to PCF8574
void PCF8574_Write(unsigned char ucData) {
	
	unsigned char ucBytesForTx = ucData;
	
	//set transaction params
	sI2C_Params.eI2CTransmissionMode = TX;
	sI2C_Params.pucBytesForTx = &ucBytesForTx;
	sI2C_Params.ucNrOfBytesForTx = 1;
	sI2C_Params.ucSlaveAddress = PCF8574_SLAVE_ADDR;
	sI2C_Params.ucNrOfBytesForRx = 0;

	ExecuteTransaction();
	I2C_waitForDoneStatus();
}

//read byte from PCF8574
void PCF8574_Read(void) {
	
	unsigned char pucBytesForRx[1];
	
	//set transaction params
	sI2C_Params.eI2CTransmissionMode = RX;
	sI2C_Params.pucBytesForRx = pucBytesForRx;
	sI2C_Params.ucNrOfBytesForRx = 1;
	sI2C_Params.ucSlaveAddress = PCF8574_SLAVE_ADDR;
	sI2C_Params.ucNrOfBytesForTx = 0;

	ExecuteTransaction();
	I2C_waitForDoneStatus();
	ucPCF8574_Input = sI2C_Params.pucBytesForRx[0];
}

//write byte to MC24LC64
void MC24LC64_ByteWrite(unsigned int uiWordAddress, unsigned char ucData) {
	
	unsigned char pucBytesForTx[3];
	
	pucBytesForTx[0] = (uiWordAddress & 0xFF00) >> 8;
	pucBytesForTx[1] = (uiWordAddress & 0x00FF);
	pucBytesForTx[2] = ucData;

	//set transaction params
	sI2C_Params.eI2CTransmissionMode = TX;
	sI2C_Params.pucBytesForTx = pucBytesForTx;
	sI2C_Params.ucNrOfBytesForTx = 3;
	sI2C_Params.ucSlaveAddress = MC24LC64_SLAVE_ADDR;
	sI2C_Params.ucNrOfBytesForRx = 0;

	ExecuteTransaction();
	I2C_waitForDoneStatus();
}

//read byte from MC24LC64
void MC24LC64_RandomRead(unsigned int uiWordAddress) {
	
	unsigned char pucBytesForTx[2];
	unsigned char pucBytesForRx[1];
	
	pucBytesForTx[0] = (uiWordAddress & 0xFF00) >> 8;
	pucBytesForTx[1] = (uiWordAddress & 0x00FF);

	//set transaction params
	sI2C_Params.eI2CTransmissionMode = RX_AFTER_TX;
	sI2C_Params.pucBytesForTx = pucBytesForTx;
	sI2C_Params.ucNrOfBytesForTx = 2;
	sI2C_Params.ucSlaveAddress = MC24LC64_SLAVE_ADDR;
	sI2C_Params.pucBytesForRx = pucBytesForRx;
	sI2C_Params.ucNrOfBytesForRx = 1;

	ExecuteTransaction();
	I2C_waitForDoneStatus();
	ucMC24LC64_MemByte = sI2C_Params.pucBytesForRx[0]; 
}
