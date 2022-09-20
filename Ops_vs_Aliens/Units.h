#pragma once
#include "Declarations.h"
#include "Items.h"


namespace Gamma {

	struct Point {
		int x, y;
		Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}

		Point& operator+=(const Point& p) { x += p.x; y += p.y; return *this; }
		bool operator==(const Point& p) const { return (x == p.x) && (y == p.y); }
	};

	class Backpack {
	private:
		int weight;
		int num;
		My_vector<Item*> Items;
	protected:
		std::ofstream& save(std::ofstream&) const;
	public:
		Backpack(int size = 4);
		Backpack(std::ifstream&);
		~Backpack();
		Backpack(Backpack&& bp) noexcept : weight(bp.weight), num(bp.num) { Items = std::move(bp.Items); }
		Backpack& operator=(Backpack&&) noexcept;

		inline My_vector<Item*>* get_items() { return &Items; }
		inline int w() const { return weight; }
		inline bool not_full() const { return num < (int)Items.size(); };
		bool put_item(Item*);
		Item* drop_item(int);
		inline Item* operator[](int) const;

		friend std::ofstream& operator<<(std::ofstream& ofile, const Backpack& bp) { return bp.save(ofile); }
	};

	class Unit {
	private:
		std::string name;
		char model;
		int cur_HP, cur_MP;
		int full_HP, full_MP,
			vision_,
			move_points;
		Point position;
	protected:
		Unit() : name("noname"), model('0'), position(), cur_HP(), full_HP(), cur_MP(), full_MP(), vision_(), move_points() {}
		Unit(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move);
		Unit(std::ifstream&);

		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		virtual ~Unit() {}
		bool move(const Point&);
		inline Point pos() const { return position; }
		inline int vision() const { return vision_; }

		inline void refresh() { cur_MP = full_MP; }
		int change_HP(int);
		void change_MP(int use_points);
		bool check_MP(int MP) const;
		inline bool is_alive() const { return cur_HP > 0; }
		inline std::string name_() const { return name; }
		inline char avatar() const { return model; }

		friend std::ostream& operator<<(std::ostream& ofile, const Unit& unit) { return unit.display(ofile); }
		friend std::ofstream& operator<<(std::ofstream& ofile, const Unit& unit) { return unit.save(ofile); }
	};

	class Attack_Unit : virtual public Unit {
	protected:
		double accuracy;
		weapon* gun;

		Attack_Unit() : accuracy(), gun(nullptr) {}
		Attack_Unit(double ac, weapon* g) : accuracy(ac), gun(g) {}
		Attack_Unit(std::ifstream&);
		virtual ~Attack_Unit() { delete gun; }

		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		const std::string* attack(Unit*);
		const std::string* attack(Cell&);
		weapon** get_gun() { return &gun; }
	};

	class Take_Unit : virtual public Unit {
	protected:
		int full_weight;
		Backpack backpack;

		Take_Unit() : full_weight() {}
		Take_Unit(int f_w, Backpack&& bp) : full_weight(), backpack(std::move(bp)) {}
		Take_Unit(std::ifstream&);
		virtual ~Take_Unit() {}

		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		Item* drop_item(int);
		bool take_item(Item*);
		My_vector<Item*>* get_items();
	};

	class Operative : public Attack_Unit, public Take_Unit {
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		Operative() : Unit(), Attack_Unit(), Take_Unit() {}
		Operative(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, weapon*, double, int, Backpack&&);
		Operative(std::ifstream& file) : Unit(file), Attack_Unit(file), Take_Unit(file) {}
		virtual ~Operative() {}

		const std::string* use_item(int);
		int load_ammo(int, double);
		const std::string* change_weapon(weapon*);
	};

	class Alien_melee : public Attack_Unit {
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		Alien_melee() : Unit(), Attack_Unit() {}
		Alien_melee(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, weapon* g, double ac);
		Alien_melee(std::ifstream& file) : Unit(file), Attack_Unit(file) {}
		virtual ~Alien_melee() {}
	};

	class Alien_range : public Attack_Unit {
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		Alien_range() : Unit(), Attack_Unit() {}
		Alien_range(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, weapon*, double ac);
		Alien_range(std::ifstream& file) : Unit(file), Attack_Unit(file) {}
		virtual ~Alien_range() {}

		weapon* swap_gun(weapon*);
	};

	class Alien_friendly : public Take_Unit {
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		Alien_friendly() : Unit(), Take_Unit() {}
		Alien_friendly(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, int full_weight, Backpack&&);
		Alien_friendly(std::ifstream& file) : Unit(file), Take_Unit(file) {}
		virtual ~Alien_friendly() {}
	};
}