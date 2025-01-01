//Menahil Ahmad 23i-0546 Assignment 2
#include "FileManager.h"
#include "Console.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
using namespace std;


FileManager fileManager;
bool saveFile = false;

bool WINAPI ConsoleHandler(DWORD event) {
	system("cls");
	if (event == CTRL_CLOSE_EVENT) {
		if (saveFile) {
			cout << "Saving....";
			Sleep(1000);
			fileManager.writeFile();
		}
		return true;  
	}
	return false;  
}

int main(int argc, char* argv[]) {

	HANDLE  rhnd = GetStdHandle(STD_INPUT_HANDLE); 
	HANDLE whnd = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	//fix window size
	short screenWidth = info.srWindow.Right - info.srWindow.Left + 1;
	short screenHeight = info.srWindow.Bottom - info.srWindow.Top + 1;
	COORD screenDim = { screenWidth,screenHeight };
	int status = SetConsoleScreenBufferSize(whnd, screenDim);

	Console console(info);
	DWORD Events = 0;    
	DWORD EventsRead = 0; 

	//open file
	char* fileName = new char[20]; 
	//console.displayStartMenu(fileName,saveFile);
	//fileManager.readFile(fileName, console.list,console.trie,console.chilliTree);
	//system("cls");
	//SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE); //handler for close window

	CharNode* lastDeletion = nullptr; //for undo redo delete

	bool Running = true;
	console.display();

	bool searching = false;
	bool suggesting = false;
	char word[20]; //to complete
 

	while (Running) {
		GetNumberOfConsoleInputEvents(rhnd, &Events);
		if (Events != 0) { 
			INPUT_RECORD eventBuffer[200];
			ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);
			for (DWORD i = 0; i < EventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
					switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
					case VK_UP: //up
						if (console.list.nodeExists(console.cursor.x, console.cursor.y - 1))
							console.cursor.moveCursorUp();
						else if (console.list.lineExists(console.cursor.y - 1)) //empty line or no char up
							console.cursor.moveCursorTo(0, console.cursor.y - 1);
						break;
					case VK_DOWN: //down
						if (console.list.nodeExists(console.cursor.x, console.cursor.y + 1))
							console.cursor.moveCursorDown();
						break;
					case VK_RIGHT: //right
						if (console.list.nodeExists(console.cursor.x, console.cursor.y) && console.list.getCharNode(console.cursor.x,console.cursor.y)->data!='\n')
							console.cursor.moveCursorRight();
						break;
					case VK_LEFT: //left
						if (console.list.nodeExists(console.cursor.x - 1, console.cursor.y))
							console.cursor.moveCursorLeft();
						break;
					case VK_RETURN: //enter
						if (suggesting) {
							//insert suggestion
							Suggestion* sug = console.selectSuggestion;
							int i = 0;
							while (compareChar(sug->suggestion[i],word[i]))  //this is only for word completion not sentence completion
								i++;
							while (i < sug->length) { //no need insert in trie already in trie
								console.list.insertAt(sug->suggestion[i], console.cursor.x, console.cursor.y);
								i++;
								console.cursor.x++;
							}
							for (int i = 0; i < 20; i++)
								word[i] = 0;
							console.clearSuggestions();
							suggesting = false;
						}
						else if (console.cursor.y < console.Y_BOUNDARY) {
							if (!(console.list.getCharNode(console.cursor.x,console.cursor.y)!= nullptr && console.list.getCharNode(console.cursor.x,console.cursor.y)->data=='\n')) //dont \n a \n
								console.list.insertAt('\n', console.cursor.x, console.cursor.y); //insert \n
							console.cursor.moveCursorRight();
							console.list.getLine(console.cursor.y)->shiftToLineBelow(console.list.getCharNode(console.cursor.x, console.cursor.y)); //add new line (with shifted chars if any) below
							console.cursor.moveCursorTo(0, console.cursor.y + 1);
						}
						break;
					case VK_BACK: //backspace
					{
						if (searching) {
							console.searchBox[console.sindex-1] = 0;
							console.sindex--;
							console.cursor.moveCursorLeft();
							console.lowerCaseSearchBox = lowercaseStr(console.searchBox);
							console.trie.backSearchPtr(console.lowerCaseSearchBox);
							delete[] console.lowerCaseSearchBox;
						}
						else {
							CharNode* dNode = console.list.getCharNode(console.cursor.x - 1, console.cursor.y);
							if (dNode && dNode->data != ' ') {
								CharNode* start = dNode;
								while (!(start->prevNode == nullptr || start->prevNode->data == ' '))
									start = start->prevNode;
								console.trie.deleteNode(dNode, start);
							}
							CharNode* deletedChar = console.list.deleteAt(console.cursor.x - 1, console.cursor.y);
							if (!deletedChar && console.cursor.x <= 0) {
								console.list.deleteLine(console.cursor.y);
								if (console.cursor.y > 0) {
									int aboveY = console.cursor.y - 1;
									console.list.deleteAt(console.list.getLine(aboveY)->tail->x, aboveY); //delete the \n if any on above line
									console.cursor.moveCursorTo(console.list.getLine(aboveY)->tail->x + 2, aboveY);
								}
							}
							console.cursor.moveCursorLeft();
							//add to lastDeletion
							if (deletedChar) {
								CharNode* lastDelChar = new CharNode(deletedChar->data);
								if (lastDeletion) {
									lastDelChar->nextNode = lastDeletion;
									lastDeletion = lastDelChar;
								}
								else
									lastDeletion = lastDelChar;
								if ((deletedChar->data == ' ' || deletedChar->x == 0) && lastDeletion->nextNode != nullptr) { //not single space
									Action* latest = new Action(deletedChar->x, deletedChar->y, false, lastDeletion);
									console.undoStack.push(latest);
									//clear lastDeletion
									clearList(lastDeletion);
								}
							}
							//if now the cursor is at a space that means i deleted a word
							if (deletedChar) {
								delete deletedChar;
							}
						}
						break;
					}
					case VK_F1: //undo 
					{
						Action* undoAction = console.undo();
						if (undoAction) {
							if (undoAction->inserted) //deleted
								console.cursor.moveCursorTo(undoAction->x, undoAction->y);
							else //inserted
								console.cursor.moveCursorTo(undoAction->x + lengthOfWord(undoAction->word->nextNode) + 1, undoAction->y);
						}
						break;
					}
					case VK_F2: //redo
					{
						Action* redoAction = console.redo();
						if (redoAction) {
							if (redoAction->inserted) //insert
								console.cursor.moveCursorTo(redoAction->x + lengthOfWord(redoAction->word->nextNode) + 1, redoAction->y);
							else //delete
								console.cursor.moveCursorTo(redoAction->x, redoAction->y);
						}
						break;
					}
					case VK_F3: //search
						if (!searching) {
							console.cursor.saveLastPosition();
							console.cursor.moveCursorTo(console.X_BOUNDARY + 9, 0);
							searching = true;
						}
						else {
							//move cursor back somhow
							//unhighlight
							console.lowerCaseSearchBox = lowercaseStr(console.searchBox);
							console.trie.unhighlightNodes(console.trie.root, console.lowerCaseSearchBox, 0);
							console.trie.resetSearchPtr();
							for (int i = 0; i < 20; i++)
								console.searchBox[i] = 0;
							console.sindex = 0;
							searching = false;
							console.cursor.restoreLastPosition();
							delete[] console.lowerCaseSearchBox;
						}
						break;
					case VK_F4: //word completion
						if (suggesting) {
							//loop through each suggestion and highlight
							if (console.suggestions == nullptr)
								break;
							if (console.selectSuggestion == nullptr){
								console.selectSuggestion = console.suggestions;
								console.selectSuggestion->selected = true;
							}
							else {
								console.selectSuggestion->selected = false;
								console.selectSuggestion = console.selectSuggestion->next;
								if (console.selectSuggestion == nullptr)
									console.selectSuggestion = console.suggestions; //head
								console.selectSuggestion->selected = true;
							}
						}
						break;
					default: //insertion
						char charKey = eventBuffer->Event.KeyEvent.uChar.AsciiChar;
						if (searching) { //searching
							if (charKey >= 'A' && charKey <= 'Z' || charKey >= 'a' && charKey <= 'z' || charKey == ' ') {
								console.searchBox[console.sindex++] = charKey;
								console.trie.search(charKey, console.searchBox);
								console.cursor.moveCursorRight();
							}
						}
						else { //text insertion
							if (charKey >= 'a' && charKey <= 'z' || charKey >= 'A' && charKey <= 'Z' || charKey == ' ') {
								if (!console.list.getHead()) //empty list
									console.list.insertLine(0);
								CharNode* insertedChar = console.list.insertAt(charKey, console.cursor.x, console.cursor.y);
								if (insertedChar) {
									console.cursor.moveCursorTo(insertedChar->x + 1, insertedChar->y);
									if (charKey != ' ') {
										CharNode* start = insertedChar;
										while (!(start->prevNode == nullptr || start->prevNode->data == ' '))
											start = start->prevNode;
										if (charKey >= 'a' && charKey <= 'z' || charKey >= 'A' && charKey <= 'Z') 
											console.trie.insert(insertedChar, start);
									}
									if (charKey == ' ' && insertedChar->prevNode) {
										Action* latest = new Action(1, insertedChar->prevNode);
										console.undoStack.push(latest);
										//chillitree
										char word[20];
										for (int i = 0; i < 20; i++)
											word[i] = 0;
										CharNode* tmp = console.list.getCharNode(console.cursor.x - 1, console.cursor.y);
										int index = 0;
										while (!(tmp->prevNode == nullptr || tmp->prevNode->data == ' ')) {
											tmp = tmp->prevNode;
										}
										while (tmp && tmp->data!=' ') {
											word[index++] = tmp->data;
											tmp = tmp->nextNode;
										}
										//word has the word to add to chillitree
										console.chilliTree.addNode(word);

										console.trie.lastInserted->wordEnd = true; //single char word needs this check
									}
								}
							}
							else if (charKey == '@' || charKey=='*') { //word completion or sentence completion
								//get word so far
								suggesting = true;
								for (int i = 0; i < 20; i++)
									word[i] = 0;
								CharNode* tmp = console.list.getCharNode(console.cursor.x - 1, console.cursor.y);
								int index = 0;
								while (!(tmp->prevNode == nullptr || tmp->prevNode->data == ' ')) {
									tmp = tmp->prevNode;
								}
								while (tmp) {
									word[index++] = tmp->data;
									tmp = tmp->nextNode;
								}
								char* lowerCaseWord = nullptr;
								if (charKey == '@') {
									lowerCaseWord = lowercaseStr(word);
									console.suggestWord(lowerCaseWord);
								}
								else {
									word[--index] = 0; //remove space
									lowerCaseWord = lowercaseStr(word);
									console.suggestSentence(lowerCaseWord);
									for (int i = 0; i < 20; i++)
										word[i] = 0;
								}
								if (lowerCaseWord)
									delete[] lowerCaseWord;
							}
						}
						break;
					}
				}
			}
			system("cls");
			console.display();
			console.cursor.updateCursor();
		}
	} // end program loop

	delete[] fileName;

	return 0;
}
