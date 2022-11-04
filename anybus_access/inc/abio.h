/*
 * abio.h
 *
 *  Created on: Mar 20, 2019
 *      Author: hinoeng
 */

#ifndef ABIO_H_
#define ABIO_H_
#include <poll.h>
#include "gpio.h"

#define ABIO_DEVICE "/dev/spidev1.0"
#define ABIO_DEVICE_SEL GPIO_TO_NUM(5, 12)
#define ABIO_DEVICE_RST GPIO_TO_NUM(1, 4)
#define ABIO_DEVICE_IRQ GPIO_TO_NUM(1, 6)
#define ABIO_DEVICE_MD0 GPIO_TO_NUM(4, 19)
#define ABIO_DEVICE_MD1 GPIO_TO_NUM(4, 18)
#define ABIO_DEVICE_MI0 GPIO_TO_NUM(4, 17)
#define ABIO_DEVICE_MI1 GPIO_TO_NUM(4, 20)



typedef struct _abio_devdata {
	struct spi_ioc_transfer *drop;
	int amount;
	int bus;
	int cs;
	uint8_t mode;
	struct pollfd irqgpio;
} abioDev_t;

void abio_doprepare (abioDev_t *abio);
int abio_dotransfer (abioDev_t *abio);
int abio_deviceid(void);
int abio_devicedetect(void);
void abio_devicereset(void);
int abio_deviceactivewait(abioDev_t *dat, int timeout);
int abio_deviceinit (abioDev_t *dat);
void abio_deviceexit (abioDev_t *dat);

#endif /* ABIO_H_ */
