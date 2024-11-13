#ifndef SUGGESTION_H
#define SUGGESTION_H

struct Suggestion {
	char* suggestion;
	int length;
	bool selected;
	Suggestion* next;
	Suggestion() :suggestion(nullptr), selected(false),length(0),next(nullptr) {}
	Suggestion(char* suggestion,int size) :selected(false),next(nullptr) {
		this->suggestion = new char[size+1];
		for (int i = 0; i < size; i++)
			this->suggestion[i] = suggestion[i];
		this->suggestion[size] = 0;
		this->length = size;
	}
	Suggestion(char* suggestion) :selected(false), next(nullptr) {
		int size = 0;
		while (suggestion[size] != 0)
			size++;
		this->suggestion = new char[size + 1];
		for (int i = 0; i < size; i++)
			this->suggestion[i] = suggestion[i];
		this->suggestion[size] = 0;
		this->length = size;
	}
	//void setSuggestion(char* suggestion) {
	//	int i = 0;
	//	while (suggestion[i] != 0) {
	//		char* tmp = new char[i + 1];
	//		for (int j = 0; j < i; j++)
	//			tmp[j] = suggestion[i];
	//		tmp[i] = suggestion[i];
	//		delete[] suggestion;
	//		suggestion = tmp;
	//		i++;
	//	}
	//}
	void operator=(Suggestion& copy) {
		char* tmp = new char[copy.length];
		for (int i = 0; i < copy.length; i++)
			tmp[i] = suggestion[i];
		delete[] this->suggestion;
		this->suggestion = tmp;
		this->length = copy.length;
	}
	~Suggestion() {
		delete[] this->suggestion;
	}
};

#endif