#ifndef CONSOLE_H
#define CONSOLE_H

#include "FileManager.h"
#include "Cursor.h"
#include "List.h"
#include "Stack.h"
#include "Trie.h"
#include "ChilliTree.h"
#include <iostream>
using namespace std;

class Console {
public:
	int X_BOUNDARY;
	int Y_BOUNDARY;
	Cursor cursor;
	List list;
	Stack undoStack;
	Stack redoStack;

	Trie trie;
	char searchBox[20]; //for display
	char* lowerCaseSearchBox; //for processing
	int sindex = 0;

	// linked list of suggestions (for both word and sentence completion)
	Suggestion* suggestions; //sll, head
	Suggestion* lastSuggestion; //tail
	int numSuggestions;
	Suggestion* selectSuggestion;

	ChilliTree chilliTree;

	Console(CONSOLE_SCREEN_BUFFER_INFO info):X_BOUNDARY(0.8 * (info.srWindow.Right - info.srWindow.Left)), Y_BOUNDARY(0.8 * (info.srWindow.Bottom - info.srWindow.Top)), list(Y_BOUNDARY ,X_BOUNDARY) {
		for (int i = 0; i < 20; i++)
			searchBox[i] = 0;
		sindex = 0;
		suggestions = nullptr;
		lastSuggestion = nullptr;
		selectSuggestion = nullptr;
		numSuggestions = 0;
	}
	void createPartitions() {
		int latestX = cursor.getX();
		int latestY = cursor.getY();
		for (int y = 0; y <= Y_BOUNDARY+1; y++) {
			cursor.moveCursorTo(X_BOUNDARY+1, y);
			cout << '|';
		}
		cursor.moveCursorTo(X_BOUNDARY + 2, 0);
		cout << "Search:";
		cout << searchBox;
		displaySearchResults(cursor);
		cursor.moveCursorTo(X_BOUNDARY + 3, 15);
		char* trieStr = new char[20];
		for (int i = 0; i < 20;i++)
			trieStr[i] = 0;
		cout << "N-Ary tree:\n";
		cursor.moveCursorDown();
		trie.display(trie.root, trieStr,-1,cursor);
		for (int x = X_BOUNDARY; x >= 0; x--) {
			cursor.moveCursorTo(x, Y_BOUNDARY+1);
			cout << '-';
		}
		cursor.moveCursorTo(0, Y_BOUNDARY+2);
		cout << "Suggestions:";
		cursor.moveCursorDown();
		displaySuggestions();
		cursor.moveCursorDown();
		chilliTree.displayTree(); //just displays the tree rn
		cursor.moveCursorTo(latestX, latestY);
	}
	void display() {
		this->list.display();
		createPartitions();
		//displaySearchResults();
	}
	Action* undo() { 
		Action* undoAction = undoStack.pop();
		if (undoAction) {
			if (undoAction->inserted) { //delete
				CharNode* deleteFrom = list.getCharNode(undoAction->x, undoAction->y);
				CharNode* currDel = undoAction->word;
				int delX = undoAction->x;
				int delY = undoAction->y;
				while (currDel) {
					list.deleteAt(delX, delY);
					currDel = currDel->nextNode;
				}
			}
			else { //insert
				int x = undoAction->x;
				int y = undoAction->y;
				if (list.getLine(y) == nullptr)
					list.insertLine(y);
				CharNode* currIns = undoAction->word;
				while (currIns) {
					CharNode* ins = list.insertAt(currIns->data, x, y);
					x++;
					currIns = currIns->nextNode;
				}
			}
			redoStack.push(undoAction);
		}
		return undoAction;
	}
	Action* redo() {
		Action* redoAction = redoStack.pop();
		if (redoAction) {
			if (redoAction->inserted) { //re-insert
				int x = redoAction->x;
				int y = redoAction->y;
				CharNode* currIns = redoAction->word;
				while (currIns) {
					list.insertAt(currIns->data, x, y);
					x++;
					currIns = currIns->nextNode;
				}
			}
			else { //re-delete
				CharNode* deleteFrom = list.getCharNode(redoAction->x, redoAction->y);
				CharNode* currDel = redoAction->word;
				int delX = redoAction->x;
				int delY = redoAction->y;
				while (currDel) {
					list.deleteAt(delX, delY);
					currDel = currDel->nextNode;
				}
			}
			undoStack.push(redoAction);
		}
		return redoAction;
	}
	void displaySearchResults(Cursor& cursor) {
		//for now this just displays the trie tree
		//trie.display(trie.root, "");
		if (trie.searchPtr) {
			for (int i = 0; i < trie.searchPtr->numInstances; i++) {
				cursor.moveCursorDown();
				cout << "line: " << trie.searchPtr->instances[i]->y << " ";
			}
		}
	}
	//suggestions
	void suggestWord(char* word) {
		TrieNode* node = trie.root;
		int index = 0;
		while (node && word[index] != 0) { //traverse trie with word
			node = node->child[word[index] - 'a'];
			index++;
		} 
		if (node == nullptr)
			return;
		char copy[20];
		for (int i = 0; i < 20; i++)
			copy[i] = word[i];
		for (int i = 0; i < 26; i++)
			trie.getSuggestions(node->child[i], copy, index, suggestions, lastSuggestion, numSuggestions);//get all 'paths' from node onwards
	}
	void suggestSentence(char* word) {
		chilliTree.getSuggestions(word, suggestions, lastSuggestion, numSuggestions);
		
	}
	void displaySuggestions() {
		Suggestion* currSuggestion = suggestions;
		while (currSuggestion) {
			if (currSuggestion->selected)
				cout << "\033[32m" << currSuggestion->suggestion << "\033[0m" << ", ";
			else
				cout << currSuggestion->suggestion << ", ";
			currSuggestion = currSuggestion->next;
		}
	}
	void clearSuggestions() {
		lastSuggestion = nullptr;
		while (suggestions) {
			Suggestion* delSug = suggestions;
			suggestions = suggestions->next;
			delete delSug;
		}
		selectSuggestion = nullptr;
	}

