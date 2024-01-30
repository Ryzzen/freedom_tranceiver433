#include "cc1101.h"
#include "dwt_delay.h"

static HAL_StatusTypeDef CC1101_ReadReg(CC1101_HandleTypeDef* this, uint8_t address, uint8_t* buf, size_t size)
{
	if (!this || !buf) { return HAL_ERROR; }

	/* uint8_t header = size <= 1 ? CC1101_READ(address) : CC1101_READ_BURST(address); */
	uint8_t header = CC1101_READ_BURST(address);
	size_t i = 0; while ( i < size) *(buf + i++) = 0;

	SELECT;

	HAL_SPI_Transmit(this->hspi, &header, 1, CC1101_SPI_DELAY);
	HAL_StatusTypeDef status = HAL_SPI_Receive(this->hspi, buf, size, CC1101_SPI_DELAY);

	DESELECT;

	DWT_Init();
	DWT_Delay(50);

	return status;
}

static HAL_StatusTypeDef CC1101_WriteReg(CC1101_HandleTypeDef* this, uint8_t address, uint8_t* data, size_t size)
{
	if (!this) { return HAL_ERROR; }

	HAL_StatusTypeDef status;
	/* uint8_t header = (data && size) ? (size <= 1) ? CC1101_WRITE(address) : CC1101_WRITE_BURST(address) : address; */
	uint8_t header = (data && size) ? CC1101_WRITE_BURST(address) : address;

	SELECT;

	status = HAL_SPI_Transmit(this->hspi, &header, 1, CC1101_SPI_DELAY);

	if (status == HAL_OK && data)
		status = HAL_SPI_Transmit(this->hspi, data, size, CC1101_SPI_DELAY);

	DESELECT;
	
	DWT_Init();
	DWT_Delay(50);

	return status;
}

static HAL_StatusTypeDef CC1101_WriteData(CC1101_HandleTypeDef* this, uint8_t* data, uint8_t size)
{
	if (!this) { return HAL_ERROR; }

	HAL_StatusTypeDef status;

	/* status = WriteReg(this, CC1101_TXFIFO, &size, 1); */

	/* if (status == HAL_OK) */
		status = CC1101_WriteReg(this, CC1101_TXFIFO, data, size);

	return status;
}

