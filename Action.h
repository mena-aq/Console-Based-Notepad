#ifndef ACTION_H
#define ACTION_H

#include "List.h"

class Action {
//private:
public:
	int x, y;
	bool inserted; //true for insert,false for delete
	CharNode* word; //list for word
	Action* next;
	Action* prev;
public:
	Action(bool inserted,CharNode* end) :inserted(inserted),next(nullptr),prev(nullptr) {
		word = nullptr;
		CharNode* curr = end;
		while (curr){
			if (word) {
				CharNode* tmp = new CharNode(curr->data);
				tmp->nextNode = word;
				word = tmp;
			}
			else {
				word = new CharNode(curr->data);
			}
			if (curr->data == ' ' || !curr->prevNode)
				break;
			curr = curr->prevNode;
		}
		this->x = curr->getX();
		this->y = curr->getY();
	} 
	Action(int x, int y, bool inserted, CharNode* curr) :x(x), y(y), inserted(inserted),next(nullptr),prev(nullptr),word(nullptr) {
		CharNode* tail = nullptr;
		while (curr) {
			if (this->word) {
				tail->nextNode = new CharNode(curr->data);
				tail = tail->nextNode;
			}
			else {
				this->word = new CharNode(curr->data);
				tail = this->word;
			}
			curr = curr->nextNode;
		}
	}
};

#endif