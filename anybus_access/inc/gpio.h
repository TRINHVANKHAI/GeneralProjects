/*
 * gpio.h
 *
 *  Created on: Mar 20, 2019
 *      Author: hinoeng
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_TO_NUM(a, b) ((a-1)*32 + b)
#define GPIOIRQ_EDGE_FALLING  0
#define GPIOIRQ_EDGE_RISING	  1
#define GPIOIRQ_EDGE_BOTH     2
void gpio_Init (int gpio);
void gpio_DeInit (int gpio);
void gpio_SetDirectionOut (int gpio);
void gpio_SetDirectionIn (int gpio);
void gpio_SetValue (int gpio, int stat);
int gpio_GetValue (int gpio);

int gpio_EdgeDetectPinInit(struct pollfd *pfd, int gpio, int edge);
void gpio_EdgeDetectPinDeInit(struct pollfd *pfd);
int gpio_EdgeDetect (struct pollfd *pfd, int timeout);
#endif /* GPIO_H_ */
