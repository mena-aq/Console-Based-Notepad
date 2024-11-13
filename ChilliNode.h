
#ifndef CHILLINODE_H
#define CHILLINODE_H

struct ChilliNode {
	char* data;
	ChilliNode** child; 
	int numChild;
	bool visited;

	ChilliNode() :data(nullptr), child(nullptr), numChild(0),visited(false) {}
	ChilliNode(char* word) :child(nullptr),numChild(0),visited(false) {
		int length = 0;
		while (word[length] != 0)
			length++;
		this->data = new char[length+1];
		for (int i = 0; i < length; i++)
			this->data[i] = word[i];
		this->data[length] = 0;
	}
	void addToChild(ChilliNode* newChild) {
		ChilliNode** tmp = new ChilliNode*[numChild + 1];
		for (int i = 0; i < numChild; i++)
			tmp[i] = this->child[i];
		tmp[numChild] = newChild;
		delete[] this->child;
		this->child = tmp;
		numChild++;
	}
	bool containsChild(ChilliNode* child) {
		for (int i = 0; i < this->numChild; i++) {
			if (this->child[i] == child)
				return true;
		}
		return false;
	}
};

#endif
