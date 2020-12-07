//***************************************include's************************************************
#include <LPC213X.H>

//****************************************PINSEL0_bm**********************************************
#define P04_SCK0_PINSEL0_bm  1 << 8
#define P05_MISO0_PINSEL0_bm 1 << 10
#define P06_MOSI0_PINSEL0_bm 1 << 12

//*****************************************PIN_bm*************************************************
#define CS_P010_bm 1 << 10

//*****************************************SPI_bm*************************************************
#define SPI_LSBF_bm 0 << 6 // transfer 0 - MSB, 1 - LSB first
#define SPI_MSTR_bm 1 << 5 // 0 - SLAVE, 1 - MASTER
#define SPI_CPOL_bm 0 << 4 // SCK clock polarity: active 0 - HIGH, 1 - LOW
#define SPI_CPHA_bm 0 << 3 // data sampled on 0 - LEADING, 1 - TRAILING SCK clock edge
#define SPI_PCLK_DIVIDER 0x8
#define SPIF 1 << 7 //SPI TRANSFER COMPLETE FLAG - STATUS REGISTER

//*****************************************DAC_bm*************************************************
#define DAC_AB_SELECT_bm	  0 << 7 // 0 - DAC_A, 1 - DAC_B
#define DAC_INPUT_BUFFER_bm  0 << 6 // 0 - unbuffered, 1 - buffered
#define DAC_OUTPUT_GAIN_bm   1 << 5 // 0 - 2x, 1 - 1x
#define DAC_OUTPUT_BUFFER_bm 1 << 4 // 0 - disabled, 1 - enabled
#define DAC_CONFIG_BITS DAC_AB_SELECT_bm | DAC_INPUT_BUFFER_bm | DAC_OUTPUT_GAIN_bm | DAC_OUTPUT_BUFFER_bm

//**************************************LUT TABLE*************************************************
unsigned int get_uisin_dac_value (unsigned int uiValue) {
	static const unsigned int lookup[360] =
	{   1000, 1017, 1034, 1052, 1069, 1087, 1104, 1121, 1139, 1156, 
		1173, 1190, 1207, 1224, 1241, 1258, 1275, 1292, 1309, 1325, 
		1342, 1358, 1374, 1390, 1406, 1422, 1438, 1453, 1469, 1484, 
		1499, 1515, 1529, 1544, 1559, 1573, 1587, 1601, 1615, 1629, 
		1642, 1656, 1669, 1681, 1694, 1707, 1719, 1731, 1743, 1754, 
		1766, 1777, 1788, 1798, 1809, 1819, 1829, 1838, 1848, 1857, 
		1866, 1874, 1882, 1891, 1898, 1906, 1913, 1920, 1927, 1933, 
		1939, 1945, 1951, 1956, 1961, 1965, 1970, 1974, 1978, 1981, 
		1984, 1987, 1990, 1992, 1994, 1996, 1997, 1998, 1999, 1999, 
		2000, 1999, 1999, 1998, 1997, 1996, 1994, 1992, 1990, 1987, 
		1984, 1981, 1978, 1974, 1970, 1965, 1961, 1956, 1951, 1945, 
		1939, 1933, 1927, 1920, 1913, 1906, 1898, 1891, 1882, 1874, 
		1866, 1857, 1848, 1838, 1829, 1819, 1809, 1798, 1788, 1777, 
		1766, 1754, 1743, 1731, 1719, 1707, 1694, 1681, 1669, 1656, 
		1642, 1629, 1615, 1601, 1587, 1573, 1559, 1544, 1529, 1515, 
		1500, 1484, 1469, 1453, 1438, 1422, 1406, 1390, 1374, 1358, 
		1342, 1325, 1309, 1292, 1275, 1258, 1241, 1224, 1207, 1190, 
		1173, 1156, 1139, 1121, 1104, 1087, 1069, 1052, 1034, 1017, 
		1000, 982, 965, 947, 930, 912, 895, 878, 860, 843, 826, 809, 
		792, 775, 758, 741, 724, 707, 690, 674, 657, 641, 625, 609, 
		593, 577, 561, 546, 530, 515, 500, 484, 470, 455, 440, 426, 
		412, 398, 384, 370, 357, 343, 330, 318, 305, 292, 280, 268, 
		256, 245, 233, 222, 211, 201, 190, 180, 170, 161, 151, 142, 
		133, 125, 117, 108, 101, 93, 86, 79, 72, 66, 60, 54, 48, 43, 
		38, 34, 29, 25, 21, 18, 15, 12, 9, 7, 5, 3, 2, 1, 0, 0, 0, 
		0, 1, 2, 3, 5, 7, 9, 12, 15, 18, 21, 25, 29, 34, 38, 43, 48, 54, 60, 66, 
		72, 79, 86, 93, 101, 108, 117, 125, 133, 142, 151, 161, 170, 180, 190, 201, 
		211, 222, 233, 245, 256, 268, 280, 292, 305, 318, 330, 343, 357, 370, 384, 
		398, 412, 426, 440, 455, 470, 484, 499, 515, 530, 546, 561, 577, 593, 609, 
		625, 641, 657, 674, 690, 707, 724, 741, 758, 775, 792, 809, 826, 843, 860, 
		878, 895, 912, 930, 947, 965, 982, 999};
		
	uiValue = lookup[uiValue];
	return uiValue;
}

//******************************global variables and structs**************************************
struct SPI_FrameParams {
	unsigned char ucCpha;
	unsigned char ucCpol;
	unsigned char ucClsbf;
	unsigned char ClkDivider;
};

struct SPI_TransactionParams {
	
	unsigned char *pucBytesForTx;
	unsigned char ucNrOfBytesForTx;
	unsigned char ucTxBytesOffset;
	
	unsigned char *pucBytesForRx;
	unsigned char ucNrOfBytesForRx;
	unsigned char ucRxBytesOffset;
};

//***************************************functions************************************************
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
		S0SPDR = *(sSPI_TransactionParams.pucBytesForTx + sSPI_TransactionParams.ucTxBytesOffset);
		while(!(S0SPSR & SPIF)){}
	}
	
	for(i = 0; i < sSPI_TransactionParams.ucNrOfBytesForRx; i++) {
		*(sSPI_TransactionParams.pucBytesForTx + sSPI_TransactionParams.ucRxBytesOffset) = S0SPDR;
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

//******************************************main************************************************
int main() {

	int i;
	
	DAC_MCP_4921_InitCSPin();
	SPI_LPC2132_Init();
	
	while(1) {
		for (i = 0; i < 360; i++) {
			DAC_MCP_4921_SetAdv_mV(get_uisin_dac_value(i));
		}
	}
}
