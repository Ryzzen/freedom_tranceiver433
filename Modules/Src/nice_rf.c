
#include "nice.h"
#include <stdlib.h>
#include "cc1101.h"


void* Nice_CC1101RfSettings()
{
	rfSettings* settings = malloc(sizeof(rfSettings));

	*settings = (rfSettings){
		0x29,  // IOCFG2        GDO2 Output Pin Configuration
		0x2E,  // IOCFG1        GDO1 Output Pin Configuration
		0x06,  // IOCFG0        GDO0 Output Pin Configuration
		0x07,  // FIFOTHR       RX FIFO and TX FIFO Thresholds
		0xD3,  // SYNC1         Sync Word, High Byte
		0x91,  // SYNC0         Sync Word, Low Byte
		NICE_PACKETSIZE,   // PKTLEN        Packet Length
		0x04,  // PKTCTRL1      Packet Automation Control
		0x00,  // PKTCTRL0      Packet Automation Control
		0x00,  // ADDR          Device Address
		0x00,  // CHANNR        Channel Number
		0x06,  // FSCTRL1       Frequency Synthesizer Control
		0x00,  // FSCTRL0       Frequency Synthesizer Control
		0x10,  // FREQ2         Frequency Control Word, High Byte
		0xB0,  // FREQ1         Frequency Control Word, Middle Byte
		0x71,  // FREQ0         Frequency Control Word, Low Byte
		0xF5,  // MDMCFG4       Modem Configuration
		0xE9,  // MDMCFG3       Modem Configuration
		0x30,  // MDMCFG2       Modem Configuration
		0x20,  // MDMCFG1       Modem Configuration
		0x00,  // MDMCFG0       Modem Configuration
		0x15,  // DEVIATN       Modem Deviation Setting
		0x07,  // MCSM2         Main Radio Control State Machine Configuration
		0x30,  // MCSM1         Main Radio Control State Machine Configuration
		0x18,  // MCSM0         Main Radio Control State Machine Configuration
		0x16,  // FOCCFG        Frequency Offset Compensation Configuration
		0x6C,  // BSCFG         Bit Synchronization Configuration
		0x03,  // AGCCTRL2      AGC Control
		0x40,  // AGCCTRL1      AGC Control
		0x91,  // AGCCTRL0      AGC Control
		0x87,  // WOREVT1       High Byte Event0 Timeout
		0x6B,  // WOREVT0       Low Byte Event0 Timeout
		0xF8,  // WORCTRL       Wake On Radio Control
		0x56,  // FREND1        Front End RX Configuration
		0x11,  // FREND0        Front End TX Configuration
		0xE9,  // FSCAL3        Frequency Synthesizer Calibration
		0x2A,  // FSCAL2        Frequency Synthesizer Calibration
		0x00,  // FSCAL1        Frequency Synthesizer Calibration
		0x1F,  // FSCAL0        Frequency Synthesizer Calibration
		0x41,  // RCCTRL1       RC Oscillator Configuration
		0x00,  // RCCTRL0       RC Oscillator Configuration
		0x59,  // FSTEST        Frequency Synthesizer Calibration Control
		0x7F,  // PTEST         Production Test
		0x3F,  // AGCTEST       AGC Test
		0x81,  // TEST2         Various Test Settings
		0x35,  // TEST1         Various Test Settings
		0x09,  // TEST0         Various Test Settings
		{0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00},
	};

	return (void*)(settings);
}

static void*(*factory[TRANCEIVER_MAX])() = { Nice_CC1101RfSettings };

void* Nice_GetRfSettings(niceModule* this)
{
	return factory[this->tranceiver]();	
}
