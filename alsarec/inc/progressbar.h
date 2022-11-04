/*
 * progressbar.h
 *
 *  Created on: Feb 18, 2020
 *      Author: hinoeng
 */

#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_


#define PROGRESSBAR_FILL_RES 10
void progressbar_fillup (const char *msg, int pos, int step);
void progressbar_draw(const char *msg, int pos, int step);


#endif /* PROGRESSBAR_H_ */
