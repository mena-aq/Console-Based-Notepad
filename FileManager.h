#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "List.h"
#include "Trie.h"
#include "ChilliTree.h"
#include <fstream>
#include <iostream>
using namespace std;

class FileManager {
private:
	List* list;
	Trie* trie;
	ChilliTree* chilliTree;
	char* file;
public:
	void readFile(char* readFile,List& list,Trie& trie,ChilliTree& chilliTree) {
		//read file into linked list
		fstream file;
		file.open(readFile, ios::in|ios::out);
		int lineNum = 0;
		//create file if new
		if (!file.is_open()) {
			file.clear();
			file.open(readFile, ios::out);
			file.close();
			file.open(readFile, ios::in);
		}
		char ch;
		int index = 0;
		int line = 0;
		list.insertLine(0); 
		while (file.get(ch)) {
			CharNode* insertedChar = list.insertAt(ch, index, line);
			if (ch == '\n') { //new line
				//chillitree
				char word[20];
				for (int i = 0; i < 20; i++)
					word[i] = 0;
				CharNode* tmp = insertedChar->prevNode;
				int i = 0;
				while (!(tmp->prevNode == nullptr || tmp->prevNode->data == ' ')) {
					tmp = tmp->prevNode;
				}
				while (tmp && tmp->data != ' ' && tmp->data != '\n') {
					word[i++] = tmp->data;
					tmp = tmp->nextNode;
				}
				//word has the word to add to chillitree
				chilliTree.addNode(word);
				list.insertLine(++line);
				index = 0;
			}
			else {
				if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
					CharNode* start = insertedChar;
					while (!(start->prevNode == nullptr || start->prevNode->data == ' '))
						start = start->prevNode;
					trie.insert(insertedChar, start);
				}
				if (ch == ' ' /*|| file.peek() == EOF*/) {
					//chillitree
					char word[20];
					for (int i = 0; i < 20; i++)
						word[i] = 0;
					CharNode* tmp = insertedChar->prevNode;
					int index = 0;
					while (!(tmp->prevNode == nullptr || tmp->prevNode->data == ' ')) {
						tmp = tmp->prevNode;
					}
					while (tmp && tmp->data != ' ') {
						word[index++] = tmp->data;
						tmp = tmp->nextNode;
					}
					//word has the word to add to chillitree
					chilliTree.addNode(word);
				}
				line = insertedChar->getY();
				index = insertedChar->getX() + 1;
			}

		}
		file.close();
		this->file = readFile; 
		this->list = &list;
	}
	void writeFile() {
		fstream file;
		file.open(this->file, ios::out);
		if (file.is_open()) {
			int numLines = this->list->countNumLines();
			for (int i = 0; i < numLines; i++) {
				LineNode* line = this->list->getLine(i);
				int numCharsInLine = line->tail->x+1;
				for (int j = 0; j < numCharsInLine; j++)
					file.put(line->getCharNode(j)->data);
			}
		}
	}
};

#endif