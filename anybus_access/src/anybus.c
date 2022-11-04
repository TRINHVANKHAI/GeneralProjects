/*
 * anybus.c
 *
 *  Created on: Mar 18, 2019
 *      Author: hinoeng.co.jp
 */
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "abio.h"
#include "anybus.h"

static abioDev_t abioDev;
static abTXFrame_t abTxFrame;
static abRXFrame_t abRxFrame;

static void abTransferOfSet (abIOData_t *pIOd) {
	abTXFrame_t *pTxfd = pIOd->ioTxFrame;
	pIOd->ioMsgOf = NUM_WORDS_TO_BYTES(ABCC_CFG_TXHEAD_PDOF);
	pIOd->ioPrgOf = NUM_WORDS_TO_BYTES(ABCC_CFG_TXHEAD_PDOF + pTxfd->abMessageLength);
	pIOd->ioCrcOf = NUM_WORDS_TO_BYTES(ABCC_CFG_TXHEAD_PDOF + pTxfd->abMessageLength + pTxfd->abPDataLength);
}

void abMessageObjectSet(abIOData_t *pIOd, uint8_t mObject) {
	abMessageData_t *pMsg = pIOd->ioSMsgData;
	pMsg->mObjectSourceID = (pMsg->mObjectSourceID & 0xff) | ((uint16_t)mObject << 8);
}

void abMessageSourceIDSet(abIOData_t *pIOd, uint8_t mSourceID) {
	abMessageData_t *pMsg = pIOd->ioSMsgData;
	pMsg->mObjectSourceID = (pMsg->mObjectSourceID & 0xff00) | (uint16_t)mSourceID;
}

int abMessageSourceIDCheck(abIOData_t *pIOd, uint8_t id) {
	uint16_t gnd = pIOd->ioGMsgData->mObjectSourceID & 0xff;
	return (gnd == id) ? 1: 0;
	return 1;
}

void abMessageCommandSet(abIOData_t *pIOd, uint8_t mCmd) {
	abMessageData_t *pMsg = pIOd->ioSMsgData;
	pMsg->mCommandCode = (uint16_t)(0x40 | mCmd);
}

void abMessageInstanceSet(abIOData_t *pIOd, uint16_t mInstance) {
	abMessageData_t *pMsg = pIOd->ioSMsgData;
	pMsg->mInstance = mInstance;
}

void abMessageDataSizeSet(abIOData_t *pIOd, uint16_t mSize) {
	abMessageData_t *pMsg = pIOd->ioSMsgData;
	abTXFrame_t *pTxfd = pIOd->ioTxFrame;
	pMsg->mDataSize = mSize;
	pTxfd->abMessageLength = NUM_BYTES_TO_WORDS(mSize + ABCC_CFG_MSGHEAD_BSZ);
	abTransferOfSet(pIOd);
}

void abGetAttributeSet(abIOData_t *pIOd, uint16_t mAtt) {
	abMessageData_t *pMsg = pIOd->ioSMsgData;
	pMsg->mCommandExtend = mAtt;
}

void abProcessDataSizeSet(abIOData_t *pIOd, uint16_t pDatSz){
	abTXFrame_t *pTxfd = pIOd->ioTxFrame;
	pTxfd->abPDataLength = NUM_BYTES_TO_WORDS(pDatSz);
	abTransferOfSet(pIOd);
}

uint16_t abTransferSizeGet(abIOData_t *pIOd) {
	uint16_t size = (pIOd->ioCrcOf + 4 + 2);
	return size;
}

int abGetMessageGet(abIOData_t *pIOd, uint8_t *gMsg, uint8_t id) {
	uint8_t* pmsg = (uint8_t*)pIOd->ioGMsgData;
	int msize = NUM_WORDS_TO_BYTES(pIOd->ioTxFrame->abMessageLength);
	if(msize == 0) {
		printf("No message\n");
		return -1;
	}

	memcpy(gMsg, pmsg, msize);
	return msize;
}

uint8_t abErrorCodeGet(abIOData_t *pIOd) {
	uint8_t err = (uint8_t)pIOd->ioGMsgData->mPad;
	return err;
}


