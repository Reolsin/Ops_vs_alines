#pragma once
#include "Declarations.h"
#include "Units.h"
#include "Items.h"
#include <iostream>


namespace Gamma {

	struct Cell {
		My_vector<Item*> Items;
		char cell_type;

		Cell() : cell_type('.') {}
		~Cell();

		bool add_item(Item*);
	};

	class GameLevel {
	private:
		My_vector<My_vector<Cell>> Map;
	protected:
		std::ofstream& save(std::ofstream&) const;
	public:
		GameLevel(int l_ = 100, int h_ = 100);
		GameLevel(std::ifstream&);

		My_vector<Item*>* get_items(const Point& p);
		bool add_item(Item*, int, int);
		Cell& operator[](const Point&);
		const Cell& operator()(const Point&) const;

		bool point_access(int, int) const;
		bool point_on_map(double, double) const;
		bool is_transparent(double, double) const;

		friend std::ofstream& operator<<(std::ofstream& ofile, const GameLevel& level) { return level.save(ofile); }
	};

	enum class mode { game, inventory, attack, take, loot };
	struct operation_code {
		int code;
		mode m;
	};
	struct CompareType {
		using is_transparent = std::true_type;
		bool operator()(const operation_code& lhs, const operation_code& rhs) const
		{
			return (lhs.code < rhs.code) || ((lhs.code == rhs.code) && (lhs.m < rhs.m));
		}
	};

	class GameSession {
	private:
		static std::map<operation_code, bool (Gamma::GameSession::*)(), CompareType> operation;
		int con_len;
		const std::string* message;

		GameLevel level;

		Unit* current_unit;
		int current_unit_num;
		Point attack_cursor;
		My_vector<My_vector<char>> unit_vision;
		My_vector<Item*>* displayed_items;
		My_vector<Item**> lootmode_items;
		size_t disp_items_num;

		My_vector<Unit*> player;
		My_vector<Unit*> enemy;

		mode current_mode;

		void move(const Point&);
		bool move_up();
		bool move_down();
		bool move_right();
		bool move_left();
		bool choose_next();
		bool choose_previous();

		bool inventory_init();
		bool inventory_drop();
		bool inventory_activate();
		bool take_init();
		bool take_activate();
		bool loot_init();
		bool loot_activate();
		bool table_choose_up();
		bool table_choose_down();
		bool loot_choose_up();
		bool loot_choose_down();

		void attack_cursor_move(const Point&);
		bool attack_init();
		bool attack_activate();
		bool attack_cursor_up();
		bool attack_cursor_down();
		bool attack_cursor_right();
		bool attack_cursor_left();

		bool close();
		bool next_turn();
		bool exit();

	public:
		GameSession() : con_len(160), message(close_mods), current_unit_num(0), current_unit(nullptr), disp_items_num(0), displayed_items(nullptr), attack_cursor(), current_mode(mode::game), level() {}
		GameSession(std::ifstream&);
		~GameSession();

		Unit* alive_unit_here(const Point&) const;
		My_vector<Unit*> dead_units_here(const Point& p) const;
		bool current_unit_is_alive() const { return current_unit->is_alive(); }
		My_vector<My_vector<char>> render_vision(const Unit* unit) const;

		bool execute(int button)
		{
			if (operation.count({ button, current_mode }) == 1)
				return (this->*operation.at({ button, current_mode }))();
			return true;
		}
		bool check() const;

		void draw() const;

		bool save(std::ofstream&);
	};
}
