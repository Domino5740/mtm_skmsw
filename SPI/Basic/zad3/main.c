//***************************************include's***********************************************
#include <LPC213X.H>
#include <math.h>

//****************************************PINSEL0_bm********************************************
#define P04_SCK0_PINSEL0_bm  1 << 8
#define P05_MISO0_PINSEL0_bm 1 << 10
#define P06_MOSI0_PINSEL0_bm 1 << 12

//*****************************************PIN_bm***********************************************
#define P010_bm 1 << 10

//*****************************************SPI_bm***********************************************
#define SPI_LSBF 0 << 6 // transfer 0 - MSB, 1 - LSB first
#define SPI_MSTR 1 << 5 // 0 - SLAVE, 1 - MASTER
#define SPI_CPOL 0 << 4 // SCK clock polarity: active 0 - HIGH, 1 - LOW
#define SPI_CPHA 0 << 3 // data sampled on 0 - LEADING, 1 - TRAILING SCK clock edge
#define SPI_PCLK_DIVIDER 0x8
#define SPIF 1 << 7 //SPI TRANSFER COMPLETE FLAG - STATUS REGISTER

//*****************************************DAC_bm***********************************************
#define DAC_AB_SELECT	  0 << 7 // 0 - DAC_A, 1 - DAC_B
#define DAC_INPUT_BUFFER  0 << 6 // 0 - unbuffered, 1 - buffered
#define DAC_OUTPUT_GAIN   1 << 5 // 0 - 2x, 1 - 1x
#define DAC_OUTPUT_BUFFER 1 << 4 // 0 - disabled, 1 - enabled
#define DAC_CONFIG_BITS DAC_AB_SELECT | DAC_INPUT_BUFFER | DAC_OUTPUT_GAIN | DAC_OUTPUT_BUFFER

//****************************************CONST***************************************************
#define PI 3.14159265

//*****************************************LUT***************************************************
/*double dsin_values(double dsin_value) {
	static const double lookup[180] =
	
	{
	};
dsin_value = lookup[dsin_value];
return dsin_value;
}8*/



//***************************************functions***********************************************
void DAC_MCP_4921_Set(unsigned int uiVoltage) {
	
	PINSEL0 = P04_SCK0_PINSEL0_bm | P05_MISO0_PINSEL0_bm | P06_MOSI0_PINSEL0_bm;
	IO0DIR = IO0DIR | P010_bm;
	
	S0SPCCR = SPI_PCLK_DIVIDER;
	S0SPCR = 0;
	S0SPCR = SPI_LSBF | SPI_MSTR | SPI_CPOL | SPI_CPHA;
	
	IO0SET = P010_bm;
	IO0CLR = P010_bm;
	
	S0SPDR = DAC_CONFIG_BITS | ((uiVoltage & 0xf00) >> 8);
	while(!(S0SPSR & SPIF)){}
	S0SPDR = (uiVoltage & 0x0ff);
	while(!(S0SPSR & SPIF)){}
		
	IO0SET = P010_bm;
}

void DAC_MCP_4921_Set_mV(unsigned int uiVoltage) {
	
	unsigned int uiVref = 3300;
	unsigned int D = (uiVoltage * 4095) / uiVref;
	
	DAC_MCP_4921_Set(D);
}

//******************************************main************************************************
int main() {
	
	unsigned int i;
	double x;
	unsigned int uiVoltage;
	while(1) {
		
		for(i = 0; i <= 360; i++) {
			x = (i * PI) / 180;
			uiVoltage = 1000 * sin(x) + 1000;
			DAC_MCP_4921_Set_mV(uiVoltage);
		}
	}
}