static void CC1101_ConfUpdate(CC1101_HandleTypeDef* this)
{
	if (!this) { return; }

	CC1101_WriteReg(this, CC1101_IOCFG2, &this->settings.iocfg2, 1);
	CC1101_WriteReg(this, CC1101_IOCFG1, &this->settings.iocfg1, 1);
	CC1101_WriteReg(this, CC1101_IOCFG0, &this->settings.iocfg0, 1);
	CC1101_WriteReg(this, CC1101_FIFOTHR, &this->settings.fifothr, 1);
	CC1101_WriteReg(this, CC1101_SYNC1, &this->settings.sync1, 1);
	CC1101_WriteReg(this, CC1101_SYNC0, &this->settings.sync0, 1);
	CC1101_WriteReg(this, CC1101_PKTLEN, &this->settings.pktlen, 1);
	CC1101_WriteReg(this, CC1101_PKTCTRL1, &this->settings.pktctrl1, 1);
	CC1101_WriteReg(this, CC1101_PKTCTRL0, &this->settings.pktctrl0, 1);
	CC1101_WriteReg(this, CC1101_ADDR, &this->settings.addr, 1);
	CC1101_WriteReg(this, CC1101_CHANNR, &this->settings.channr, 1);
	CC1101_WriteReg(this, CC1101_FSCTRL1, &this->settings.fsctrl1, 1);
	CC1101_WriteReg(this, CC1101_FSCTRL0, &this->settings.fsctrl0, 1);
	CC1101_WriteReg(this, CC1101_FREQ2, &this->settings.freq2, 1);
	CC1101_WriteReg(this, CC1101_FREQ1, &this->settings.freq1, 1);
	CC1101_WriteReg(this, CC1101_FREQ0, &this->settings.freq0, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG4, &this->settings.mdmcfg4, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG3, &this->settings.mdmcfg3, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG2, &this->settings.mdmcfg2, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG1, &this->settings.mdmcfg1, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG0, &this->settings.mdmcfg0, 1);
	CC1101_WriteReg(this, CC1101_DEVIATN, &this->settings.deviatn, 1);
	CC1101_WriteReg(this, CC1101_MCSM2, &this->settings.mcsm2, 1);
	CC1101_WriteReg(this, CC1101_MCSM1, &this->settings.mcsm1, 1);
	CC1101_WriteReg(this, CC1101_MCSM0, &this->settings.mcsm0, 1);
	CC1101_WriteReg(this, CC1101_FOCCFG, &this->settings.foccfg, 1);
	CC1101_WriteReg(this, CC1101_BSCFG, &this->settings.bscfg, 1);
	CC1101_WriteReg(this, CC1101_AGCCTRL2, &this->settings.agcctrl2, 1);
	CC1101_WriteReg(this, CC1101_AGCCTRL1, &this->settings.agcctrl1, 1);
	CC1101_WriteReg(this, CC1101_AGCCTRL0, &this->settings.agcctrl0, 1);
	CC1101_WriteReg(this, CC1101_WOREVT1, &this->settings.worevt1, 1);
	CC1101_WriteReg(this, CC1101_WOREVT0, &this->settings.worevt0, 1);
	CC1101_WriteReg(this, CC1101_WORCTRL, &this->settings.worctrl, 1);
	CC1101_WriteReg(this, CC1101_FREND1, &this->settings.frend1, 1);
	CC1101_WriteReg(this, CC1101_FREND0, &this->settings.frend0, 1);
	CC1101_WriteReg(this, CC1101_FSCAL3, &this->settings.fscal3, 1);
	CC1101_WriteReg(this, CC1101_FSCAL2, &this->settings.fscal2, 1);
	CC1101_WriteReg(this, CC1101_FSCAL1, &this->settings.fscal1, 1);
	CC1101_WriteReg(this, CC1101_FSCAL0, &this->settings.fscal0, 1);
	CC1101_WriteReg(this, CC1101_RCCTRL1, &this->settings.rcctrl1, 1);
	CC1101_WriteReg(this, CC1101_RCCTRL0, &this->settings.rcctrl0, 1);
	CC1101_WriteReg(this, CC1101_FSTEST, &this->settings.fstest, 1);
	CC1101_WriteReg(this, CC1101_PTEST, &this->settings.ptest, 1);
	CC1101_WriteReg(this, CC1101_AGCTEST, &this->settings.agctest, 1);
	CC1101_WriteReg(this, CC1101_TEST2, &this->settings.test2, 1);
	CC1101_WriteReg(this, CC1101_TEST1, &this->settings.test1, 1);
	CC1101_WriteReg(this, CC1101_TEST0, &this->settings.test0, 1);
}

static void CC1101_Reset(CC1101_HandleTypeDef* this)
{
	if (!this) { return; }

	DWT_Init();

	SELECT;
	DWT_Delay(10);

	DESELECT;
	DWT_Delay(40 - 10);

	CC1101_WriteReg(this, CC1101_SRES, NULL, 0);
}

static void CC1101_SendPacket(CC1101_HandleTypeDef* this, uint8_t* buf, uint8_t size)
{
	if (!this || !buf) { return; }

	uint8_t state = 0;

	CC1101_WriteData(this, buf, size);

	CC1101_WriteReg(this, CC1101_STX, NULL, 0);

	CC1101_ReadReg(this, CC1101_MARCSTATE, &state, 1);
	while ((state & 0x1F) != 0x01)
		CC1101_ReadReg(this, CC1101_MARCSTATE, &state, 1);

	CC1101_WriteReg(this, CC1101_SFTX, NULL, 0);
}

void CC1101_Init(CC1101_HandleTypeDef* this, SPI_HandleTypeDef* hspi, rfSettings settings)
{
	if (!this || !hspi) { return; }

	this->hspi = hspi;
	this->settings = settings;
	this->SendPacket = CC1101_SendPacket;

	uint8_t version;

	CC1101_Reset(this);

	CC1101_ReadReg(this, CC1101_VERSION, &version, 1);

	CC1101_WriteReg(this, CC1101_SIDLE, NULL, 0);

	CC1101_WriteReg(this, CC1101_PATABLE, this->settings.pa_table, sizeof(this->settings.pa_table));
	CC1101_WriteReg(this, CC1101_SFRX, NULL, 0);
	CC1101_WriteReg(this, CC1101_SFTX, NULL, 0);
	CC1101_WriteReg(this, CC1101_SIDLE, NULL, 0);

	CC1101_ConfUpdate(this);
}
