#ifndef LINENODE_H
#define LINENODE_H

#include "CharNode.h"
#include <Windows.h>

//seperate header file? i despertaely nee to rearrange this
int lengthOfWord(CharNode* start) { // w/ space
	int length = 0;
	while (start != nullptr) {
		length++;
		if (start->data == ' ')
			break;
		start = start->nextNode;
	}
	return length;
}

void clearList(CharNode* &head) {
	CharNode* curr = head;
	while (curr) {
		if (curr->topNode)
			curr->topNode->bottomNode = nullptr;
		if (curr->bottomNode)
			curr->bottomNode->topNode = nullptr;
		CharNode* tmp = curr->nextNode;
		delete curr;
		curr = tmp;
	}
	head = nullptr;
}

struct LineNode {
	CharNode* head;
	CharNode* tail;
	LineNode* nextLine;
	LineNode* prevLine;
	int y;
	const int MAX_LINE_LENGTH; 
	const int MAX_NUM_LINES;

	LineNode(int y,int max_line_length):head(nullptr),tail(nullptr),nextLine(nullptr),prevLine(nullptr),y(y), MAX_LINE_LENGTH(max_line_length),MAX_NUM_LINES(10) {}
	void setHead(CharNode* head) {
		this->head = head;
	}
	void display() {
		CharNode* currNode = head;
		while (currNode && currNode->data != '\n') {
			if (currNode->highlighted)
				std::cout << "\033[33m" << currNode->data << "\033[0m";
			else
				std::cout << currNode->data;
			currNode = currNode->nextNode;
		}
	}
	CharNode* getCharNode(int x) {
		if (x < 0)
			return nullptr;
		CharNode* currNode = head;
		int currX = 0;
		while (currNode && currX < x) {
			currNode = currNode->nextNode;
			currX++;
		}
		return currNode;
	}
	CharNode* insert(char c,int index) { 
		CharNode* currNode = head;
		CharNode* aboveNode = nullptr;
		CharNode* belowNode = nullptr;
		if (prevLine) 
			aboveNode = prevLine->head;
		if (nextLine)
			belowNode = nextLine->head;
		int currIndex = 0;
		while (currNode && currIndex < index - 1) {
			currIndex++;
			currNode = currNode->nextNode;
		}
		if (currNode == nullptr && currIndex < index) //invalid index
			return nullptr;
		CharNode* tmp = new CharNode(c);
		tmp->y = this->y; //set y coordinate
		tmp->x = index;
		if (currNode) { //adjust above and below to currents above and below, else stay at heads
			aboveNode = currNode->topNode;
			belowNode = currNode->bottomNode;
		}
		if (index == 0) { //first node
			if (head == nullptr) //if empty list, assign tail
				tail = tmp;
			tmp->nextNode = head; //links w/ next
			if (tmp->nextNode != nullptr)
				tmp->nextNode->prevNode = tmp;
				shiftNodeLinks(tmp, aboveNode, belowNode,index);
			head = tmp; //links w/ prev
			tmp->prevNode = nullptr;
		}
		else {
			tmp->nextNode = currNode->nextNode; //links w/ next
			if (tmp->nextNode != nullptr)
				tmp->nextNode->prevNode = tmp;
			if (aboveNode)
				aboveNode = aboveNode->nextNode;
			if (belowNode)
				belowNode = belowNode->nextNode;
				shiftNodeLinks(tmp, aboveNode, belowNode,index);
			currNode->nextNode = tmp;
			tmp->prevNode = currNode;
			if (currNode == tail) //if at tail,make new tail
				tail = tmp;
		}
		if (tail->x >= MAX_LINE_LENGTH && !(c == '\n' && index == MAX_LINE_LENGTH))
 			shiftLinesDown(this);
		return tmp;
	}
	CharNode* deleteNode(int x) {
		CharNode* currNode = head;
		CharNode* prevNode = nullptr;
		int index = 0;
		while (currNode && index < x) {
			prevNode = currNode;
			currNode = currNode->nextNode;
			index++;
		}
		if (currNode == nullptr)
			return nullptr;
		if (prevNode) 
			prevNode->nextNode = currNode->nextNode;
		else 
			head = currNode->nextNode;
		if (currNode->nextNode)
			currNode->nextNode->prevNode = prevNode;
		CharNode* aboveNode = currNode->topNode;
		CharNode* belowNode = currNode->bottomNode;
		if (aboveNode)
			aboveNode->bottomNode = nullptr;
		if (belowNode)
			belowNode->topNode = nullptr;
		if (currNode == tail) //if deleting tail
			tail = prevNode;
		shiftNodeLinks(currNode->nextNode, aboveNode, belowNode,index);
		while (aboveNode || belowNode) {
			if (aboveNode) {
				aboveNode->bottomNode = nullptr;
				aboveNode = aboveNode->nextNode;
			}
			if (belowNode) {
				belowNode->topNode = nullptr;
				belowNode = belowNode->nextNode;
			}
		}
		//CharNode* tmp = new CharNode;
		//tmp->data=currNode->data;
		//tmp->x = currNode->x;
		//tmp->y = currNode->y;
		//delete currNode;
		//check if below lines can be shifted up
		if (this->head)
			shiftLinesUp(this);
		//return tmp;
		return currNode;
	}
	void shiftLinesDown(LineNode* thisLine) {
		while (thisLine) {
			if (thisLine->tail->x >= MAX_LINE_LENGTH)
				thisLine->shiftAboveToLineBelow(thisLine, thisLine->nextLine);
			thisLine = thisLine->nextLine;
		}
	}
	void shiftLinesUp(LineNode* thisLine) {
		while (thisLine->nextLine) {
			int lengthOfFirstWordBelow = lengthOfWord(thisLine->nextLine->head);
			if (!thisLine->lineHasEnter() && MAX_LINE_LENGTH - thisLine->tail->x + 1 >= lengthOfFirstWordBelow)
				thisLine->shiftBelowToLineAbove(thisLine, thisLine->nextLine);
			thisLine = thisLine->nextLine;
		}
	}
	void shiftNodeLinks(CharNode* thisNode, CharNode*& aboveNode, CharNode*& belowNode,int index) {
		while (thisNode) {
			thisNode->topNode = aboveNode;
			thisNode->bottomNode = belowNode;
			thisNode->x = index;
			index++;
			if (aboveNode) {
				aboveNode->bottomNode = thisNode;
				aboveNode = aboveNode->nextNode;
			}
			if (belowNode) {
				belowNode->topNode = thisNode;
				belowNode = belowNode->nextNode;
			}
			thisNode = thisNode->nextNode;
		}
	}
	LineNode* insertLineBelow() {
		LineNode* newLine = new LineNode(this->y+1,MAX_LINE_LENGTH);
		newLine->nextLine = this->nextLine;
		if (this->nextLine)
			this->nextLine->prevLine = newLine;
		this->nextLine = newLine;
		newLine->prevLine = this;
		return newLine;
	}
	void shiftToLineBelow(CharNode* startNode) {
		//amake line below
		LineNode* newLine = insertLineBelow();
		if (startNode) { //adjust links
			this->tail = startNode->prevNode; //move tail
			startNode->prevNode->nextNode = nullptr; //detach from line horizontally
			startNode->prevNode = nullptr;
			newLine->head = startNode;
			CharNode* aboveNode = nullptr;
			CharNode* belowNode = nullptr;
			if (newLine->prevLine)
				aboveNode = newLine->prevLine->head;
			if (newLine->nextLine)
				belowNode = newLine->nextLine->head;
			int index = 0;
			newLine->shiftNodeLinks(startNode, aboveNode, belowNode,index); //from startNode onwards set vertical links
			while (startNode) {
				startNode->y++;
				if (startNode->nextNode == nullptr)
					newLine->tail = startNode; //set tail
				startNode = startNode->nextNode;
			}
		}
	}
	void shiftAboveToLineBelow(LineNode* aboveLine, LineNode* belowLine) {
		if (belowLine == nullptr || aboveLine->lineHasEnter()) { //insert a new line
			belowLine = aboveLine->insertLineBelow();
		}
		while (aboveLine->tail) {
			CharNode* aboveTail = aboveLine->tail;
			aboveLine->tail = aboveLine->tail->prevNode;
			CharNode* c = belowLine->moveNodeToHead(aboveTail);
			if (aboveLine->tail && aboveLine->tail->data == ' ')
				break;
		}
	}
	void shiftBelowToLineAbove(LineNode* aboveLine,LineNode* belowLine) {
		CharNode* belowHead = belowLine->head;
		while (belowHead) {
			CharNode* head = belowHead->nextNode;
			aboveLine->moveNodeToTail(belowHead);
			belowLine->head = head;
			if (belowHead->data == ' ')
				break;
			belowHead = head;
		}
	}
	CharNode* moveNodeToHead(CharNode* newHead) {
		if (!newHead)
			return nullptr;
		//remove from where it is in the list
		if (newHead->topNode)
			newHead->topNode->bottomNode = nullptr;
		if (newHead->bottomNode)
			newHead->bottomNode->topNode = nullptr;
		if (newHead->prevNode)
			newHead->prevNode->nextNode = newHead->nextNode;
		if (newHead->nextNode)
			newHead->nextNode->prevNode = newHead->prevNode;
		if (newHead->nextNode)
			shiftNodeLinks(newHead->nextNode, newHead->topNode, newHead->bottomNode,newHead->x);
		newHead->nextNode = nullptr;
		newHead->prevNode = nullptr;
		newHead->topNode = nullptr;
		newHead->bottomNode = nullptr;
		//add to head
		newHead->nextNode = head; 
		if (newHead->nextNode != nullptr)
			newHead->nextNode->prevNode = newHead;
		CharNode* aboveNode = nullptr;
		CharNode* belowNode = nullptr;
		if (this->prevLine)
			aboveNode = this->prevLine->head;
		if (this->nextLine)
			belowNode = this->prevLine->head;
		shiftNodeLinks(newHead, aboveNode, belowNode, 0);
		if (this->head == nullptr)
			this->tail = newHead;
		this->head = newHead; 
		newHead->prevNode = nullptr;
		//set y coordinate
		newHead->y = this->y;
		return newHead;
	}
	CharNode* moveNodeToTail(CharNode* newTail) {
		if (!newTail)
			return nullptr;
		//remove from where it is in the list
		if (newTail->topNode)
			newTail->topNode->bottomNode = nullptr;
		if (newTail->bottomNode)
			newTail->bottomNode->topNode = nullptr;
		if (newTail->prevNode) //else set head !!! FIX THIS
			newTail->prevNode->nextNode = newTail->nextNode;
		if (newTail->nextNode)
			newTail->nextNode->prevNode = newTail->prevNode;
		newTail->nextNode = nullptr;
		newTail->prevNode = nullptr;
		newTail->topNode = nullptr;
		newTail->bottomNode = nullptr;
		//add to tail
		CharNode* curr = this->head;
		while (curr && curr->nextNode)
			curr = curr->nextNode;
		if (curr) {
			curr->nextNode = newTail;
			if (curr->topNode) {
				newTail->topNode = curr->topNode->nextNode;
				if (newTail->topNode)
					newTail->topNode->bottomNode = newTail;
			}
			if (curr->bottomNode) {
				newTail->bottomNode = curr->bottomNode->nextNode;
				if (newTail->bottomNode)
					newTail->bottomNode->topNode = newTail;
			}
		}
		else {
			this->head = newTail;
			if (this->prevLine)
				newTail->topNode = this->prevLine->head;
			if (this->nextLine)
				newTail->bottomNode = this->nextLine->head;
		}
		newTail->prevNode = curr;
		this->tail = newTail;
		//set y coordinate
		newTail->y = this->y;
		return newTail;
	}
	bool nodeExists(int x) {
		int i = 0;
		CharNode* curr = head;
		while (curr && i<x) {
			curr = curr->nextNode;
			i++;
		}
		if (curr)
			return true;
		return false;
	}
	bool lineHasEnter() {
		CharNode* curr = this->head;
		while (curr) {
			if (curr->data == '\n')
				return true;
			curr = curr->nextNode;
		}
		return false;
	}
	CharNode* getStartChar(CharNode* end) {
		CharNode* curr = end;
		while (curr->prevNode && curr->data != ' ')
			curr = curr->prevNode;
		return curr;
	}
	~LineNode() {
		CharNode* curr = head;
		while (curr) {
			if (curr->topNode)
				curr->topNode->bottomNode = nullptr;
			if (curr->bottomNode)
				curr->bottomNode->topNode = nullptr; 
			CharNode* tmp = curr->nextNode;
			delete curr;
			curr = tmp;
		}
	}
};

#endif
