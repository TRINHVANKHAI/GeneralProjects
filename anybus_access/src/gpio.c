/*
 * gpio.c
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
#include <poll.h>
#include <sys/ioctl.h>
#include "gpio.h"


void gpio_Init (int gpio)
{
	int fd;
	int sz;
	char gpno[9];

	sz = sprintf(gpno, "%d", gpio);
	fd = open("/sys/class/gpio/export", O_WRONLY);
	write (fd, gpno, sz);
	close(fd);

}

void gpio_DeInit (int gpio)
{
	int fd;
	int sz;
	char gpno[9];

	sz = sprintf(gpno, "%d", gpio);
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	write (fd, gpno, sz);
	close(fd);

}

void gpio_SetDirectionOut (int gpio)
{
	int fd;

	char buf[64];
	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(buf, O_WRONLY);
	write (fd, "out", 3);
	close (fd);
}

void gpio_SetDirectionIn (int gpio)
{
	int fd;

	char buf[64];
	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(buf, O_WRONLY);
	write (fd, "in", 2);
	close (fd);
}

void gpio_SetValue (int gpio, int stat)
{
	int fd;

	char buf[64];
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);

	if (stat == 1)
		write (fd, "1", 1);
	else if (stat == 0)
		write (fd, "0", 1);
	close(fd);

}

int gpio_GetValue (int gpio)
{
	int fd;

	char buf[64];
	char value;
	int stat;
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_RDONLY);
	read(fd, &value, 1);
	stat = (value == '1') ? 1 : 0;
	close(fd);
	return stat;
}

int gpio_EdgeDetectPinInit(struct pollfd *pfd, int gpio, int edge) {
	int fd;
	char buf[64];
	sprintf(buf, "/sys/class/gpio/gpio%d/edge", gpio);
	fd = open(buf, O_WRONLY);
	if(fd < 0) perror("GPIO edge");
	if(edge)
		write(fd, "rising", 6);
	else
		write(fd, "falling", 7);
	close(fd);

	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_RDONLY);
	if(fd < 0) perror("GPIO value");
	pfd->fd = fd;
	pfd->events = POLLPRI;
	pfd->revents = 0;
	return fd;
}

void gpio_EdgeDetectPinDeInit(struct pollfd *pfd) {
	close(pfd->fd);
}

int gpio_EdgeDetect (struct pollfd *pfd, int timeout) {
	int res;
	char value;
	pfd->revents = 0;
	res = poll (pfd, 1, timeout);
	if (res < 0) perror("Poll failed");
	if (pfd->revents & POLLPRI) {
		read (pfd->fd, &value, 1);
		pfd->revents = 0;
		return 1;
	}
	return 0;
}



