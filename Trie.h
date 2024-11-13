#ifndef TRIE_H
#define TRIE_H

#include "string.h"
using namespace std;

#include "CharNode.h"
#include "TrieNode.h"
#include "Suggestion.h"
#include "Cursor.h"

class Trie {
public:
	TrieNode* root;
	TrieNode* lastInserted;
	TrieNode* searchPtr;

	Trie():lastInserted(nullptr) {
		this->root = new TrieNode;
		searchPtr = root;
	}
	void display(TrieNode* root,char* str,int index,Cursor& cursor) {
		if (root == nullptr) {
			return;
		}
		if (root->data != '\0') { //not root
			//int len = lengthStr(str)+1;
			//char* tmp = new char[len + 1];
			//for (int i = 0; i < len-1; i++)
			//	tmp[i] = str[i];
			//tmp[len-1] = root->data;
			//tmp[len] = 0; //null terminate
			//if (str)
			//	delete[] str;
			//str = tmp;
			str[index] = root->data;
		}
		if (root->wordEnd) {
			cout << str << endl;
			cursor.moveCursorDown();
		}
		for (int i = 0; i < 26; i++)
			display(root->child[i],str,index+1,cursor);
		str[index] = 0;
	}
	void insert(CharNode* newC, CharNode* str) {
		char nodeData = newC->data;
		char strData = str->data;
		if (nodeData >= 'A' && nodeData <= 'Z') //lowercase it for tree
			nodeData += 32;
		if (strData >= 'A' && strData <= 'Z') //lowercase it for tree
			strData += 32;

		TrieNode* curr = root->child[strData - 'a'];
		TrieNode* prev = nullptr;
		if (newC->prevNode==nullptr||newC->prevNode->data==' ') { //root
			if (curr == nullptr) { //make root node only if not already made for that letter
				TrieNode* newNode = new TrieNode(nodeData);
				newNode->addInstance(newC);
				root->child[nodeData - 'a'] = newNode;
				lastInserted = newNode;
				//newNode->wordEnd = true;
			}
			else {
				root->child[nodeData - 'a']->addInstance(newC);
				lastInserted = root->child[nodeData - 'a'];
				//lastInserted->wordEnd = true;
			}
		}
		else { //internal/leaf
			while (curr) {
				prev = curr;
				if (compareChar(curr->data,newC->prevNode->data) && curr->containsInstance(newC->prevNode)) {
					if (curr->child[nodeData - 'a'] == nullptr) {
						bool changeWordEnd = curr->isLeaf();
						TrieNode* newNode = new TrieNode(nodeData);
						newNode->addInstance(newC);
						curr->child[nodeData - 'a'] = newNode;
						lastInserted = newNode;
						//t = newNode;

						if (lastInserted->isLeaf() && (newC->nextNode==nullptr|| newC->nextNode->data == ' ')) {
							lastInserted->wordEnd = true;
							if (curr->wordEnd && changeWordEnd)
								curr->wordEnd = false;
						}
					}
					else {
						curr->child[nodeData - 'a']->addInstance(newC);
						lastInserted = curr->child[nodeData - 'a'];
					}

					break;
				}
				str = str->nextNode;
				strData = str->data;
				if (strData >= 'A' && strData <= 'Z') 
					strData += 32;
				curr = curr->child[strData - 'a'];
			}
		}
		
		str = newC->nextNode;
		if (str == nullptr || str->data == ' ') {
			return;
		}
		strData = str->data;
		if (strData >= 'A' && strData <= 'Z')
			strData += 32;
		TrieNode* node = nullptr;
		if (prev) {
			moveBranch(curr->child[strData - 'a'], lastInserted->child[str->data - 'a'], str);
		}
		else {
			moveBranch(root->child[strData - 'a'], lastInserted->child[str->data-'a'], str);
		}
	}