	void displayStartMenu(char* &fileName,bool &saveFile) {
		cout << R"(
                         :--:                
                       +*-..-*-    .:--:     
                      =+      =+-*+-:..-#-   
            -+******+-%        .@     :..@   
         .##=----:::-=@ .#%-    %    .#=:%   
        -%-:-----::-::*= :=    .@.     .%.   
       -%--:::::::::::-%=     .#*+**-.+#:    
      .%-:::::::::::::::=**++**-::::+%:      
      #=:::-::::::::::::::::::::::::-#+      
     :*::+#*+#:::::::#=#::--=++=+*##*-       
     ==-%:-*%:=@##***%*#+++%##---.           
  :=+###%+##*#*@#*:..:##===.=#%+*+: .::.     
%%%####%%@%*@#%*+++###%#%%#%%@*#%%%%#**##*=. 
=-::. #+=  -==.        =+-...+%##%*     -*%@%
      *+#    :+###*:          .--.          :
       #++  -@*=--*%-                        
       .#+*  -##*.++@                        
         +**=     ++@                        
          .+#%%#*%#*:                        
              ...                            
		)";
		cout << "\nWELCOME TO NOTEPAD :)\n";
		cout << "\t1. Open a file?\n"; //will create if doesnt exist
		cout << "\t2. Create a file?\n"; 
		int option;
		cout << "Enter option (1/2): ";
		cin >> option;
		cout << "Enter file name: ";
		cin >> fileName;
		char saveC;
		cout << "Do you wish to save changes? Enter 'y' or 'n': ";
		cin >> saveC;
		if (saveC == 'Y' || saveC == 'y')
			saveFile = true;
	}
};



#endif