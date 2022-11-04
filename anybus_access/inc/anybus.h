/*
 * anybus.h
 *
 *  Created on: Mar 18, 2019
 *      Author: hinoeng.co.jp
 */

#ifndef ANYBUS_H_
#define ANYBUS_H_
#include "crc.h"

#ifndef ABCC_CFG_MSG_FRAG_LEN
#define ABCC_CFG_MSG_FRAG_LEN 16
#endif
#ifndef ABCC_CFG_MAX_PROCESS_DATA_SIZE
#define ABCC_CFG_MAX_PROCESS_DATA_SIZE 512
#endif
#define ABCC_CFG_DEFAULT_PD_LEN 0
#define ABCC_CFG_TXHEAD_PDOF 4
#define ABCC_CFG_RXHEAD_PDOF 5
#define ABCC_CFG_MSGHEAD_BSZ 12

#define NUM_BYTES_TO_WORDS(x)  (((x)+1) >>1)
#define NUM_WORDS_TO_BYTES(x)  ((x) <<1)
#define MAX_PAYLOAD_WORD_LEN (NUM_BYTES_TO_WORDS(ABCC_CFG_MSG_FRAG_LEN) + NUM_BYTES_TO_WORDS(ABCC_CFG_MAX_PROCESS_DATA_SIZE) + CRC_WORD_LEN_IN_WORDS)

/*Anybus's SPI Control registers*/
#define ABCC_SPICTRL_WRPD_VALID         (1 << 0)
#define ABCC_SPICTRL_ABRQ_CMDCNT_0      (0 << 1)
#define ABCC_SPICTRL_ABRQ_CMDCNT_1      (1 << 1)
#define ABCC_SPICTRL_ABRQ_CMDCNT_2      (2 << 1)
#define ABCC_SPICTRL_ABRQ_CMDCNT_3      (3 << 1)
#define ABCC_SPICTRL_FLAGS_MESSAGE      (1 << 3)
#define ABCC_SPICTRL_FLAGS_LASTFRAG     (1 << 4)
#define ABCC_SPICTRL_TOGGLE			    (1 << 7)

/*Anybus's SPI status register*/
#define ABCC_SPISTAT_WRMSG_FULL		    (1 << 0)
#define ABCC_SPISTAT_ABRS_CMDCNT_0      (0 << 1)
#define ABCC_SPISTAT_ABRS_CMDCNT_1      (1 << 1)
#define ABCC_SPISTAT_ABRS_CMDCNT_2      (2 << 1)
#define ABCC_SPISTAT_ABRS_CMDCNT_3      (3 << 1)
#define ABCC_SPISTAT_FLAGS_MESSAGE      (1 << 3)
#define ABCC_SPISTAT_FLAGS_LASTFRAG	    (1 << 4)
#define ABCC_SPISTAT_NEWPD				(1 << 5)


/*Anybus application status register*/
#define ABCC_APPSTAT_NORMAL				(1 << 0)
#define ABCC_APPSTAT_NOTSYNC			(1 << 1)
#define ABCC_APPSTAT_SYNC_CONF_ERR		(1 << 2)
#define ABCC_APPSTAT_RDPD_CONF_ERR		(1 << 3)
#define ABCC_APPSTAT_WRPD_CONF_ERR		(1 << 4)
#define ABCC_APPSTAT_SYNC_LOSS			(1 << 5)
#define ABCC_APPSTAT_EXCESSIVE_DAT_LOSS (1 << 6)
#define ABCC_APPSTAT_OUTPUT_ERR			(1 << 7)

/*Anybus buffer control register*/
#define ABCC_BUFCTRL_WRPD				(1 << 0)
#define ABCC_BUFCTRL_RDPD				(1 << 1)
#define ABCC_BUFCTRL_WRMSG				(1 << 2)
#define ABCC_BUFCTRL_RDMSG				(1 << 3)
#define ABCC_BUFCTRL_ANBR				(1 << 4)
#define ABCC_BUFCTRL_APPR				(1 << 5)
#define ABCC_BUFCTRL_APPRCLR			(1 << 6)

/*Anybus module status register*/
#define ABCC_MODSTAT_S_SETUP			(0 << 0)
#define ABCC_MODSTAT_S_NWINI			(1 << 0)
#define ABCC_MODSTAT_S_WAIT				(2 << 0)
#define ABCC_MODSTAT_S_ACTIVE			(3 << 0)
#define ABCC_MODSTAT_S_IDLE				(4 << 0)
#define ABCC_MODSTAT_S_ERROR			(5 << 0)
#define ABCC_MODSTAT_S_EXCEPTION		(7 << 0)
#define ABCC_MODSTAT_SUP				(1 << 3)

/*Anybus interrupt mask register*/
#define ABCC_INTMASK_RDPDIEN			(1 << 0)
#define ABCC_INTMASK_RDMSGIEN			(1 << 1)
#define ABCC_INTMASK_WRMSGIEN			(1 << 2)
#define ABCC_INTMASK_ANBRIEN			(1 << 3)
#define ABCC_INTMASK_STATUSIEN			(1 << 4)
#define ABCC_INTMASK_SYNCIEN			(1 << 6)