	void deleteNode(CharNode* delC, CharNode* str) {
		char nodeData = delC->data;
		char strData = str->data;
		if (nodeData >= 'A' && nodeData <= 'Z') //lowercase it for tree
			nodeData += 32;
		if (strData >= 'A' && strData <= 'Z') //lowercase it for tree
			strData += 32;

		TrieNode* curr = root->child[strData - 'a'];
		TrieNode* prev = root; //root
		while (curr) {
			if (curr->containsInstance(delC)) { //delete curr
				curr->deleteInstance(str);
				if (curr->numInstances > 0)
					return;
				str = str->nextNode;
				if (!(str== nullptr || str->data == ' ')) {
					strData = str->data;
					if (strData >= 'A' && strData <= 'Z') //lowercase it for tree
						strData += 32;
					moveBranch(curr->child[strData - 'a'], prev->child[strData - 'a'], str);
				}
				TrieNode* tmp = curr;
				prev->child[curr->data - 'a']=nullptr;
				if (prev->isLeaf())
					prev->wordEnd = true;
				delete tmp;
				break;
			}
			str = str->nextNode;
			strData = str->data;
			if (strData >= 'A' && strData <= 'Z') //lowercase it for tree
				strData += 32;
			prev = curr;
			curr = curr->child[strData - 'a']; 
		}
	}

	void moveBranch(TrieNode* &node1,TrieNode* &node2, CharNode* str) {
		TrieNode* copy;
		if (node2==nullptr)
			node2 = new TrieNode(*node1);
		node2->addInstance(str);
		node1->deleteInstance(str);

		str = str->nextNode;
		if (str == nullptr || str->data ==' ') { //base case
			if (node1->numInstances == 0) {
				delete node1;
				node1 = nullptr;
			}
			return;
		}
		char strData = str->data;
		if (strData >= 'A' && strData <= 'Z') //lowercase it for tree
			strData += 32;
		moveBranch(node1->child[strData - 'a'], node2->child[strData - 'a'], str);
		if (node1->numInstances == 0) { //general case
			delete node1;
			node1 = nullptr;
		}
		return;
	}

