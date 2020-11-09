#include <LPC213X.H>

//****************************************************PINSEL0_bm*****************************************************************
#define SCL0_bm 1 << 4
#define SDA0_bm 1 << 6

//********************************************************I2C********************************************************************
#define I2EN_bm 1 << 6  //I2C interface 1 = enable, 0 = disable
#define STA_bm  1 << 5	 //START flag
#define STO_bm  1 << 4  //STOP flag
#define SI_bm   1 << 3  //I2C Interrupt flag
#define AA_bm   1 << 2  //Assert Acknowledge flag

#define I2C_SCL_HIGH_TIME 0x80
#define I2C_SCL_LOW_TIME  0x80
//I2cbit_f = fPCLK/(I2CSCLH + I2CSCLL)

#define I2C_SLAVE_ADDR 0x20

//******************************************************VIC********************************************************************
#define VIC_I2C_bm 1 << 9
#define VIC_IRQ_EN_bm 1 << 5
#define VIC_I2C_INT 0x9

//******************************************************I2STAT********************************************************************
#define START_TRANSMITTED 0X08
#define REPEATED_START_TRANSMITTED 0x10
#define SLAR_ACK_RECEIVED 0x18
#define SLAR_NOT_ACK_RECEIVED 0X20
#define DATA_ACK_RECEIVED 0x28
#define DATA_NOT_ACK_RECEIVED 0x30
#define ARBITRATION_LOST 0x38

//*****************************************global variables and functions**********************************************************
unsigned char ucI2CData;

__irq void I2C_Interrupt(void) {
	
	int iReadI2Stat = 0;
	iReadI2Stat = I2STAT;
	I2CONCLR = STA_bm;
	
	switch(iReadI2Stat) {
		case START_TRANSMITTED:
			I2DAT = I2C_SLAVE_ADDR << 1; //slave address + 0: W bit
			I2CONSET = AA_bm;
			break;
		case REPEATED_START_TRANSMITTED:
			I2DAT = I2C_SLAVE_ADDR << 1; //slave address + 0: W bit
			I2CONSET = AA_bm;
			break;
		case SLAR_ACK_RECEIVED:
			I2DAT = ucI2CData;
			I2CONSET = AA_bm;
			break;
		case SLAR_NOT_ACK_RECEIVED:
			I2CONSET = STO_bm | AA_bm;
			break;
		case DATA_ACK_RECEIVED:
			I2CONSET = STO_bm | AA_bm;
			break;
		case DATA_NOT_ACK_RECEIVED:
			I2CONSET = STO_bm | AA_bm;
			break;
		case ARBITRATION_LOST:
			I2CONSET = STA_bm | AA_bm;
			break;
	}
	I2CONCLR = SI_bm;
}

void I2C_init(void) {
	
	PINSEL0 = SCL0_bm | SDA0_bm;
	
	I2CONCLR = I2EN_bm | STA_bm | STO_bm | SI_bm | AA_bm;
	I2CONSET = I2EN_bm;
	I2SCLH = I2C_SCL_HIGH_TIME;
	I2SCLL = I2C_SCL_LOW_TIME;
	

	VICIntSelect = 0x00;
	VICVectCntl0 = VIC_IRQ_EN_bm | VIC_I2C_INT;
	VICVectAddr0 = (unsigned) I2C_Interrupt;
	VICIntEnable = VIC_I2C_bm;
}

void PCF8574_Write(unsigned char ucData) {
	ucI2CData = ucData;
	I2CONSET = STA_bm;
}

//******************************************************main*********************************************************************
int main(void) {
	unsigned char ucNum;
	while(1) {
		for(ucNum = 0; ucNum <= 255; ucNum++) {
			PCF8574_Write(ucNum);
		}
	}
}
