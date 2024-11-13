#ifndef CURSOR_H
#define CURSOR_H

#include <Windows.h>
using namespace std;

struct Cursor {
	int x, y;
	int lastX, lastY;
	Cursor() :x(0), y(0) {}
	void moveCursorUp() {
		if (this->y > 0) {
			this->y--;
			updateCursor();
		}
	}
	void moveCursorDown() {
			this->y++;
			updateCursor();
	}
	void moveCursorLeft() {
		if (this->x > 0) {
			this->x--;
			updateCursor();
		}
	}
	void moveCursorRight() {
			this->x++;
			updateCursor();
	}
	int getX() {
		return this->x;
	}
	int getY() {
		return this->y;
	}
	void moveCursorTo(int x, int y) {
		if (x >= 0) {
			this-> x = x;
			this->y = y;
			updateCursor();
		}
	}
	void updateCursor() {
		COORD c = { this->x, this->y };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	}
	void saveLastPosition() {
		lastX = x;
		lastY = y;
	}
	void restoreLastPosition() {
		x = lastX;
		y = lastY;
		moveCursorTo(x, y);
	}
};


#endif