/*Anybus interrupt status register*/
#define ABCC_INTSTAT_RDPDI				(1 << 0)
#define ABCC_INTSTAT_RDMSGI				(1 << 1)
#define ABCC_INTSTAT_WRMSGI				(1 << 2)
#define ABCC_INTSTAT_ANBRI				(1 << 3)
#define ABCC_INTSTAT_STATUSI			(1 << 4)
#define ABCC_INTSTAT_PWRI				(1 << 5)
#define ABCC_INTSTAT_SYNC				(1 << 6)

/*Anybus command code*/
#define ABCC_CMDCODE_GET_ATTRIBUTE		0x01
#define ABCC_CMDCODE_SET_ATTRIBUTE		0x02
#define ABCC_CMDCODE_CREATE				0x03
#define ABCC_CMDCODE_DELETE				0x04
#define ABCC_CMDCODE_RESET				0x05
#define ABCC_CMDCODE_GET_ENUMSTRING		0x06
#define ABCC_CMDCODE_GET_IDXATTRIBUTE	0x07
#define ABCC_CMDCODE_SET_IDXATTRIBUTE	0x08

/*Anybus error response code*/
#define ABCC_ERRCODE_INVALID_MSGFORMAT		0x02
#define ABCC_ERRCODE_UNSUPPORTED_OBJECT		0x03
#define ABCC_ERRCODE_UNSUPPORTED_INSTANCE	0x04
#define ABCC_ERRCODE_UNSUPPORTED_COMMAND	0x05
#define ABCC_ERRCODE_INVALID_CMDEXT0		0x06
#define ABCC_ERRCODE_INVALID_CMDEXT1		0x07
#define ABCC_ERRCODE_ATTRIBUTE_NOTSETABLE	0x08
#define ABCC_ERRCODE_ATTRIBUTE_NOTGETABLE	0x09
#define ABCC_ERRCODE_TOO_MUCH_DATA			0x0A
#define ABCC_ERRCODE_NOT_ENOUGH_DATA		0x0B
#define ABCC_ERRCODE_OUT_OF_RANGE			0x0C
#define ABCC_ERRCODE_INVALID_STATE			0x0D
#define ABCC_ERRCODE_OUT_OF_RESOUCE			0x0E
#define ABCC_ERRCODE_SEG_FAILURE			0x0F
#define ABCC_ERRCODE_SEG_BUFF_OVERFLOW		0x10
#define ABCC_ERRCODE_VAL_TOO_HIGH			0x11
#define ABCC_ERRCODE_VAL_TOO_LOW			0x12
#define ABCC_ERRCODE_ATTRIBUTE_NAK			0x13
#define ABCC_ERRCODE_MSG_CHANNEL_SMALL		0x14
#define ABCC_ERRCODE_GENERAL_ERROR			0x15
#define ABCC_ERRCODE_PROTECTED_ACCESS		0x16
#define ABCC_ERRCODE_NO_DATA_AVAILABLE		0x17




typedef struct __attribute__((packed, aligned(2))) _ab_TX_Frame {
	uint16_t abSpiControl;             /*SPI control*/
	uint16_t abMessageLength;          /*Message len*/
	uint16_t abPDataLength;            /*Process data len*/
	uint16_t abIntMaskAppStatus;       /*Application status*/
	uint16_t abPayloadData[MAX_PAYLOAD_WORD_LEN];
	uint16_t abDummy;
} abTXFrame_t;

typedef struct __attribute__((packed, aligned(2))) _ab_RX_Frame {
	uint16_t abReserved;				/*Reserved*/
	uint16_t abLedStatus; 				/*LED state*/
	uint16_t abSpiStatus;    			/*Anybus module state*/
	uint16_t abNetTime[2]; 				/*Network time*/
	uint16_t abPayloadData[MAX_PAYLOAD_WORD_LEN];
} abRXFrame_t;

typedef struct __attribute__((packed, aligned(2))) _ab_SMessage_Data {
	uint16_t mDataSize;
	uint16_t mReserved;
	uint16_t mObjectSourceID;
	uint16_t mInstance;
	uint16_t mCommandCode;
	uint16_t mCommandExtend;
	uint16_t mPad;
} abMessageData_t;

typedef struct _ab_IO_Data {
	abTXFrame_t *ioTxFrame;
	abRXFrame_t *ioRxFrame;
	abMessageData_t *ioSMsgData;
	abMessageData_t *ioGMsgData;
	uint16_t ioMsgOf;
	uint16_t ioPrgOf;
	uint16_t ioCrcOf;
} abIOData_t;

void abMessageObjectSet(abIOData_t *pIOd, uint8_t mObject);
void abMessageSourceIDSet(abIOData_t *pIOd, uint8_t mSourceID);
void abMessageCommandSet(abIOData_t *pIOd, uint8_t mCmd);
void abMessageInstanceSet(abIOData_t *pIOd, uint16_t mInstance);
void abMessageDataSizeSet(abIOData_t *pIOd, uint16_t mSize);
void abGetAttributeSet(abIOData_t *pIOd, uint16_t mAtt);
void abProcessDataSizeSet(abIOData_t *pIOd, uint16_t pDatSz);
int abGetMessageGet(abIOData_t *pIOd, uint8_t *gMsg, uint8_t id);
void abTXCrcSet(abIOData_t *pIOd);
int abRXCrcCheck(abIOData_t *pIOd);
uint16_t abTransferSizeGet(abIOData_t *pIOd);
int abTransferInit(abIOData_t *pIOd);
int abSPITransferStart(abIOData_t *pIOd);
void abSpiTransferExit(void);

#endif /* ANYBUS_H_ */
