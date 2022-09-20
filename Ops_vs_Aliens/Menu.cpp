#include <iostream>
#include "../my_vector/My_vector.h"
#include "conio.h"

using My_vect::My_vector;

class Menu {
	std::string pointer;
	My_vector<std::string> messages;
	void show(int pos)
	{
		for (int i = 0, n = (60 - messages.size()) / 2; i < n; i++)
			std::cout << std::endl;

		for (size_t i = 0; i < messages.size(); i++) {
			std::cout << messages[i];
			if (pos == i)
				std::cout << pointer;
			std::cout << std::endl;
		}
	}

public:
	Menu(My_vector<std::string> msgs, std::string p = "<") : \
		messages(msgs), pointer(p) {}

	int get_recall()
	{
		int rc = 0;
		int switcher;
		system("cls");
		do {
			show(rc);
			switcher = _getch();
			if (switcher == 224)
				switcher = _getch();
			switch (switcher) {
			case 72:
				rc = (messages.size() + rc - 1) % messages.size();
				break;
			case 80:
				rc = (messages.size() + rc + 1) % messages.size();
				break;
			}
			system("cls");
		} while (switcher != 13);

		return rc;
	}

	inline bool is_last(int n) { return n == messages.size() - 1; }
};