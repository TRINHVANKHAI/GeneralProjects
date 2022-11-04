/** @mainpage anybus_access - HIEC
 *
 * @author TRINH VAN KHAI <mrkhai@live.com>
 * @version 1.0.0
**/

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "abobject.h"
#define VERSIONNO "1.0.0"

/**
 * Main class of project anybus_access
 *
 * @param argc the number of arguments
 * @param argv the arguments from the commandline
 * @returns exit code of the application
 */


int main(int argc, char **argv) {
	int pid;

	abobInit();
	abobGetObjAttribute(0, ABOBJ_INS0_GM_DES, pid);
	abobGetObjAttribute(0, ABOBJ_INS0_GM_REV, pid);
	abobGetObjAttribute(1, ABOBJ_INS0_GM_DES, pid);
	abobGetObjAttribute(1, ABOBJ_INS1_GM_FVR, pid);
	abobGetObjAttribute(1, ABOBJ_INS1_GM_SRN, pid);
	abnwGetObjAttribute(1, ABOBJ_INS0_GM_DES, pid);
	abobDeInit();
	return 0;
}
