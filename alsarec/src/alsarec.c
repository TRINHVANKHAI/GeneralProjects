/*
 * alsarec.c
 *
 *  Created on: Feb 13, 2020
 *      Author: hinoeng
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include "allocator.h"
#include "alsarec.h"
#include "progressbar.h"

alsarec_state_t alsarec_state = ALSAREC_STATE_STOP;

static void alsarec_state_stopped(int sig) {
	alsarec_state = ALSAREC_STATE_STOP;
}

void alsarec_init_signal(void) {
	signal(SIGINT, alsarec_state_stopped);
	signal(SIGQUIT, alsarec_state_stopped);
	signal(SIGKILL, alsarec_state_stopped);
	signal(SIGTERM, alsarec_state_stopped);
}

struct timeval tv_begin, tv_end;
void alsarec_key_event(void *key) {
	static char pre=0, cur;
	unsigned long diff_ms, diff_us;
	for (;alsarec_state;) {
		gettimeofday(&tv_begin, NULL);
		cur = getchar();
		gettimeofday(&tv_end, NULL);
		diff_us = ((tv_end.tv_sec - tv_begin.tv_sec)*1000000 + tv_end.tv_usec-tv_begin.tv_usec);
		diff_ms = diff_us/1000;
		switch (cur) {
			case '\n':
				if ((pre != cur) || ((pre == cur) && (diff_ms>100)))
					alsarec_state = ALSAREC_STATE_NEG(alsarec_state);
				if (alsarec_state == ALSAREC_STATE_PAUSED) printf("PAUSED");
				else if (alsarec_state == ALSAREC_STATE_RECORDING) printf("RECORDING...");
				break;
			case 'T':
				alsarec_state = ALSAREC_STATE_STOP;
				break;
			default:
				break;

		}
		pre = cur;
	}
}
