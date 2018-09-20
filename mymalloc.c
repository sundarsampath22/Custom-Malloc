#include <stdio.h>
#include "mymalloc.h"
#include <unistd.h>
#define NODESIZE sizeof(struct Node)

//Structure to keep track of adjacent allocation
struct Node {
	size_t size;
	struct Node *next;
	struct Node *prev;
	int free;
};

void *head = NULL;
void *tail = NULL;

//Returns a pointer to a region in the heap whose size best matches that of the request
static struct Node *bestfit(size_t size) {
	struct Node *temp = head;
	struct Node *best = NULL;
	int diff = -1;
	if (temp->free && temp->size == size) { //If the head of the linked list has the space we need, no need to search
		return temp;
	}
	while (temp) {
		if (temp->free && temp->size >= size) {
			if (temp->size - size < diff || diff == -1) {
				diff = temp->size - size;
				best = temp;
			}
		}
		temp = temp->next;
	}
	return best;
}

//Returns a pointer to a node containing info about the subsequent allocated block
static struct Node *get_space(size_t size) {
	//Allocate space
	struct Node *node; 
	node = sbrk(0); //Current brk at heap
	void *space = sbrk(NODESIZE + size); //Allocates space for request + linked list node 
	node->size = size; 
	node->next = NULL;
	node->free = 0;
	return node;
}

//Allocates a region of memory of given size if memory can be allocated.
//Returns a pointer to this allocated region.
//Parameters: An integer rperesenting the size of request
void *my_bestfit_malloc(int size) { 
	if (size <= 0) {
		return NULL;
	}
	struct Node *node;
	if (!head) { //First Call
		//call to request space
		node = get_space(size);
		node->prev = NULL; 
		head = node; 
	}
	else {
		node = bestfit(size);
		if (!node) {
			node = get_space(size);
			if (!node) {
				return NULL;	
			}
			node->prev = tail;
			tail = node;
		}
	}
	node->free = 0;
	return (node+1);
}

//Deallocates a region of memory pointed to by given pointer
void my_free(void *ptr) { 
	struct Node *node = (struct Node*)ptr - 1;
	struct Node *tempprev = node->prev;
	struct Node *tempnext = node->next;
	//Check if previous node is free, if it is coallesce and free
	if (tempprev && tempprev->free) {
		struct Node *prevprev = tempprev->prev;
		node->size += tempprev->size + NODESIZE;
		if (prevprev) {
			prevprev->next = node;
			node->prev = prevprev;
		}
		else {
			head = node;
		}
	}
	if (tempnext && tempnext->free) {
		node->next = tempnext->next;
		node->size += tempnext->size + NODESIZE;
	}
	if (node == tail) {
		if (node == head) {
			head = NULL;
			tail = NULL;
			node = NULL;
			sbrk((node->size)* (-1) + NODESIZE);
		}
		else {
			struct Node *temptail = node->prev;
			tail = temptail;
			int size = node->size;
			int offset = size * (-1) + NODESIZE;
			sbrk(offset);
		}
	}
	node->free = 1;
}
