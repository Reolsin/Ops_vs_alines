#include "../Ops_vs_Aliens/Session.h"
#include "../Ops_vs_Aliens/Menu.cpp"
#include "windows.h"

using namespace Gamma;

void OpenGameField(Gamma::GameSession& Session)
{
	int button(0);
	system("cls");
	while (Session.execute(button) && Session.check())
	{
		Session.draw();
		button = _getch();
		if (button == 224)
			button = _getch();
		Sleep(1000 / 60);
	}
}

std::string get_save_name()
{
	My_vector<std::string> saves;
	std::ifstream s("saves.txt");
	int n;
	s >> n; s.ignore(1);
	saves.resize(n);
	for (int i = 0; i < n; i++)
		getline(s, saves[i]);
	saves.push_back("Back");
	Menu load_menu(saves);

	return saves[load_menu.get_recall()];
}

const std::vector<std::string> startmenu_msgs{ "New game", "Load game", "Quit" };
const std::vector<std::string> pausemenu_msgs{ "Continue", "Save game", "Leave w/o saving" };
const My_vector<std::string> startmenu_msgs(startmenu_msgs);
const My_vector<std::string> pausemenu_msgs(pausemenu_msgs);

int main()
{
	int rc1, rc2;
	Menu start_menu(startmenu_msgs);
	Menu pause_menu(pausemenu_msgs);
	std::ifstream config_file;
	do {
		rc1 = start_menu.get_recall();
		if (rc1 == 0)
			config_file.open("default.txt");
		else if (rc1 == 1) {
			std::string save_name = get_save_name();
			if (save_name != "Back")
				config_file.open(save_name);
			else
				continue;
		}
		if (!start_menu.is_last(rc1)) {
			Gamma::GameSession Session(config_file);
			do {
				OpenGameField(Session);
				rc2 = pause_menu.get_recall();
				if (rc2 == 1) {
					std::ofstream save_file("save.txt");
					Session.save(save_file);
				}
			} while (!pause_menu.is_last(rc2));
		}
	} while (!start_menu.is_last(rc1));

	return 0;
}