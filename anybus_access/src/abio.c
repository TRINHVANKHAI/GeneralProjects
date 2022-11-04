/*
 * abio.c
 *
 *  Created on: Mar 20, 2019
 *      Author: hinoeng
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

static struct spi_ioc_transfer trsdat;

static int abioGPIOs[8] = {ABIO_DEVICE_RST, ABIO_DEVICE_IRQ, ABIO_DEVICE_MD0, ABIO_DEVICE_MD1, ABIO_DEVICE_MI0, ABIO_DEVICE_MI1,0,0};


static inline void pabort(const char *s)
{
	perror(s);
	abort();
}

static void abio_GPIOInit(int *gpios, int inum) {
	int i;
	for (i=0;i<inum;i++)
		if(gpios[i])
			gpio_Init (gpios[i]);

	gpio_SetDirectionOut(ABIO_DEVICE_RST);
	gpio_SetDirectionIn(ABIO_DEVICE_MI0);
	gpio_SetDirectionIn(ABIO_DEVICE_MI1);
	gpio_SetDirectionIn(ABIO_DEVICE_IRQ);
	gpio_SetDirectionIn(ABIO_DEVICE_MD0);
	gpio_SetDirectionIn(ABIO_DEVICE_MD1);

	gpio_SetValue(ABIO_DEVICE_RST, 1);

}

static void abio_GPIODeInit(int *gpios, int inum) {
	int i;
	for (i=0;i<inum;i++)
		if(gpios[i])
			gpio_DeInit (gpios[i]);
}

static void inline abio_deviceirqinit(abioDev_t *dat) {
	gpio_EdgeDetectPinInit(&dat->irqgpio, ABIO_DEVICE_IRQ, GPIOIRQ_EDGE_FALLING);
}

static void inline abio_deviceirqdeinit(abioDev_t *dat) {
	close(dat->irqgpio.fd);
}

void abio_doprepare (abioDev_t *abio)
{
	int ret;
	if (abio->drop == NULL)
		pabort ("No data");

	ret = ioctl(abio->bus, SPI_IOC_WR_MODE, &abio->mode);
	if(ret == -1)
		pabort("Unable to set SPI mode");
	ret = ioctl(abio->bus, SPI_IOC_RD_MODE, &abio->mode);
	if(ret == -1)
		pabort("Unable to get SPI mode");

	ret = ioctl(abio->bus, SPI_IOC_WR_BITS_PER_WORD, &abio->drop->bits_per_word);
	if(ret == -1)
		pabort("Set bits per word failed");
	ret = ioctl(abio->bus, SPI_IOC_RD_BITS_PER_WORD, &abio->drop->bits_per_word);
	if(ret == -1)
		pabort("Get bits per word failed");

	ret = ioctl(abio->bus, SPI_IOC_WR_MAX_SPEED_HZ, &abio->drop->speed_hz);
	if(ret == -1)
		pabort("Set max speed failed");
	ret = ioctl(abio->bus, SPI_IOC_RD_MAX_SPEED_HZ, &abio->drop->speed_hz);
	if(ret == -1)
		pabort("Get max speed failed");

}



static int abio_open (void)
{
	int dev;
	dev = open (ABIO_DEVICE, O_RDWR);
	if (dev < 0) pabort("Can not open device, unexpected exit");
	return dev;
}

#if defined(ABIO_SPI_GPIOCS_USER)
static void inline abio_assert (abioDev_t *dat)
{
	if (dat->cs)
		write (dat->cs, "0", 1);
}

static void inline abio_deactive (abioDev_t *dat)
{
	if (dat->cs)
		write (dat->cs, "1", 1);
}

int abio_deviceinit (abioDev_t *dat, struct spi_ioc_transfer *trs)
{
	int dp;
	int sz;
	char gpno[9];
	char buf[64];

	dat->drop = trs;
	dat->amount = 1;

	dat->bus = abio_open();

	dp = open("/sys/class/gpio/export", O_WRONLY);
	sz = sprintf(gpno, "%d", ABIO_DEVICE_SEL);
	write (dp, gpno, sz);
	close (dp);

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", ABIO_DEVICE_SEL);
	dp = open(buf, O_WRONLY);
	if (dp < 0) {
		perror ("GPIO direction");
		return -1;
	}
	write (dp, "out", 3);
	close (dp);

	sprintf(buf, "/sys/class/gpio/gpio%d/value", ABIO_DEVICE_SEL);
	dat->cs = open(buf, O_WRONLY);
	if (dat->cs < 0) {
		perror ("GPIO value");
		return -1;
	}
	abio_deactive (dat);
	return 0;
}

void abio_deviceexit (abioDev_t *dat)
{
	int dp;
	int sz;
	char gpno[9];

	abio_deactive (dat);
	close (dat->bus);
	close (dat->cs);
	dp = open("/sys/class/gpio/unexport", O_WRONLY);
	sz = sprintf(gpno, "%d", ABIO_DEVICE_SEL);
	write (dp, gpno, sz);
	close (dp);
}
#else

static void inline abio_assert (abioDev_t *dat)
{

}

static void inline abio_deactive (abioDev_t *dat)
{

}


int abio_deviceinit (abioDev_t *dat)
{
	abio_GPIOInit(abioGPIOs, 8);
	abio_deviceirqinit(dat);
	dat->bus = abio_open();
	dat->drop = &trsdat;
	dat->amount = 1;
	return 0;
}

void abio_deviceexit (abioDev_t *dat)
{
	abio_GPIODeInit(abioGPIOs, 8);
	abio_deviceirqdeinit(dat);
	close (dat->bus);

}

#endif

int abio_devicedetect(void) {
	int de;
	de = gpio_GetValue(ABIO_DEVICE_MD0) | gpio_GetValue(ABIO_DEVICE_MD1);
	return de;
}

int abio_deviceid(void) {
	int id;
	id = gpio_GetValue(ABIO_DEVICE_MI1) << 1;
	id |= gpio_GetValue(ABIO_DEVICE_MI0);
	return id;
}



int abio_deviceactivewait(abioDev_t *dat, int timeout) {
	while(!gpio_GetValue (ABIO_DEVICE_IRQ));
	while(gpio_GetValue (ABIO_DEVICE_IRQ));
	usleep(100000);
	return 0;
}

void abio_devicereset(void) {
	gpio_SetValue(ABIO_DEVICE_RST, 0);
	usleep(100000);
	gpio_SetValue(ABIO_DEVICE_RST, 1);
}

int abio_dotransfer (abioDev_t *abio)
{
	int ret;
	abio_assert (abio);
	ret = ioctl(abio->bus, SPI_IOC_MESSAGE(abio->amount), abio->drop);
	if (ret < 1)
		perror("Can not transfer via SPI");
	abio_deactive (abio);
	return ret;
}



