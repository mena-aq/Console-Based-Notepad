#ifndef TRIENODE_H
#define TRIENODE_H

#include "CharNode.h"

struct TrieNode {
	char data;
	TrieNode* child[26];
	CharNode** instances;
	int numInstances;
	bool wordEnd;
	TrieNode() :data('\0'), instances(nullptr), numInstances(0), wordEnd(false) { //root
		for (int i = 0; i < 26; i++)
			child[i] = nullptr;
	}
	TrieNode(char data) :data(data),instances(nullptr),numInstances(0),wordEnd(false) {
		for (int i = 0; i < 26; i++)
			child[i] = nullptr;
	}
	TrieNode(TrieNode &copy):instances(nullptr),numInstances(0),wordEnd(false) {
		for (int i = 0; i < 26; i++)
			child[i] = nullptr;
		this->data = copy.data;
		this->wordEnd = copy.wordEnd;
	}
	void addInstance(CharNode* listNode) {
		CharNode** tmp = new CharNode * [numInstances + 1];
		for (int i = 0; i < numInstances; i++) {
			tmp[i] = instances[i];
		}
		tmp[numInstances] = listNode;
		delete[] instances;
		instances = tmp;
		numInstances++;
	}
	void deleteInstance(CharNode* listNode) {
		int toDelete = -1;
		for (int i = 0; i < numInstances; i++) {
			if (instances[i] == listNode) {
				toDelete = i;
				break;
			}
		}
		if (toDelete <= -1)
			return;
		if (numInstances <= 1) {
			delete[] instances;
			instances = nullptr;
			numInstances = 0;
			return;
		}
		CharNode** tmp = new CharNode * [numInstances - 1];
		int j = 0;
		for (int i = 0; i < numInstances; i++) {
			if (i != toDelete)
				tmp[j++] = instances[i];
		}
		delete[] instances;
		instances = tmp;
		numInstances--;
	}
	bool containsInstance(CharNode* str) {
		for (int i = 0; i < numInstances; i++) {
			if (str == instances[i])
				return true;
		}
		return false;
	}
	void highlightInstances(/*Cursor& cursor*/ ) {
		for (int i = 0; i < numInstances; i++) {
			//set color for each listnode
			instances[i]->highlighted = true;
			//cursor.moveCursorDown();
			//cout << "line: " << instances[i]->y << " ";
		}
	}
	bool isLeaf() {
		for (int i = 0; i < 26; i++) {
			if (child[i])
				return false;
		}
		return true;
	}
	TrieNode* operator[](int index) {
		return child[index];
	}
	void operator=(TrieNode* copy) {
		this->data = copy->data;
		this->wordEnd = copy->wordEnd;
	}
};


#endif