	void resetSearchPtr() {
		searchPtr = root;
	}
	void backSearchPtr(char* searchBox) {
		if (searchPtr) {
			for (int i = 0; i < searchPtr->numInstances; i++) {
				searchPtr->instances[i]->highlighted = false;
			}
			int index = 0;
			TrieNode* newSPtr = root;
			while (searchBox[index] != 0) {
				if (searchBox[index]!=' ' && newSPtr->child[searchBox[index] - 'a'])
					newSPtr = newSPtr->child[searchBox[index] - 'a'];
				else
					newSPtr = root;
				index++;
			}
			searchPtr = newSPtr;
			newSPtr = nullptr;
		}
	}
	void search(char sChar,char* searchBox) {
		if (sChar >= 'A' && sChar <= 'Z') //lowercase
			sChar += 32;
		char* lowerCaseSearchBox = lowercaseStr(searchBox);

		if (sChar == ' ') {
			resetSearchPtr();
			return;
		}
		if (searchPtr->child[sChar - 'a']) {
			searchPtr = searchPtr->child[sChar - 'a'];
			highlightSearchResults(root->child[lowerCaseSearchBox[0]-'a'], lowerCaseSearchBox, 0);
		}

		delete[] lowerCaseSearchBox;
	}
	//void search(char* searchBox) {
	//	if (root->child[searchBox[0] - 'a']) {
	//		highlightSearchResults(root->child[searchBox[0] - 'a'], searchBox, 0);
	//	}
	//}
	void highlightSearchResults(TrieNode* root,char* searchBox,int index) {
		if (searchBox[index] == ' '|| root==nullptr) {
			root = this->root;
			//index++;
		}
		else {
			if (root == searchPtr) {
				root->highlightInstances();
				return;
			}
			for (int i = 0; i < root->numInstances; i++) {
				root->instances[i]->highlighted = false;
			}
		}
		highlightSearchResults(root->child[searchBox[index + 1]-'a'], searchBox, index + 1);
		for (int i = 0; i < root->numInstances; i++) {
			if (root->instances[i]->nextNode != nullptr) {
				if (root->instances[i]->nextNode->highlighted)
					root->instances[i]->highlighted = true;
				else if (root->instances[i]->nextNode->data == ' ' && root->instances[i]->nextNode->nextNode && root->instances[i]->nextNode->nextNode->highlighted)
					root->instances[i]->highlighted = true;
			}
		}
	}
	void unhighlightNodes(TrieNode* root,char* searchBox, int index) {
		if (root == nullptr) {
			root = this->root;
		}
		else {
			for (int i = 0; i < root->numInstances; i++) 
				root->instances[i]->highlighted = false;
			if (root == searchPtr) 
				return;
		}
		unhighlightNodes(root->child[searchBox[index] - 'a'], searchBox, index + 1);
	}
	/*void highlightSearchResults(TrieNode* root, char* searchBox, int index) {
		if (searchBox[index+1]==0) {
			root->highlightInstances();
			return;
		}
		for (int i = 0; i < root->numInstances; i++) {
			root->instances[i]->highlighted = false;
		}
		highlightSearchResults(root->child[searchBox[index + 1] - 'a'], searchBox, index + 1);
		for (int i = 0; i < root->numInstances; i++) {
			if (root->instances[i]->nextNode != nullptr && root->instances[i]->nextNode->highlighted)
				root->instances[i]->highlighted = true;
		}
	}*/
	//void suggest(char* word,Suggestion* suggestions,Suggestion* &lastSuggestion,int numSuggestions) {
	//	TrieNode* node = root;
	//	int index = 0;
	//	while (node && word[index] != 0) {
	//		node = node->child[word[index] - 'a'];
	//		index++;
	//	}
	//	if (node == nullptr)
	//		return;
	//	//if (this->suggestions) {
	//	//	for (int i = 0; i < this->numSuggestions; i++)
	//	//		delete suggestions[i];
	//	//	delete[] this->suggestions;
	//	//}
	//	//this->numSuggestions = 0;
	//	char copy[20];
	//	for (int i = 0; i < 20; i++)
	//		copy[i] = word[i];
	//	for (int i=0; i<26; i++)
	//		getSuggestions(node->child[i], copy, index, suggestions, lastSuggestion,numSuggestions);
	//}
	void getSuggestions(TrieNode* root, char* suggestion, int index,Suggestion* &suggestions,Suggestion* &lastSuggestion,int &numSuggestions) {
		if (root == nullptr)
			return;
		suggestion[index] = root->data;
		if (root->wordEnd) { //add to suggestions
			if (suggestions) {
				lastSuggestion->next = new Suggestion(suggestion, index + 1);
				lastSuggestion = lastSuggestion->next;
			}
			else {
				suggestions = new Suggestion(suggestion, index + 1);
				lastSuggestion = suggestions;
			}
			numSuggestions++;
		}
		for (int i = 0; i < 26; i++)
			getSuggestions(root->child[i], suggestion, index + 1,suggestions,lastSuggestion,numSuggestions);
	}
	//void displaySuggestions() {
	//	Suggestion* currSuggestion = suggestions;
	//	while (currSuggestion) {
	//		if (currSuggestion->selected) 
	//			cout << "\033[32m" << currSuggestion->suggestion << "\033[0m" << ", ";
	//		else
	//			cout << currSuggestion->suggestion << ", ";
	//		currSuggestion = currSuggestion->next;
	//	}
	//}
	//void clearSuggestions() {
	//	lastSuggestion = nullptr;
	//	while (suggestions) {
	//		Suggestion* delSug = suggestions;
	//		suggestions = suggestions->next;
	//		delete delSug;
	//	}
	//}
};

#endif


// i have to change the structure so theres an empty root node (then itll be easier to generalise etc)

//if im inserting at a leaf, check if need to make word end
//if deleting a leaf also