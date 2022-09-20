#pragma once
#include "Declarations.h"


namespace Gamma {

	class Item {
	private:
		std::string name;
		int weight, use_points;
	protected:
		Item() : name("noname"), weight(), use_points() {}
		Item(std::string, int, int);
		Item(std::ifstream&);

		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		inline int up() const { return use_points; }
		inline int w() const { return weight; }

		virtual const std::string* use(Operative*) = 0;

		friend std::ofstream& operator<<(std::ofstream& ofile, const Item& item) { return item.save(ofile); }
		friend std::ostream& operator<<(std::ostream& os, const Item& item) { return item.display(os); }
	};


	class weapon : public Item {
	private:
		int cur_clip_ammo,
			full_clip_ammo,
			damage, reload_points;
		double ammo_type;
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		weapon() : Item(), cur_clip_ammo(), full_clip_ammo(), damage(), reload_points(), ammo_type() {}
		weapon(std::string n, int w, int up, int cur, int full, int d, int rp, double a_type);
		weapon(std::ifstream&);

		double deal_damage();
		int reload(int, double);

		virtual const std::string* use(Operative*);
	};


	class aidkit : public Item {
	private:
		int heal_amount, charges, full_charges;
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		aidkit() : Item(), heal_amount(), charges(), full_charges() {}
		aidkit(std::string n, int w, int up, int cur, int ful, int heal);
		aidkit(std::ifstream&);

		virtual const std::string* use(Operative*);
	};


	class ammunition : public Item {
	private:
		int cur_count, full_count;
		double ammo_type;
	protected:
		virtual std::ostream& display(std::ostream&) const;
		virtual std::ofstream& save(std::ofstream&) const;
	public:
		ammunition() : Item(), cur_count(), full_count(), ammo_type() {}
		ammunition(std::string n, int w, int up, int cur, int ful, double a_type);
		ammunition(std::ifstream&);

		virtual const std::string* use(Operative*);
	};
}
