# include <LPC213X.H>

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

#define I2C_SLAVE_ADDR 0x20

//******************************************************VIC********************************************************************
#define VIC_I2C_bm    1 << 9
#define VIC_IRQ_EN_bm 1 << 5
#define VIC_I2C_INT 9

//******************************************************I2STAT********************************************************************
#define START_TRANSMITTED 0X08
#define REPEATED_START_TRANSMITTED 0x10
#define SLAR_ACK_RECEIVED 0x18
#define SLAR_NOT_ACK_RECEIVED 0X20
#define DATA_ACK_RECEIVED 0x28
#define DATA_NOT_ACK_RECEIVED 0x30
#define ARBITRATION_LOST 0x38

//*****************************************************LED_bm*********************************************************************
#define LED0_bm 1 << 16
#define LED1_bm 1 << 17
#define LED2_bm 1 << 18
#define LED3_bm 1 << 19
#define LED4_bm 1 << 20
#define LED5_bm 1 << 21
#define LED6_bm 1 << 22
#define LED7_bm 1 << 23

//*****************************************global variables and functions**********************************************************
unsigned char ucI2CData;

__irq void I2C_Interrupt(void) {
	
	int iReadI2Stat = 0;
	iReadI2Stat = I2C0STAT;
	
	switch(iReadI2Stat) {
		case START_TRANSMITTED:
			I2C0DAT = I2C_SLAVE_ADDR << 1; //slave address + 0: W bit
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case REPEATED_START_TRANSMITTED:
			I2C0DAT = I2C_SLAVE_ADDR << 1; //slave address + 0: W bit
			I2C0CONSET = AA_bm;
			I2C0CONCLR = STA_bm;
			break;
		case SLAR_ACK_RECEIVED:
			I2C0DAT = ucI2CData;
			I2C0CONSET = AA_bm;
			break;
		case SLAR_NOT_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			IO1SET = iReadI2Stat << 16;
			break;
		case DATA_ACK_RECEIVED:
			I2C0CONSET = STO_bm | AA_bm;
			break;
		case DATA_NOT_ACK_RECEIVED:
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
	I2C0CONCLR = SI_bm;
	VICVectAddr = 0x00000000;
}

void I2C_init(void) {
	
	PINSEL0 = SCL0_bm | SDA0_bm;
	
	VICIntSelect = 0x00;
	VICVectCntl0 = VIC_IRQ_EN_bm | VIC_I2C_INT;
	VICVectAddr0 = (unsigned long) I2C_Interrupt;
	VICIntEnable = VIC_I2C_bm;
	
	I2C0CONCLR = I2EN_bm | STA_bm | STO_bm | SI_bm | AA_bm;
	I2C0CONSET = I2EN_bm;
	I2C0SCLH = I2C_SCL_HIGH_TIME;
	I2C0SCLL = I2C_SCL_LOW_TIME;
}

void Led_init(void) {
	
	IO1DIR = LED0_bm | LED1_bm | LED2_bm | LED3_bm | LED4_bm | LED5_bm | LED6_bm | LED7_bm;
	IO1CLR = LED0_bm | LED1_bm | LED2_bm | LED3_bm | LED4_bm | LED5_bm | LED6_bm | LED7_bm;
}

//write byte to PCF8574
void PCF8574_Write(unsigned char ucData) {
	
	ucI2CData = ucData;
	I2C0CONSET = STA_bm;
}

void Delay(int iLatency) {
	
	int iLoopCounter;
	
	for (iLoopCounter = 0; iLoopCounter < (iLatency*5455); iLoopCounter++) {}
}

//******************************************************main*********************************************************************
int main(void) {
	
	unsigned char ucNum;
	
	Led_init();
	I2C_init();
	while(1) {
		for(ucNum = 0; ucNum <= 255; ucNum++) {
			PCF8574_Write(ucNum);
			Delay(1000);
		}
	}
}
