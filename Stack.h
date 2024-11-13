#ifndef STACK_H
#define STACK_H

#include "Action.h"

class Stack {
public:
	Action* stackptr; //head
	Action* baseptr; //tail
	int currSize;
	const int capacity;
public:
	Stack() :stackptr(nullptr),baseptr(nullptr),currSize(0),capacity(5) {}
	void push(Action* node) {
		if (currSize == capacity) {
			baseptr = baseptr->prev;
			delete baseptr->next;
			baseptr->next = nullptr;
			currSize--;
		}
		if (stackptr == nullptr)
			baseptr = node;
		node->next = stackptr;
		if (node->next)
			node->next->prev = node;
		node->prev = nullptr;
		stackptr = node;
		currSize++;
	}
	Action* pop() {
		Action* tmp = stackptr;
		if (stackptr)
			stackptr = stackptr->next;
		currSize--;
		if (!stackptr)
			baseptr = nullptr;
		return tmp;
	}
	~Stack() {
		Action* curr = stackptr;
		while (curr) {
			Action* tmp = curr->next;
			delete curr;
			curr = tmp;
		}
	}
};

#endif

