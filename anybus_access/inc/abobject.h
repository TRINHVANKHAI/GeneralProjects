/*
 * abobject.h
 *
 *  Created on: Mar 21, 2019
 *      Author: hinoeng
 */

#ifndef ABOBJECT_H_
#define ABOBJECT_H_

#include "anybus.h"

#define ABOBJ_INS0_GM_DES 1
#define ABOBJ_INS0_GM_REV 2
#define ABOBJ_INS0_GM_NOI 3
#define ABOBJ_INS0_GM_MIN 4
#define ABOBJ_INS1_GM_TYP 1
#define ABOBJ_INS1_GM_FVR 2
#define ABOBJ_INS1_GM_SRN 3
typedef struct abob_ObjAtt {
	uint16_t abobReserved;
	uint8_t *att;
}abobObj_t;


void abobInit(void);
void abobDeInit(void);
void abobGetObjAttribute(int inst, int att, int id);
void abnwGetObjAttribute(int inst, int att, int id);

#endif /* ABOBJECT_H_ */
