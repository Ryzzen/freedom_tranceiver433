#include "cc1101.h"
#include "dwt_delay.h"

static HAL_StatusTypeDef CC1101_ReadReg(CC1101_HandleTypeDef* this, uint8_t address, uint8_t* buf, size_t size)
{
	if (!this || !buf) { return HAL_ERROR; }

	/* uint8_t header = size <= 1 ? CC1101_READ(address) : CC1101_READ_BURST(address); */
	uint8_t header    = CC1101_READ_BURST(address);
	size_t i          = 0; while ( i < size) *(buf + i++) = 0;

	SPI_SELECT;

	HAL_SPI_Transmit(this->hspi, &header, 1, CC1101_SPI_DELAY);
	HAL_StatusTypeDef status = HAL_SPI_Receive(this->hspi, buf, size, CC1101_SPI_DELAY);

	SPI_DESELECT;

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

	SPI_SELECT;

	status = HAL_SPI_Transmit(this->hspi, &header, 1, CC1101_SPI_DELAY);

	if (status == HAL_OK && data)
		status = HAL_SPI_Transmit(this->hspi, data, size, CC1101_SPI_DELAY);

	SPI_DESELECT;
	
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

static void CC1101_ConfUpdate(CC1101_HandleTypeDef* this, rfSettings* settings)
{
	if (!this) { return; }

	CC1101_WriteReg(this, CC1101_IOCFG2, &settings->iocfg2, 1);
	CC1101_WriteReg(this, CC1101_IOCFG1, &settings->iocfg1, 1);
	CC1101_WriteReg(this, CC1101_IOCFG0, &settings->iocfg0, 1);
	CC1101_WriteReg(this, CC1101_FIFOTHR, &settings->fifothr, 1);
	CC1101_WriteReg(this, CC1101_SYNC1, &settings->sync1, 1);
	CC1101_WriteReg(this, CC1101_SYNC0, &settings->sync0, 1);
	CC1101_WriteReg(this, CC1101_PKTLEN, &settings->pktlen, 1);
	CC1101_WriteReg(this, CC1101_PKTCTRL1, &settings->pktctrl1, 1);
	CC1101_WriteReg(this, CC1101_PKTCTRL0, &settings->pktctrl0, 1);
	CC1101_WriteReg(this, CC1101_ADDR, &settings->addr, 1);
	CC1101_WriteReg(this, CC1101_CHANNR, &settings->channr, 1);
	CC1101_WriteReg(this, CC1101_FSCTRL1, &settings->fsctrl1, 1);
	CC1101_WriteReg(this, CC1101_FSCTRL0, &settings->fsctrl0, 1);
	CC1101_WriteReg(this, CC1101_FREQ2, &settings->freq2, 1);
	CC1101_WriteReg(this, CC1101_FREQ1, &settings->freq1, 1);
	CC1101_WriteReg(this, CC1101_FREQ0, &settings->freq0, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG4, &settings->mdmcfg4, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG3, &settings->mdmcfg3, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG2, &settings->mdmcfg2, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG1, &settings->mdmcfg1, 1);
	CC1101_WriteReg(this, CC1101_MDMCFG0, &settings->mdmcfg0, 1);
	CC1101_WriteReg(this, CC1101_DEVIATN, &settings->deviatn, 1);
	CC1101_WriteReg(this, CC1101_MCSM2, &settings->mcsm2, 1);
	CC1101_WriteReg(this, CC1101_MCSM1, &settings->mcsm1, 1);
	CC1101_WriteReg(this, CC1101_MCSM0, &settings->mcsm0, 1);
	CC1101_WriteReg(this, CC1101_FOCCFG, &settings->foccfg, 1);
	CC1101_WriteReg(this, CC1101_BSCFG, &settings->bscfg, 1);
	CC1101_WriteReg(this, CC1101_AGCCTRL2, &settings->agcctrl2, 1);
	CC1101_WriteReg(this, CC1101_AGCCTRL1, &settings->agcctrl1, 1);
	CC1101_WriteReg(this, CC1101_AGCCTRL0, &settings->agcctrl0, 1);
	CC1101_WriteReg(this, CC1101_WOREVT1, &settings->worevt1, 1);
	CC1101_WriteReg(this, CC1101_WOREVT0, &settings->worevt0, 1);
	CC1101_WriteReg(this, CC1101_WORCTRL, &settings->worctrl, 1);
	CC1101_WriteReg(this, CC1101_FREND1, &settings->frend1, 1);
	CC1101_WriteReg(this, CC1101_FREND0, &settings->frend0, 1);
	CC1101_WriteReg(this, CC1101_FSCAL3, &settings->fscal3, 1);
	CC1101_WriteReg(this, CC1101_FSCAL2, &settings->fscal2, 1);
	CC1101_WriteReg(this, CC1101_FSCAL1, &settings->fscal1, 1);
	CC1101_WriteReg(this, CC1101_FSCAL0, &settings->fscal0, 1);
	CC1101_WriteReg(this, CC1101_RCCTRL1, &settings->rcctrl1, 1);
	CC1101_WriteReg(this, CC1101_RCCTRL0, &settings->rcctrl0, 1);
	CC1101_WriteReg(this, CC1101_FSTEST, &settings->fstest, 1);
	CC1101_WriteReg(this, CC1101_PTEST, &settings->ptest, 1);
	CC1101_WriteReg(this, CC1101_AGCTEST, &settings->agctest, 1);
	CC1101_WriteReg(this, CC1101_TEST2, &settings->test2, 1);
	CC1101_WriteReg(this, CC1101_TEST1, &settings->test1, 1);
	CC1101_WriteReg(this, CC1101_TEST0, &settings->test0, 1);
}

// Manual reset routine as define in the documentation
static void CC1101_Reset(CC1101_HandleTypeDef* this)
{
	if (!this) { return; }

	DWT_Init();

	SPI_SELECT;
	DWT_Delay(10);

	SPI_DESELECT;
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

void CC1101_Init(CC1101_HandleTypeDef* this, SPI_HandleTypeDef* hspi, rfSettings* settings)
{
	if (!this || !hspi) { return; }

	this->hspi       = hspi;
	this->SendPacket = CC1101_SendPacket;

	uint8_t version;

	CC1101_Reset(this);

	CC1101_ReadReg(this, CC1101_VERSION, &version, 1);

	CC1101_WriteReg(this, CC1101_SIDLE, NULL, 0);

	CC1101_WriteReg(this, CC1101_PATABLE, settings->pa_table, sizeof(settings->pa_table));
	CC1101_WriteReg(this, CC1101_SFRX, NULL, 0);
	CC1101_WriteReg(this, CC1101_SFTX, NULL, 0);
	CC1101_WriteReg(this, CC1101_SIDLE, NULL, 0);

	CC1101_ConfUpdate(this, settings);
}
