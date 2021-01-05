#include <LPC210X.H>
#include "uart.h"
#include "string.h"

#define NULL 00

/************ UART ************/

#define mP0_1_RX0_PIN_MODE 						   0x00000004
#define mP0_0_TX0_PIN_MODE 						   0x00000001

// U0LCR Line Control Register
#define mDIVISOR_LATCH_ACCESS_BIT                  0x00000080
#define m8BIT_UART_WORD_LENGTH                     0x00000003

// UxIER Interrupt Enable Register
#define mRX_DATA_AVALAIBLE_INTERRUPT_ENABLE        0x00000001
#define mTHRE_INTERRUPT_ENABLE                     0x00000002

// UxIIR Pending Interrupt Identification Register
#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    0x00000002
#define mRX_DATA_AVALAIBLE_INTERRUPT_PENDING       0x00000004

/************ Interrupts **********/
// VIC (Vector Interrupt Controller) channels
#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

// VICVectCntlx Vector Control Registers
#define mIRQ_SLOT_ENABLE                           0x00000020

////////////// Zmienne globalne ///////////
char cOdebranyZnak;
struct ReceiverBuffer sReceiverBuffer;
struct TransmitterBuffer sTransmitterBuffer;

unsigned char ucCounter = 0;

////////////////////////////////////////////
void Receiver_PutCharacterToBuffer(char cCharacter) {
	
	if(sReceiverBuffer.ucCharCtr >= RECEIVER_SIZE) {
		sReceiverBuffer.eStatus = OVERFLOW;
	}
	else {
		if(cCharacter == TERMINATOR) {
			sReceiverBuffer.cData[sReceiverBuffer.ucCharCtr] = '\0';
			sReceiverBuffer.eStatus = READY;
			sReceiverBuffer.ucCharCtr = 0;
		}
		else {
			sReceiverBuffer.cData[sReceiverBuffer.ucCharCtr] = cCharacter;
			sReceiverBuffer.ucCharCtr += 1;
		}
	}
}

enum eReceiverStatus Receiver_GetStatus(void){
	return sReceiverBuffer.eStatus;
}	

void Receiver_GetStringCopy(char *cDestination){
	
	sReceiverBuffer.ucCharCtr = 0;
	
	while(sReceiverBuffer.cData[sReceiverBuffer.ucCharCtr] != '\0') {
		cDestination[sReceiverBuffer.ucCharCtr] = sReceiverBuffer.cData[sReceiverBuffer.ucCharCtr];
		sReceiverBuffer.ucCharCtr++;
	}
	
	cDestination[sReceiverBuffer.ucCharCtr] = '\0';
	
	sReceiverBuffer.eStatus = EMPTY;
	sReceiverBuffer.ucCharCtr = 0;
}	

////////////////////////////////////////////
char Transmitter_GetCharacterFromBuffer(void) {
	
	char cData;
	
	if(sTransmitterBuffer.eStatus == BUSY) {
		
		cData = sTransmitterBuffer.cData[sTransmitterBuffer.ucCharCtr];
		sTransmitterBuffer.ucCharCtr++;
		
		if(sTransmitterBuffer.fLastCharacter == 1) {
			cData = NULL;
			sTransmitterBuffer.fLastCharacter = 0;
			sTransmitterBuffer.ucCharCtr = 0;
			sTransmitterBuffer.eStatus = FREE;
		}
		else if(cData == NULL) {
			cData = TERMINATOR;
			sTransmitterBuffer.fLastCharacter = 1;
		}
	}
	return cData;
}

void Transmitter_SendString(char cString[]) {
	
	CopyString(cString, sTransmitterBuffer.cData);
	U0THR = sTransmitterBuffer.cData[0];
	sTransmitterBuffer.eStatus = BUSY;
	sTransmitterBuffer.fLastCharacter = 0;
	sTransmitterBuffer.ucCharCtr = 1;
}

enum eTransmitterStatus Transmitter_GetStatus(void) {
	return sTransmitterBuffer.eStatus;
}

///////////////////////////////////////////
__irq void UART0_Interrupt (void) {
   
   unsigned int uiCopyOfU0IIR = U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii
   char cCharacterFromBuffer;

   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALAIBLE_INTERRUPT_PENDING) { // odebrano znak
     cOdebranyZnak = U0RBR;
	 Receiver_PutCharacterToBuffer(cOdebranyZnak);
   }
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING) { // wyslano znak - nadajnik pusty
		if(Transmitter_GetStatus() == BUSY) {
			cCharacterFromBuffer = Transmitter_GetCharacterFromBuffer();
			U0THR = cCharacterFromBuffer;
		}
	}
	
   VICVectAddr = 0; // Acknowledge Interrupt
}

////////////////////////////////////////////
void UART_InitWithInt(unsigned int uiBaudRate) {

   // UART0
   PINSEL0 = PINSEL0 | mP0_1_RX0_PIN_MODE | mP0_0_TX0_PIN_MODE;                                     // ustawic pina na odbiornik uart0
   U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCESS_BIT; // dlugosc slowa, DLAB = 1
   U0DLL   = ((15000000/16)/uiBaudRate);                         // predkosc transmisji
   U0LCR  &= (~mDIVISOR_LATCH_ACCESS_BIT);                       // DLAB = 0
   U0IER  |= mRX_DATA_AVALAIBLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE;                // wlaczamy przerwania na linii Rx data
	
   // INT
   VICVectAddr1  = (unsigned long)UART0_Interrupt;           // set interrupt service routine address
   VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
   VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);           // Enable UART 0 Interrupt Channel
}
