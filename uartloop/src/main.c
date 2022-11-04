/********************************************************/
/*  Header                                              */
/********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "common.h"


/********************************************************/
/*  Define                                              */
/********************************************************/
#define DBG_MSG_GRP "[APP-COM1-3]"
#define ComnSnd "/dev/ttymxc1"
#define ComnRcv "/dev/ttymxc2"
#define COM_SHDSZ_REG (DATA_BUF_SIZE -2)
#define COM_SLDSZ_REG (DATA_BUF_SIZE -1)

/********************************************************/
/*  Global                                              */
/********************************************************/
static int fdComSnd = 0;
static int fdComRcv = 0;
static struct termios stOldOptionSnd;
static struct termios stOldOptionRcv;

/********************************************************/
/*  Fuction                                             */
/********************************************************/

void CloseCom()
{
	if (fdComSnd > 0) {
		if (tcsetattr(fdComSnd, TCSAFLUSH, &stOldOptionSnd) ) {
			DBG_PRINT("%s ERR : Uart tcsetattr() failed. : errno = %d\n", DBG_MSG_GRP, errno);
		}

		if (close(fdComSnd) < 0 ) {
			DBG_PRINT( "%s ERR : COM close() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		}
	}

	if (fdComRcv > 0) {
		if (tcsetattr(fdComRcv, TCSAFLUSH, &stOldOptionRcv) ) {
			DBG_PRINT("%s ERR : Uart tcsetattr() failed. : errno = %d\n", DBG_MSG_GRP, errno);
		}

		if (close(fdComRcv) < 0 ) {
			DBG_PRINT( "%s ERR : COM close() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		}
	}
}

void sig_handler(int signo)
{
	int hit = 0;

	DBG_PRINT("%s INF : Received Signal. [number:%d]\n", DBG_MSG_GRP, signo);

	switch (signo) {
	case SIGINT  :
	case SIGTERM :
	case SIGKILL :
		hit = 1;
		break;
	}

	if (hit) {
		CloseCom();
		exit(0);
	}
}

void *commoncomsmg (void *args)
{
	int i;
	int sszNum=DATA_BUF_SIZE;
	fd_set fdsCom;
	unsigned char ucRcvBuf[DATA_BUF_SIZE];
	unsigned char ucInitBuf[DATA_BUF_SIZE];
	memcpy (ucInitBuf, args, DATA_BUF_SIZE);
	sszNum = ucInitBuf[COM_SHDSZ_REG];
	sszNum = (sszNum << 8) | ucInitBuf[COM_SLDSZ_REG];
	while (1) {

	// Check Received COM?
	FD_ZERO(&fdsCom);
	FD_SET(fdComSnd, &fdsCom);

	if ( select( fdComSnd + 1, &fdsCom, (fd_set *)0, (fd_set *)0, NULL ) <= 0 ) {
		DBG_PRINT( "%s ERR : select() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		continue;
	}
	memset (ucRcvBuf, 0, DATA_BUF_SIZE);
	sszNum = read(fdComSnd, ucRcvBuf, sszNum);
	printf ("COM1 Read: %d\n", sszNum);
	if (memcmp (ucInitBuf, ucRcvBuf, sszNum) != 0){
		printf ("COM1: Data lost\nStop transfer\n");
		break;
	}
	write (fdComSnd, ucRcvBuf, sszNum);
	}
	return NULL;
}
void *commoncomrmg (void *args)
{
	int i, sszNum;
	fd_set fdsComRcv;
	unsigned char ucRcvBuf[DATA_BUF_SIZE];
	unsigned char ucInitBuf[DATA_BUF_SIZE];
	memcpy (ucInitBuf, args, DATA_BUF_SIZE);
	sszNum = ucInitBuf[COM_SHDSZ_REG];
	sszNum = (sszNum << 8) | ucInitBuf[COM_SLDSZ_REG];
	while (1) {
	// Check Received COM?
	FD_ZERO(&fdsComRcv);
	FD_SET(fdComRcv, &fdsComRcv);

	if ( select( fdComRcv + 1, &fdsComRcv, (fd_set *)0, (fd_set *)0, NULL ) <= 0 ) {
		DBG_PRINT( "%s ERR : select() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		continue;
	}
	memset (ucRcvBuf, 0, DATA_BUF_SIZE);
	sszNum = read(fdComRcv, ucRcvBuf, sszNum);
	printf ("COM2 Read: %d\n", sszNum);
	if (memcmp (ucInitBuf, ucRcvBuf, sszNum) != 0)
		{
			printf ("COM2: Data lost\nStop transfer\n");
			break;
		}
	write (fdComRcv, ucRcvBuf, sszNum);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	int i, iNiceRet, datsize;
	struct termios stNewOptionSnd;
	struct termios stNewOptionRcv;
	unsigned char ucSndBuf[DATA_BUF_SIZE];
	for (i=0; i<DATA_BUF_SIZE; i++)
		ucSndBuf[i] = i%253;
	fd_set fdsCom;
	ssize_t sszSendCntBase = DATA_BUF_SIZE;
	datsize = sszSendCntBase;
	ssize_t sszNum = 0;
	pthread_t pComSnd;
	pthread_t pComRcv;

	memset (&stOldOptionSnd, 0, sizeof(stOldOptionSnd));
	memset (&stOldOptionRcv, 0, sizeof(stOldOptionRcv));
	DBG_PRINT("%s START UART Communication Test for COM1-3.\n\n", DBG_MSG_GRP);
	DBG_PRINT("%s INF : COM device is <%s>\n", DBG_MSG_GRP, argv[1]);

	// NICE Setting
	errno = 0;
	iNiceRet = nice( PRIO_MAIN_VAL );
	DBG_PRINT( "%s INF : iNiceRet = %d\n", DBG_MSG_GRP, iNiceRet );
	if ( (iNiceRet == -1) && (errno != 0) ) {
		DBG_PRINT( "%s ERR : nice() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		return -1;
	}

	// Signal Handler Setting
	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		DBG_PRINT("%s ERR : signal register failed. (SIGINT)\n", DBG_MSG_GRP);
		return -2;
	}
	if (signal(SIGTERM, sig_handler) == SIG_ERR) {
		DBG_PRINT("%s ERR : signal register failed. (SIGTERM)\n", DBG_MSG_GRP);
		return -2;
	}
#if 0
	if (signal(SIGKILL, sig_handler) == SIG_ERR) {
		DBG_PRINT("%s ERR : signal register failed. (SIGKILL)\n", DBG_MSG_GRP);
		return -2;
	}
#endif

	// Open COM Port
	fdComSnd = open( ComnSnd, (O_RDWR | O_NOCTTY));
	if ( fdComSnd < 0 ) {
		DBG_PRINT( "%s ERR : COM open() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		return -3;
	}
	fdComRcv = open( ComnRcv, (O_RDWR | O_NOCTTY));
	if ( fdComRcv < 0 ) {
		DBG_PRINT( "%s ERR : COM open() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		return -3;
	}
	// Read Old COM Setting
	if ( tcgetattr( fdComSnd, &stOldOptionSnd ) ) {
		DBG_PRINT( "%s ERR : tcgetattr() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		CloseCom();
		return -4;
	}
	if ( tcgetattr( fdComRcv, &stOldOptionRcv ) ) {
		DBG_PRINT( "%s ERR : tcgetattr() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		CloseCom();
		return -4;
	}
	// Set New COM Setting
	memset( &stNewOptionSnd, 0, sizeof( stNewOptionSnd ) );
	stNewOptionSnd.c_iflag = 0;
	stNewOptionSnd.c_oflag = 0;
	stNewOptionSnd.c_cflag = (B230400 | CLOCAL | CREAD | CS8 | HUPCL);
	stNewOptionSnd.c_lflag = (ISIG | ECHOCTL);
	memset( &stNewOptionRcv, 0, sizeof( stNewOptionSnd ) );
	stNewOptionRcv.c_iflag = 0;
	stNewOptionRcv.c_oflag = 0;
	stNewOptionRcv.c_cflag = (B230400 | CLOCAL | CREAD | CS8 | HUPCL);
	stNewOptionRcv.c_lflag = (ISIG | ECHOCTL);

	// Set Raw mode(Set Binary transfer)
	cfmakeraw(&stNewOptionSnd);
	stNewOptionSnd.c_cc[0] = 254;
	cfmakeraw(&stNewOptionRcv);
	stNewOptionRcv.c_cc[0] = 254;

	if ( tcsetattr( fdComSnd, TCSANOW, &stNewOptionSnd ) ) {
		DBG_PRINT( "%s ERR : tcsetattr() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		CloseCom();
		return -5;
	}
	if ( tcsetattr( fdComRcv, TCSANOW, &stNewOptionRcv ) ) {
		DBG_PRINT( "%s ERR : tcsetattr() failed. : errno = %d\n", DBG_MSG_GRP, errno );
		CloseCom();
		return -5;
	}

	ucSndBuf[COM_SHDSZ_REG] = datsize >> 8;
	ucSndBuf[COM_SLDSZ_REG] = datsize & 0xFF;
	write (fdComSnd, ucSndBuf, datsize);
	write (fdComRcv, ucSndBuf, datsize);
	pthread_create (&pComSnd, NULL, commoncomsmg, ucSndBuf);
	pthread_create (&pComRcv, NULL, commoncomrmg, ucSndBuf);

	while (datsize < 0) {

		// Check Received COM?
		FD_ZERO(&fdsCom);
		FD_SET(fdComSnd, &fdsCom);

		if ( select( fdComSnd + 1, &fdsCom, (fd_set *)0, (fd_set *)0, NULL ) <= 0 ) {
			DBG_PRINT( "%s ERR : select() failed. : errno = %d\n", DBG_MSG_GRP, errno );
			continue;
		}

		DBG_PRINT("Error: Data lost");
		// Read COM
		memset(ucSndBuf, 0, sizeof(ucSndBuf));
		sszNum = read(fdComSnd, ucSndBuf, DATA_BUF_SIZE);
		DBG_PRINT("%s INF : COM read() = %d\n", DBG_MSG_GRP, (int)sszNum);

		if (sszNum < 0) {
			DBG_PRINT("%s ERR : COM read() failed. : errno = %d\n", DBG_MSG_GRP, errno);
			continue;
		}

		// Write COM
		sszSendCntBase = sszNum;
		do {
			sszNum = write(fdComSnd, &ucSndBuf[(sszNum - sszSendCntBase)], sszSendCntBase);
			DBG_PRINT("%s INF : COM write() = %d\n", DBG_MSG_GRP, (int)sszNum);

			if (sszNum < 0) {
				DBG_PRINT("%s ERR : COM write() failed. : errno = %d\n", DBG_MSG_GRP, errno);
				break;
			}
			else {
				sszSendCntBase -= sszNum;
			}
		} while (sszSendCntBase > 0);


	}

		pthread_join(pComSnd, NULL);
		pthread_join(pComRcv, NULL);

	CloseCom();

	return 0;
}
