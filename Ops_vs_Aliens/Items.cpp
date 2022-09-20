#include "Items.h"
#include "Units.h"

using namespace Gamma;

Item::Item(std::string n, int w, int up) :
	name(n), weight(w), use_points(up) {}

Item::Item(std::ifstream& file) { file >> name >> weight >> use_points; }

std::ofstream& Item::save(std::ofstream& ofile) const
{
	ofile << name << ' ' << weight << ' ' << use_points << ' ';
	return ofile;
}

std::ostream& Item::display(std::ostream& os) const
{
	return os << "Item: " << name << ", weight: " << weight << ", use points: " << use_points << ' ';
}


weapon::weapon(std::string n, int w, int up, int f_clip_ammo, int f_ammo, int d, int rp, double a_type) :
	Item(n, w, up), cur_clip_ammo(f_clip_ammo), full_clip_ammo(f_clip_ammo), damage(d), reload_points(rp), ammo_type(a_type) {}

weapon::weapon(std::ifstream& file) : Item(file) { file >> cur_clip_ammo >> full_clip_ammo >> damage >> reload_points >> ammo_type; }

std::ofstream& weapon::save(std::ofstream& ofile) const
{
	ofile << "weapon ";
	Item::save(ofile);
	ofile << cur_clip_ammo << ' ' << full_clip_ammo << ' ' << damage << ' ' << reload_points << ' ' << ammo_type;
	return ofile;
}

double weapon::deal_damage()
{
	if (cur_clip_ammo) {
		cur_clip_ammo--;
		return damage * ammo_type;
	}
	return 0;
}

int weapon::reload(int ammo, double type)
{
	int n = 0;
	if (ammo_type == type) {
		n = (ammo > full_clip_ammo - cur_clip_ammo) ? full_clip_ammo - cur_clip_ammo : ammo;
		cur_clip_ammo += n;
	}
	return n;
}

const std::string* weapon::use(Operative* unit)
{
	return unit->change_weapon(this);
}

std::ostream& weapon::display(std::ostream& os) const
{
	Item::display(os);
	return os << "type: weapon, clip: " << cur_clip_ammo << "/" << full_clip_ammo << ", damage: " << damage << ", rel. points: " << reload_points << ", ammo type: " << ammo_type;
}


aidkit::aidkit(std::string n, int w, int up, int cur, int ful, int heal) :
	Item(n, w, up), heal_amount(heal), charges(cur), full_charges(ful) {}

aidkit::aidkit(std::ifstream& file) : Item(file) { file >> charges >> full_charges >> heal_amount; }

std::ofstream& aidkit::save(std::ofstream& ofile) const
{
	ofile << "aidkit ";
	Item::save(ofile);
	ofile << charges << ' ' << full_charges << ' ' << heal_amount;
	return ofile;
}

const std::string* aidkit::use(Operative* unit)
{
	if (charges) {
		if (unit->check_MP(up())) {
			if (unit->change_HP(heal_amount)) {
				unit->change_MP(up());
				charges--;
				return msgs;
			}
			return msgs + 3;
		}
		return msgs + 2;
	}
	return msgs + 4;
}

std::ostream& aidkit::display(std::ostream& os) const
{
	Item::display(os);
	return os << "type: aidkit, charges:" << charges << "/" << full_charges << ", heal:" << heal_amount;
}


ammunition::ammunition(std::string n, int w, int up, int cur, int ful, double a_type) :
	Item(n, w, up), cur_count(cur), full_count(ful), ammo_type(a_type) {}

ammunition::ammunition(std::ifstream& file) : Item(file) { file >> cur_count >> full_count >> ammo_type; }

std::ofstream& ammunition::save(std::ofstream& ofile) const
{
	ofile << "ammunition ";
	Item::save(ofile);
	ofile << cur_count << ' ' << full_count << ' ' << ammo_type;
	return ofile;
}

const std::string* ammunition::use(Operative* unit)
{
	if (cur_count) {
		if (unit->check_MP(up())) {
			int n;
			if (n = unit->load_ammo(cur_count, ammo_type)) {
				unit->change_MP(up());
				cur_count -= n;
				return msgs;
			}
			return msgs + 5;
		}
		return msgs + 2;
	}
	return msgs + 6;
}

std::ostream& ammunition::display(std::ostream& os) const
{
	Item::display(os);
	return os << "type: ammunition, ammo: " << cur_count << "/" << full_count << ", ammo type: " << ammo_type;
}