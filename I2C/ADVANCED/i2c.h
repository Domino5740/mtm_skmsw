enum I2CTransmissionMode {TX, RX, RX_AFTER_TX, TX_AFTER_RX };

struct I2C_Params {
	enum I2CTransmissionMode eI2CTransmissionMode;
	
	unsigned char ucSlaveAddress;
	
	unsigned char *pucBytesForTx;
	unsigned char ucNrOfBytesForTx;
	
	unsigned char *pucBytesForRx;
	unsigned char ucNrOfBytesForRx;
	
	unsigned char ucDone;
};

enum I2CStatus {WORKING, DONE};

enum I2CStatus I2C_GetStatus(void);

void I2C_waitForDoneStatus(void);
void I2C_Init(void);
void I2C_MASTER_TX_MODE(void);
void I2C_MASTER_RX_MODE(void);
void I2C_MASTER_TX_AFTER_RX_MODE(void);
void I2C_MASTER_RX_AFTER_TX_MODE(void);

void ExecuteTransaction(void);
__irq void I2C_Interrupt(void);

void PCF8574_Write(unsigned char ucData);
void PCF8574_Read(void);
void MC24LC64_ByteWrite(unsigned int uiWordAddress, unsigned char ucData);
void MC24LC64_RandomRead(unsigned int uiWordAddress);
