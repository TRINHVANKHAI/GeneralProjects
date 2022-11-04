/*
 * progressbar.c
 *
 *  Created on: Feb 18, 2020
 *      Author: hinoeng
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "progressbar.h"

static char status_bar[200];
static char status_sp[4] = "-\\|/";
void progressbar_draw(const char *msg, int pos, int step)
{
	int i = pos%step;
	int offs = strlen(msg)+1;
	char *connecting_stat = status_bar;
	strcpy (connecting_stat+1, msg);
	memset (connecting_stat+offs, '-', step);
	*(connecting_stat+offs+i) = '=';
	*connecting_stat = 13;
	*(connecting_stat+offs+step) = 0;
	fputs(connecting_stat, stdout);
}

void progressbar_fillup (const char *msg, int pos, int step)
{
	int i = (pos*PROGRESSBAR_FILL_RES) / step;
	int offs = strlen(msg)+1;
	char *connecting_stat = status_bar;
	strcpy (connecting_stat+1, msg);
	memset (connecting_stat+offs, ' ', step);
	memset (connecting_stat+offs, '*', i);
	*connecting_stat = 13;
	*(connecting_stat+offs+step) = 0;
	fputs(connecting_stat, stdout);
}

void progressbar_spin(const char *msg, int pos, int speed) {
	int i1 = pos>>speed;
	int i = i1%4;
	char c = status_sp[i];
	char *connecting_stat = status_bar;
	int offs = strlen(msg)+1;
	strcpy (connecting_stat+1, msg);
	*(connecting_stat + offs) = c;
	*connecting_stat = 13;
	*(connecting_stat+offs+1) = 0;
	fputs(connecting_stat, stdout);
}
