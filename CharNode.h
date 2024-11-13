
#ifndef CHARNODE_H
#define CHARNODE_H

#include <iostream>

int lengthStr(char* str) {
	if (str == nullptr)
		return 0;
	int length = 0;
	while (str[length] != 0) {
		length++;
	}
	return length;
}

bool compareChar(char c1, char c2) { //case insensitive
	if (c1 >= 'A' && c1 <= 'Z')
		c1 += 32;
	if (c2 >= 'A' && c2 <= 'Z')
		c2 += 32;
	return (c1 == c2);
}

bool compareStr(char* str1, char* str2) {
	int len = lengthStr(str1);
	if (len != lengthStr(str2))
		return false;
	for (int i = 0; i < len; i++) {
		if (str1[i] != str2[i])
			return false;
	}
	return true;
}

char* lowercaseStr(char* str) {
	int length = 0;
	while (str[length] != 0) {
		length++;
	}
	char* lowerCase = new char[length + 1];
	for (int i = 0; i <= length; i++) {
		if (str[i]>='A'&&str[i]<='Z')
			lowerCase[i] = str[i]+32;
		else
			lowerCase[i] = str[i];
	}
	return lowerCase;
}

struct CharNode {
	char data;
	CharNode* topNode;
	CharNode* nextNode;
	CharNode* bottomNode;
	CharNode* prevNode;
	int x, y;
	bool highlighted;
	CharNode(char c=0):topNode(nullptr),nextNode(nullptr),bottomNode(nullptr),prevNode(nullptr),x(-1),y(-1),highlighted(false) {
		this->data = c;
	}
	CharNode(const CharNode* copy) {
		this->data = copy->data;
		this->topNode = nullptr;
		this->bottomNode = nullptr;
		this->prevNode = nullptr;
		this->nextNode = nullptr;
		this->x = copy->x;
		this->y = copy->y;
		//return *this;
	}
	void setTopNode(CharNode* topNode) {
		this->topNode = topNode;
	}
	void setBottomNode(CharNode* bottomNode) {
		this->bottomNode = bottomNode;
	}
	int getX() {
		return this->x;
	}
	int getY() {
		return this->y;
	}
	void printDetails() {
		std::cout << "data: " << data << " ";
		topNode? std::cout << " topnode: " << topNode->data : std::cout << " topnode: nullptr";
		bottomNode? std::cout << " bottomnode: " << bottomNode->data : std::cout << " bottomnode: nullptr";
		prevNode? std::cout << " prevnode: " << prevNode->data : std::cout << " prevnode: nullptr";
		nextNode? std::cout << " nextnode: " << nextNode->data : std::cout << " nextnode: nullptr";
		std::cout << "\n";
	}
	void operator=(const CharNode* copy) {
		this->data = copy->data;
		this->topNode = nullptr;
		this->bottomNode = nullptr;
		this->prevNode = nullptr;
		this->nextNode = nullptr;
		this->x = copy->x;
		this->y = copy->y;
		//return *this;
	}
};

#endif
