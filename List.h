#ifndef LIST_H
#define LIST_H

#include "LineNode.h"

class List {
private:
	LineNode* head;
	const int MAX_LINE_LENGTH; 
	const int MAX_NUM_LINES;
public:
	List(int max_num_lines,int max_line_length) :head(nullptr),MAX_NUM_LINES(max_num_lines),MAX_LINE_LENGTH(max_line_length){}
	List(const List& copy):MAX_LINE_LENGTH(copy.MAX_LINE_LENGTH),MAX_NUM_LINES(copy.MAX_NUM_LINES) {
		LineNode* currCopy = copy.head;
		int lineIndex = 0;
		while (currCopy) {
			LineNode* newLine = this->insertLine(lineIndex);
			CharNode* currChar = copy.getLine(lineIndex)->head;
			int index = 0;
			while (currChar) {
				newLine->insert(currChar->data, index);
				currChar = currChar->nextNode;
				index++;
			}
			currCopy = currCopy->nextLine;
			lineIndex++;
		}
	}
	LineNode* getHead() {
		return this->head;
	}
	int countNumLines () const {
		LineNode* currLine = head;
		int numLines = 0;
		while (currLine) {
			numLines++;
			currLine = currLine->nextLine;
		}
		return numLines;
	}
	void display() { //by moving cursor and printing and then restore pos
		LineNode* curr = head;
		while (curr) {
			curr->display();
			std::cout << "\n"; //not printing the lines own \n (formatting>raw text)
			curr = curr->nextLine;
		}
	}
	LineNode* insertLine(int line) { 
		LineNode* currNode = this->head;
		int currLine = 0;
		while (currNode && currLine < line - 1) {
			currLine++;
			currNode = currNode->nextLine;
		}
		if (currNode == nullptr && currLine > 0)
			return nullptr;
		LineNode* tmp = new LineNode(line,MAX_LINE_LENGTH);
		if (line == 0) {
			tmp->nextLine = head;
			head = tmp;
			if (tmp->nextLine != nullptr)
				tmp->nextLine->prevLine = tmp;
			tmp->prevLine = nullptr;
		}
		else {
			tmp->nextLine = currNode->nextLine;
			currNode->nextLine = tmp;
			if (tmp->nextLine != nullptr)
				tmp->nextLine->prevLine = tmp;
			tmp->prevLine = currNode;
		}
		//numLines++;
		return tmp;
	}
	LineNode* insertLineWithText(int line, char* text) {
		LineNode* newLine = this->insertLine(line);
		//add text
		int i = 0;
		int index = 0;
		while (text[i] != '\n' && text[i]!='\0') {
			CharNode* insertedChar = this->insertAt(text[i], index, line);
			if (insertedChar) {
				index = insertedChar->getX() + 1;
				line = insertedChar->getY();
			}
			i++;
		}
		CharNode* insertedChar = this->insertAt('\n', index, line);
		return newLine;
	}
	CharNode* insertAt(char c,int x, int y) {
		int currY = 0;
		LineNode* currNode = head;
		while (currNode && currY < y) {
			currNode = currNode->nextLine;
			currY++;
		}
		if (currNode) {
			CharNode* newNode = currNode->insert(c, x);
			return newNode;
		}
		return nullptr;
	}
	void deleteLine(int line) { //called once line's head is null so no need to shift
		LineNode* currLine = head;
		LineNode* prevLine = nullptr;
		int index = 0;
		while (currLine && index<line) {
			prevLine = currLine;
			currLine = currLine->nextLine;
			index++;
		}
		if (currLine == nullptr)
			return;
		if (prevLine) 
			prevLine->nextLine = currLine->nextLine;
		else 
			head = currLine->nextLine;
		if (currLine->nextLine)
			currLine->nextLine->prevLine = prevLine;
		delete currLine;
	}
	CharNode* deleteAt(int x,int y) {
		int currY = 0;
		LineNode* currNode = head;
		while (currNode && currY < y) {
			currNode = currNode->nextLine;
			currY++;
		}
		if (currNode) {
			CharNode* delNode = currNode->deleteNode(x);
			return delNode;
		}
		return nullptr;
	}
	LineNode* getLine(int line) const {
		int currLine = 0;
		LineNode* currNode = head;
		while (currNode && currLine < line) {
			currNode = currNode->nextLine;
			currLine++;
		}
		return currNode;
	}
	CharNode* getCharNode(int x,int y) {
		if (y < 0)
			return nullptr;
		LineNode* currNode = head;
		int currY = 0;
		while (currNode && currY < y) {
			currNode = currNode->nextLine;
			currY++;
		}
		if (currNode)
			return currNode->getCharNode(x);
		return nullptr;
	}
	bool nodeExists(int x,int y) {
		int i = 0;
		LineNode* curr = head;
		while (curr && i < y) {
			curr = curr->nextLine;
			i++;
		}
		if (curr)
			return curr->nodeExists(x);
		return false;
	}
	bool lineExists(int y) {
		int i = 0;
		LineNode* curr = head;
		while (curr && i < y) {
			curr = curr->nextLine;
			i++;
		}
		if (curr)
			return true;
		return false;
	}
	List& operator=(const List* copy) {
		//clear
		LineNode* curr = head;
		while (curr) {
			LineNode* tmp = curr->nextLine;
			delete curr;
			curr = tmp;
		}
		this->head = nullptr;
		//copy
		LineNode* currCopy = copy->head;
		int lineIndex = 0;
		while(currCopy) {
			LineNode* newLine = this->insertLine(lineIndex);
			CharNode* currChar = copy->getLine(lineIndex)->head;
			int index = 0;
			while (currChar) {
				newLine->insert(currChar->data, index);
				currChar = currChar->nextNode;
				index++;
			}
			currCopy = currCopy->nextLine;
			lineIndex++;
		}
		return *this;
	}

	void shiftBelowToLineAbove(LineNode* aboveLine, LineNode* belowLine) {
		CharNode* aboveTail = aboveLine->head;
		CharNode* belowHead = belowLine->head;
		while (aboveTail && aboveTail->nextNode != nullptr) {
			aboveTail = aboveTail->nextNode;
		}
		while (belowHead) {
			belowHead = belowHead->nextNode;
			CharNode* c = belowLine->deleteNode(0);
			aboveLine->insert(c->data, aboveTail->x + 1);
			aboveTail = aboveTail->nextNode;
			if (c->data == ' ')
				break;
		}
	}
	~List() {
		LineNode* curr = head;
		while (curr) {
			LineNode* tmp = curr->nextLine;
			delete curr;
			curr = tmp;
		}
	}
};

#endif