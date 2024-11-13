
#ifndef CHILLITREE_H
#define CHILLITREE_H

#include "ChilliNode.h"
#include "CharNode.h"

class ChilliTree {
public:
	ChilliNode* root;
	ChilliNode* cptr; //current node
	ChilliNode* tptr; //last 'level' (always a new node)

	int numNodes;

	ChilliTree() :root(nullptr),cptr(nullptr),tptr(nullptr),numNodes(0) {}
	//print
	void display(ChilliNode* node,int& num) {
		if (node == nullptr||node->visited)
			return;
		cout << node->data << " ";
		node->visited = true;
		num++;
		//if (node == tptr)
		//	lastLevel = true;
		if (num == numNodes) {
			node->visited = false;
			return;
		}
		/*if (!lastLevel) {*/
		for (int i = 0; i < node->numChild; i++) {
			if (node->child[i] != node)
				display(node->child[i], num);
		}
		node->visited = false;
		//}
		return;
	}
	void displayTree() {
		bool ll = false;
		cout << "chilli tree: ";
		int num = 0;
		display(root, num);
	}
	//search node
	ChilliNode* search(ChilliNode* node,char* word,int& num) {
		if (node == nullptr || node->visited)
			return nullptr;
		if (compareStr(node->data,word)) {
			return node;
		}
		node->visited = true;
		num++;
		if (num == numNodes) {
			node->visited = false;
			return nullptr;
		}
		for (int i = 0; i < node->numChild; i++) {
			if (node->child[i] != node) {
				ChilliNode* found = search(node->child[i], word, num);
				if (found) {
					node->visited = false;
					return found;
				}
			}
		}
		node->visited = false;
		return nullptr;
		return nullptr;
	}
	//add node
	void addNode(char* word) { 
		if (root == nullptr) { //first node
			root = new ChilliNode(word);
			cptr = root;
			tptr = root;
			numNodes = 1;
		}
		else {
			//search node
			ChilliNode* newChild = new ChilliNode(word);
			bool ll = false;
			int num = 0;
			ChilliNode* duplicate = search(root, word, num);
			if (duplicate) { //already in tree
				if (!cptr->containsChild(duplicate)) //if not already child
					cptr->addToChild(duplicate); //make child
				cptr = duplicate;
			}
			else {
				cptr->addToChild(newChild);
				//if (cptr->numChild == 1) //first child different from itself
				//	tptr = newChild; //only for new
				numNodes++;
				cptr = newChild;
			}
		}
	}

	void getSuggestions(char* word,Suggestion*& suggestions,Suggestion*& lastSuggestion,int& numSuggestions) {
		bool ll = false;
		int num = 0;
		ChilliNode* parent = search(root, word,num);
		if (!parent)
			return; //no suggestion
		for (int i = 0; i < parent->numChild; i++) {
			if (suggestions == nullptr) {
				suggestions = new Suggestion(parent->child[i]->data);
				lastSuggestion = suggestions;
			}
			else {
				lastSuggestion->next = new Suggestion(parent->child[i]->data);
			}
			numSuggestions++;
		}
	}

	//delete node(might not end up doing this tbh)
};

#endif

//chilli 55