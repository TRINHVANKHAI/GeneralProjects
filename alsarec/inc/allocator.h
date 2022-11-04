/*
 * pcm_buf.h
 *
 *  Created on: Feb 13, 2020
 *      Author: hinoeng
 */

#ifndef PCM_BUF_H_
#define PCM_BUF_H_

#define HOLDER_ALLOCATED 1
#define HOLDER_FILLED 2
#define HOLDER_PILLED 3
#define HOLDER_PENDING 4

typedef struct buffer_list {
	struct buffer_list *prev;
	struct buffer_list *next;
	int idx;
	int fl;
	ssize_t buff_sz;
	char *buff;
} blist_t;

typedef enum _holder_thres {
	HOLDER_FIRST_QUATER = 1,
	HOLDER_HALF,
	HOLDER_THIRD_QUATER,
	HOLDER_FULL
} holder_thres_t;

/*
 * Allocate buffer pool, return number of buffer allocated
 */
int holder_allocate_buffer(ssize_t sz, int count);
void holder_deallocate_buffer(void);
/*
 * Looking for available one and fill buffer one at a time
 */
int holder_fill_buffer(char* buff, ssize_t buff_sz);
int holder_pill_buffer(char*buff, ssize_t buff_sz);
int holder_get_free_buffer(char **buff, ssize_t *sz);
int holder_get_filled_buffer(char **buff, ssize_t *bsz);

#endif /* PCM_BUF_H_ */