void abTXCrcSet(abIOData_t *pIOd) {
	abTXFrame_t *pTxfd = pIOd->ioTxFrame;
	uint8_t* dCrc = (uint8_t*)pTxfd + pIOd->ioCrcOf;
	*(uint32_t*)dCrc = CRC_crc32((uint16_t*)pTxfd, pIOd->ioCrcOf);
}

int abRXCrcCheck(abIOData_t *pIOd) {
	abRXFrame_t *pRxfd = pIOd->ioRxFrame;
	uint16_t rxCrcOf = pIOd->ioCrcOf + 2;
	uint32_t hCrc = CRC_crc32((uint16_t*)pRxfd, rxCrcOf);
	uint8_t* crcBase = (uint8_t*)pRxfd + rxCrcOf;
	if (*(uint32_t*)crcBase == hCrc) {
		return 0;
	}
	else {
		printf("ERROR: CRC check failed: \nHost CRC:%.8x and Module CRC:%8.x\n", hCrc, *(uint32_t*)crcBase);
		return -1;
	}
}

void abTransferCtrlInit (abIOData_t *pIOd, uint16_t iCtrl) {
	abTXFrame_t *pTxfd = pIOd->ioTxFrame;
	pTxfd->abSpiControl = iCtrl;
}

static int abSpiTransferInit(abIOData_t *pIOd) {
	abioDev_t *pAbDev = &abioDev;
	int abBustype;
	abio_deviceinit (pAbDev);
	pAbDev->drop->tx_buf = (unsigned long)pIOd->ioTxFrame;
	pAbDev->drop->rx_buf = (unsigned long)pIOd->ioRxFrame;
	pAbDev->drop->len = abTransferSizeGet(pIOd);
	pAbDev->drop->speed_hz = 1000000;
	pAbDev->drop->bits_per_word = 8;
	pAbDev->drop->cs_change = 0;
	abio_doprepare (pAbDev);
	if(!abio_devicedetect()) {
		abio_devicereset();
		abio_deviceactivewait(pAbDev, 1000);
	} else {
		printf("Anybus module is not present\n");
	}

	abBustype = abio_deviceid();

	switch (abBustype) {
	case 0:
		printf("Active Anybus compactCom 30\n");
		break;
	case 1:
		printf("Passive Anybus compactCom \n");
		break;
	case 2:
		printf("Active Anybus compactCom 40\n");
		break;
	case 3:
		printf("User specific \n");
		break;
	default:
		printf("Bus type is not supported\n");
		break;
	}
	return 0;
}

void abSpiTransferExit(void) {
	abioDev_t *pAbDev = &abioDev;
	abio_deviceexit (pAbDev);
}

int abTransferInit(abIOData_t *pIOd) {
	memset(&abTxFrame, 0, sizeof(abTxFrame));
	memset(&abRxFrame, 0, sizeof(abRxFrame));
	pIOd->ioTxFrame = &abTxFrame;
	pIOd->ioRxFrame = &abRxFrame;
	pIOd->ioSMsgData = (abMessageData_t*)pIOd->ioTxFrame->abPayloadData;
	pIOd->ioGMsgData = (abMessageData_t*)pIOd->ioRxFrame->abPayloadData;
	pIOd->ioTxFrame->abSpiControl = ABCC_SPICTRL_FLAGS_MESSAGE | ABCC_SPISTAT_FLAGS_LASTFRAG;
	pIOd->ioTxFrame->abMessageLength = NUM_BYTES_TO_WORDS(ABCC_CFG_MSG_FRAG_LEN);
	pIOd->ioTxFrame->abPDataLength = ABCC_CFG_DEFAULT_PD_LEN;
	pIOd->ioTxFrame->abIntMaskAppStatus = 0;
	abSpiTransferInit(pIOd);

	return 0;
}

int abSPITransferStart(abIOData_t *pIOd) {
	abTXFrame_t *pTxfd = pIOd->ioTxFrame;
	abioDev_t *pAbDev = &abioDev;
	static uint16_t ab_SPIF_Retransmit = 0;
	pAbDev->drop->len = abTransferSizeGet(pIOd);
	if (!ab_SPIF_Retransmit)
		pTxfd->abSpiControl ^= ABCC_SPICTRL_TOGGLE;
	abTXCrcSet(pIOd);
	abio_dotransfer(pAbDev);
	ab_SPIF_Retransmit = abRXCrcCheck(pIOd);

	return ab_SPIF_Retransmit;
}



