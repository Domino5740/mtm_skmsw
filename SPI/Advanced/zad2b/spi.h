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
#define DAC_AB_SELECT_bm	 0 << 7 // 0 - DAC_A, 1 - DAC_B
#define DAC_INPUT_BUFFER_bm  0 << 6 // 0 - unbuffered, 1 - buffered
#define DAC_OUTPUT_GAIN_bm   1 << 5 // 0 - 2x, 1 - 1x
#define DAC_OUTPUT_BUFFER_bm 1 << 4 // 0 - disabled, 1 - enabled
#define DAC_CONFIG_BITS DAC_AB_SELECT_bm | DAC_INPUT_BUFFER_bm | DAC_OUTPUT_GAIN_bm | DAC_OUTPUT_BUFFER_bm

//*****************************************MCP23S09*************************************************
#define MCP23S09_REG_WRITE_OPCODE 0x40
#define MCP23S09_REG_READ_OPCODE  0x41
#define MCP23S09_IO0DIR_ADDR	  0x00
#define MCP23S09_GPIO_ADDR		  0x09
#define MCP23S09_IO0DIR_OUT		  0x00
#define MCP23S09_IO0DIR_IN		  0xff

//****************************************structs*************************************************
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

//********************************function prototypes**********************************************
void DAC_MCP_4921_Set(unsigned int uiVoltage);
void DAC_MCP_4921_Set_mV(unsigned int uiVoltage);
void SPI_ConfigMaster(struct SPI_FrameParams sSPI_FrameParams);
void SPI_ExecuteTransaction(struct SPI_TransactionParams sSPI_TransactionParams);
void DAC_MCP_4921_InitCSPin(void);
void SPI_LPC2132_Init(void);
void DAC_MCP_4921_SetAdv(unsigned int uiVoltage);
void DAC_MCP_4921_SetAdv_mV(unsigned int uiVoltage);
void Port_MCP23S09_InitCSPin(void);
void Port_MCP23S09_Set(unsigned char ucData);
unsigned char Port_MCP23S09_Get(void);
