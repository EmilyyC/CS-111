#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include "SortedList.h"

// extern variable initially declared in SortedList.h
int opt_yield;

// *list is the head of the list, *element is the element to be added to the list
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	SortedList_t *head = list;  // save the head of the list
	SortedList_t *currElement = list->next;

	// give up the CPU to another thread before critical section
	if (opt_yield & INSERT_YIELD) {
		sched_yield();
	}

	// iterate through the list to find where to insert the element
	while (currElement != head) {
		// element should be less than the next element
		if (strcmp(element->key, currElement->key) <= 0) {
			break;  // found the element to come after the element to be inserted
		}
		// move down the list
		currElement = currElement->next;
	}

	// insert element into the list
	element->prev = currElement->prev;
	element->next = currElement;
	currElement->prev->next = element;
	currElement->prev = element;
}

// *element is the element to be deleted
// return 0 if the element is deleted successfully, return 1 for corrupted next/prev pointers
int SortedList_delete( SortedListElement_t *element) {
	// check the next/prev pointers before deleting
	if ((element->prev->next != element) || (element->next->prev != element))
		return 1;

	// give up the CPU to another thread before critical section 
	if (opt_yield & DELETE_YIELD) {
		sched_yield();
	}

	// list is not corrupted, so delete the element
	element->prev->next = element->next;
	element->next->prev = element->prev;
	free(element);   // free the memory
	return 0;
}

// *list is the head of the list, *key is the desired key
// return pointer to matching element or NULL if there are none
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
	SortedListElement_t *currElement = list->next;  // start at the first real element

	// give up the CPU to another thread before critical section
	if (opt_yield & LOOKUP_YIELD) {
		sched_yield();
	}

	while (currElement != list) {
		if (strcmp(currElement->key, key) == 0)
			return currElement;
		
		// move down the list
		currElement = currElement->next;
	}
	return NULL;   // couldn't find key
}

// *list is the head of the list
// return the number of elements in the list (excluding dummy head node) or -1 if the list is corrupted
int SortedList_length(SortedList_t *list) {
	int length = 0;
	SortedListElement_t *currElement = list->next;  // start at the first real element

	// give up the CPU to another thread before critical section
	if (opt_yield & LOOKUP_YIELD) {
		sched_yield();
	}

	while (currElement != list) {
		// check too see if the list's next/prev pointers work correctly
		if ((currElement->prev->next != currElement) || (currElement->next->prev != currElement)) {
			return -1;  // list is corrupted
		}
		++length;
		currElement = currElement->next;
	}
	return length;
}