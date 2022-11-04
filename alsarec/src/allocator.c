/*
 * pcm_buf.c
 *
 *  Created on: Feb 13, 2020
 *      Author: hinoeng
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "allocator.h"
/*
 * Init holder buffer
 */
void holder_init(blist_t ** head, ssize_t buff_sz) {
	blist_t *newhead = malloc(sizeof(blist_t));
	newhead->next = newhead;
	newhead->prev = newhead;
	newhead->idx = 0;
	newhead->buff_sz = buff_sz;
	newhead->buff = malloc (newhead->buff_sz);
	if (newhead->buff) {
		newhead->fl = HOLDER_ALLOCATED;
	} else {
		fprintf (stderr, "Cannot allocate memory at %x: %d \n", newhead->buff, newhead->buff_sz);
		abort();
	}
	*head = newhead;
}

void holder_deinit(blist_t *head) {
	free(head->buff);
	free(head);
}
/*
 * Add new buffer to chain
 */
int holder_add_element(blist_t* pre, ssize_t buff_sz) {
	if (pre == NULL) {
		fprintf(stderr, "Prev node is empty\n");
		return -1;
	}
	blist_t *new_buff = malloc(sizeof(blist_t));
	if(!new_buff) {
		fprintf(stderr, "Cannot add new element \n");
		return -1;
	}

	new_buff->prev = pre;
	new_buff->next = pre->next;
	pre->next = new_buff;
	new_buff->idx = pre->idx+1;
	new_buff->buff_sz = buff_sz;
	new_buff->buff = malloc (new_buff->buff_sz);
	if (new_buff->buff) {
		new_buff->fl = HOLDER_ALLOCATED;
	} else {
		fprintf (stderr, "Cannot allocate memory at %x: %d \n", new_buff->buff, new_buff->buff_sz);
		return -1;
	}
	return 0;
}

/*
 * Add new buffer to chain
 */
int holder_add_element_tail(blist_t* head, ssize_t buff_sz) {
	blist_t *lst = head->prev;
	if (head == NULL) {
		fprintf(stderr, "Head node is empty\n");
		return -1;
	}
	blist_t *new_buff = malloc(sizeof(blist_t));
	if(!new_buff) {
		fprintf(stderr, "Cannot add new element \n");
		return -1;
	}

	new_buff->prev = lst;
	new_buff->next = head;
	head->prev = new_buff;
	lst->next = new_buff;
	new_buff->idx = lst->idx+1;
	new_buff->buff_sz = buff_sz;
	new_buff->buff = malloc (new_buff->buff_sz);
	if (new_buff->buff) {
		new_buff->fl = HOLDER_ALLOCATED;
	} else {
		fprintf (stderr, "Cannot allocate memory at %x: %d \n", new_buff->buff, new_buff->buff_sz);
		return -1;
	}
	return 0;
}

/*
 * Del buffer from chain
 */
void holder_del_element(blist_t* pre) {
	if (pre == NULL) {
		fprintf(stderr, "Prev node is empty\n");
		return;
	}
	blist_t *next = pre->next->next;
	free(pre->next->buff);
	free(pre->next);
	next->prev = pre;
	pre->next = next;
}


/*
 * Del last node buffer from chain
 */
void holder_del_element_tail(blist_t* head) {
	if (head == NULL) {
		fprintf(stderr, "Head node is empty\n");
		return;
	}
	blist_t *prev = head->prev->prev;
	free(head->prev->buff);
	free(head->prev);
	prev->next = head;
	head->prev = prev;
}

/*
 * Allocate buffer pool, return number of buffer allocated
 */
static blist_t *holder_head;
static blist_t *holder_curr;
static blist_t *holder_curw;

int holder_allocate_buffer(ssize_t sz, int count) {
	blist_t *head;
	int i=0;
	if((sz <=0) || (count<=0)) {
		fprintf(stderr, "Please assign buffer size and count\n");
		return -1;
	}
	/*Allocate first buffer*/
	holder_init(&head, sz);
	for(i=0; i<count-1; i++) {
		if(holder_add_element_tail(head, sz)<0){
			fprintf(stderr, "Cannot allocate new buffer\n");
			break;
		}
	}
	holder_head = head;
	holder_curr = head;
	holder_curw = head;
	return i+1;
}

void holder_deallocate_buffer(void) {
	blist_t *pd = NULL;
	for(pd=holder_head; pd->next != holder_head;) {
    	holder_del_element_tail(holder_head);
    }
	holder_deinit(holder_head);
	holder_curr = NULL;
	holder_head = NULL;
	holder_curw = NULL;
}


/*
 * Looking for available one and fill buffer one at a time
 */
int holder_fill_buffer(char* buff, ssize_t buff_sz){

	if(!buff) {
		fprintf(stderr, "Source buffer is NULL\n");
		return -1;
	}

	/*Only accept buffer with the same size*/
	if(holder_curw->buff_sz == buff_sz) {
		memcpy(holder_curw->buff, buff, holder_curw->buff_sz);
		holder_curw->fl = HOLDER_FILLED;
		holder_curw = holder_curw->next;
		return 0;
	} else {
		fprintf(stderr, "Omited fill buffer because buffer size is not valid\n");
		return -1;
	}

	return 0;
}

int holder_pill_buffer(char*buff, ssize_t buff_sz) {

	if(!buff) {
		fprintf(stderr, "Dest buffer is NULL\n");
		return -1;
	}

	if(holder_curr->fl == HOLDER_FILLED) {
		/*Only accept buffer with the same size*/
		if(holder_curr->buff_sz == buff_sz) {
			memcpy(buff, holder_curr->buff, holder_curr->buff_sz);
			holder_curr->fl = HOLDER_PILLED;
			holder_curr = holder_curr->next;
			return 0;
		} else {
			fprintf(stderr, "Omited fill buffer because buffer size is not valid\n");
			return -1;
		}
	}
	return -1;
}

int holder_get_free_buffer(char** buff, ssize_t *sz) {
	holder_curw->fl = HOLDER_PENDING;
	holder_curw->prev->fl = HOLDER_FILLED;
	*sz = holder_curw->buff_sz;
	*buff =  holder_curw->buff;
	holder_curw = holder_curw->next;

	return 0;
}

int holder_get_filled_buffer(char** buff, ssize_t *bsz) {

	if(holder_curr->fl == HOLDER_FILLED) {
		*bsz = holder_curr->buff_sz;
		*buff = holder_curr->buff;
		holder_curr->fl = HOLDER_PILLED;
		holder_curr = holder_curr->next;
		return 0;
	}

	return -1;
}



