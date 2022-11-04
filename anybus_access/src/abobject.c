/*
 * abobject.c
 *
 *  Created on: Mar 21, 2019
 *      Author: hinoeng
 */

#include <stdio.h>
#include "abobject.h"

static abIOData_t abIOData;

void abobInit(void) {
	abTransferInit(&abIOData);
	abMessageObjectSet(&abIOData, 0x01);
	abProcessDataSizeSet(&abIOData, 0);
	abMessageDataSizeSet(&abIOData, 20);
}

void abobDeInit(void) {
	abSpiTransferExit();
}


void abobGetObjAttribute(int inst, int att, int id) {
	int i, isz;
	uint8_t rmsg[64];
	uint8_t *infb = &rmsg[12];
	uint16_t *infw = (uint16_t*)&rmsg[12];
	uint32_t *infdw = (uint32_t*)&rmsg[12];
	abProcessDataSizeSet(&abIOData, 0);
	abMessageInstanceSet(&abIOData, inst);
	abGetAttributeSet(&abIOData, att);
	abMessageCommandSet(&abIOData, ABCC_CMDCODE_GET_ATTRIBUTE);
	abMessageDataSizeSet(&abIOData, 8);
	abMessageSourceIDSet(&abIOData, id);
	abSPITransferStart(&abIOData);
	abSPITransferStart(&abIOData);
	isz = abGetMessageGet(&abIOData, rmsg, id);

	if(inst) {
		switch(att) {
		case 1:
			printf("Module type: %.4x\n", (uint16_t)(*infw));
			break;
		case 2:
			printf("Firmware version: %d.%d.%d \n", (uint8_t)(*infb), (uint8_t)(*(infb+1)), (uint8_t)(*(infb+2)));
			break;
		case 3:
			printf("Module's serial No: %.8x\n", (uint32_t)(*infdw));
			break;

		default:
			printf("Not supported\n");
			break;
		}
	}
	else {
		switch(att) {
		case 1:
			printf("Module name: %s\n", infb);
			break;
		case 2:
			printf("Revision: %.2x\n", (uint8_t)(*infb));
			break;
		case 3:
			printf("Number of available instances: %d\n", (uint16_t)(*infw));
			break;
		case 4:
			printf("Highest instance number: %d\n", (uint16_t)(*infw));
			break;
		default:
			printf("Not supported\n");
			break;
		}
	}
}


void abnwGetObjAttribute(int inst, int att, int id) {
	int i, isz;
	uint8_t rmsg[64];
	uint8_t *infb = &rmsg[12];
	uint16_t *infw = (uint16_t*)&rmsg[12];
	uint32_t *infdw = (uint32_t*)&rmsg[12];
	abMessageObjectSet(&abIOData, 0x03);
	abProcessDataSizeSet(&abIOData, 0);
	abMessageInstanceSet(&abIOData, inst);
	abGetAttributeSet(&abIOData, att);
	abMessageCommandSet(&abIOData, ABCC_CMDCODE_GET_ATTRIBUTE);
	abMessageDataSizeSet(&abIOData, 8);
	abMessageSourceIDSet(&abIOData, id);
	abSPITransferStart(&abIOData);
	abSPITransferStart(&abIOData);
	isz = abGetMessageGet(&abIOData, rmsg, id);

	if(inst) {
		switch(att) {
		case 1:
			printf("Network type: %.4x\n", (uint16_t)(*infw));
			break;
		case 2:
			printf("Network descriptor string: %s \n", infb);
			break;

		default:
			printf("Not supported\n");
			break;
		}
	}
	else {
		switch(att) {
		case 1:
			printf("Object name: %s\n", infb);
			break;
		case 2:
			printf("Revision: %.2x\n", (uint8_t)(*infb));
			break;
		case 3:
			printf("Number of available instances: %d\n", (uint16_t)(*infw));
			break;
		case 4:
			printf("Highest instance number: %d\n", (uint16_t)(*infw));
			break;
		default:
			printf("Not supported\n");
			break;
		}
	}
}



