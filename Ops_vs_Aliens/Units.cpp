#include "Units.h"
#include "Session.h"


using namespace Gamma;

Backpack::Backpack(int size) : weight(0), num(0), Items(size, nullptr) {}

Backpack& Backpack::operator=(Backpack&& bp) noexcept
{
	weight = bp.weight;
	num = bp.num;
	std::swap(Items, bp.Items);
	return *this;
}

Backpack::Backpack(std::ifstream& file)
{
	int size;
	file >> size;
	*this = Backpack(size);
	std::string item_type;
	for (int i = 0; i < size; i++) {
		file >> item_type;
		if (item_type == "weapon")
			put_item(new weapon(file));
		else if (item_type == "aidkit")
			put_item(new aidkit(file));
		else if (item_type == "ammunition")
			put_item(new ammunition(file));
	}
}

std::ofstream& Backpack::save(std::ofstream& ofile) const
{
	ofile << Items.size();
	for (size_t i = 0; i < Items.size(); i++)
		if (Items[i]) {
			ofile << ' '; ofile << *(Items[i]);
		}
		else
			ofile << " -";
	return ofile;
}

Backpack::~Backpack()
{
	for (size_t i = 0; i < Items.size(); i++)
		delete Items[i];
}

bool Backpack::put_item(Item* item)
{
	if (num < (int)Items.size()) {
		for (My_vect::My_vectorIt<Item*> i = Items.begin(); i != Items.end(); i++)
			if (*i == nullptr) {
				*i = item;
				weight += item->w(); num++;
				break;
			}
		return true;
	}
	return false;
}

Item* Backpack::drop_item(int n)
{
	Item* item = Items[n];
	if (item != nullptr) {
		num--;
		weight -= item->w();
		Items[n] = nullptr;
	}
	return item;
}

inline Item* Backpack::operator[](int n) const
{
	return Items[n];
}


Unit::Unit(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move) :
	name(n), model('0'), position(x, y), cur_HP(cur_hp), full_HP(hp), cur_MP(cur_mp), full_MP(mp), vision_(r), move_points(move) {}

Unit::Unit(std::ifstream& file) { file >> name >> model >> position.x >> position.y >> cur_HP >> full_HP >> cur_MP >> full_MP >> vision_ >> move_points; }

std::ofstream& Unit::save(std::ofstream& ofile) const
{
	ofile << name << ' ' << model << ' ' << position.x << ' ' << position.y
		<< ' ' << cur_HP << ' ' << full_HP << ' ' << cur_MP << ' ' << full_MP << ' ' << vision_ << ' ' << move_points << ' ';
	return ofile;
}

std::ostream& Unit::display(std::ostream& os) const
{
	return os << "HP: " << cur_HP << '/' << full_HP << ", MP: " << cur_MP << '/' << full_MP << ", Points per 1 move: " << move_points << '\n';
}

bool Unit::move(const Point& p)
{
	if (cur_MP >= move_points)
	{
		cur_MP -= move_points;
		position = p;
		return true;
	}
	return false;
}

int Unit::change_HP(int HP)
{
	int n = (HP > full_HP - cur_HP) ? full_HP - cur_HP : HP;
	cur_HP += n;
	return n;
}

inline void Unit::change_MP(int use_points) { cur_MP -= use_points; }

inline bool Unit::check_MP(int MP) const { return cur_MP >= MP; }


Attack_Unit::Attack_Unit(std::ifstream& file)
{
	std::string tmp;
	file >> accuracy >> tmp;
	if (tmp != "-")
		gun = new weapon(file);
	else
		gun = nullptr;
}

std::ofstream& Attack_Unit::save(std::ofstream& ofile) const
{
	ofile << accuracy;
	if (gun) {
		ofile << ' '; ofile << *gun;
	}
	else
		ofile << " -";
	return ofile;
}

std::ostream& Attack_Unit::display(std::ostream& os) const
{
	os << "In hands: "; 
	if (gun)
		os << *gun;
	else
		os << "nothing";
	return os << ", accuracy - " << accuracy << '\n';
}

const std::string* Attack_Unit::attack(Unit* unit)
{
	if (gun) {
		if (check_MP(gun->up())) {
			double n;
			if (n = gun->deal_damage()) {
				change_MP(gun->up());
				unit->change_HP((int)-(accuracy * n));
				return msgs;
			}
			return msgs + 1;
		}
		return msgs + 2;
	}
	return msgs + 17;
}

const std::string* Attack_Unit::attack(Cell& cell)
{
	if (gun) {
		if (check_MP(gun->up())) {
			if (gun->deal_damage()) {
				change_MP(gun->up());
				cell.cell_type = '.';
				return msgs;
			}
			return msgs + 1;
		}
		return msgs + 2;
	}
	return msgs + 17;
}


Take_Unit::Take_Unit(std::ifstream& file)
{
	file >> full_weight;
	backpack = Backpack(file);
}

std::ofstream& Take_Unit::save(std::ofstream& ofile) const
{
	ofile << full_weight << ' ';
	ofile << backpack;
	return ofile;
}

std::ostream& Take_Unit::display(std::ostream& os) const
{
	return os << "Weight: " << backpack.w() << '/' << full_weight << '\n';
}

My_vector<Item*>* Take_Unit::get_items() {
	return backpack.get_items();
}

Item* Take_Unit::drop_item(int bind) {
	return backpack.drop_item(bind);
}

bool Take_Unit::take_item(Item* item)
{
	if ((full_weight >= (backpack.w() + item->w())) && (backpack.not_full()))
		return backpack.put_item(item);
	return false;
}


Operative::Operative(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, weapon* g, double ac, int f_w, Backpack&& bp) :
	Unit(n, x, y, cur_hp, hp, cur_mp, mp, r, move), Attack_Unit(ac, g), Take_Unit(f_w, std::move(bp)) {}

std::ofstream& Operative::save(std::ofstream& ofile) const
{
	ofile << "ally Operative ";
	Unit::save(ofile);
	Attack_Unit::save(ofile);
	ofile << ' ';
	Take_Unit::save(ofile);
	ofile << '\n';
	return ofile;
}

std::ostream& Operative::display(std::ostream& os) const
{
	os << "Operative, ";
	Unit::display(os);
	Take_Unit::display(os);
	return Attack_Unit::display(os);
}

const std::string* Operative::use_item(int bind)
{
	Item* item = backpack[bind];
	if (item) {
		return item->use(this);
	}
	return msgs + 7;
}

int Operative::load_ammo(int cur_ammo, double ammo_type)
{
	return gun->reload(cur_ammo, ammo_type);
}

const std::string* Operative::change_weapon(weapon* gun2)
{
	std::swap(*gun, *gun2);
	return msgs;
}


Alien_melee::Alien_melee(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, weapon* g, double ac) :
	Unit(n, x, y, cur_hp, hp, cur_mp, mp, r, move), Attack_Unit(ac, g) {}

std::ofstream& Alien_melee::save(std::ofstream& ofile) const
{
	ofile << "enemy Alien_melee ";
	Unit::save(ofile);
	Attack_Unit::save(ofile);
	ofile << '\n';
	return ofile;
}

std::ostream& Alien_melee::display(std::ostream& os) const
{
	os << "Alien_melee, ";
	Unit::display(os);
	return Attack_Unit::display(os) << '\n';
}


Alien_range::Alien_range(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, weapon* g, double ac) :
	Unit(n, x, y, cur_hp, hp, cur_mp, mp, r, move), Attack_Unit(ac, g) {}

std::ofstream& Alien_range::save(std::ofstream& ofile) const
{
	ofile << "enemy Alien_range ";
	Unit::save(ofile);
	Attack_Unit::save(ofile);
	ofile << '\n';
	return ofile;
}

std::ostream& Alien_range::display(std::ostream& os) const
{
	os << "Alien_range, ";
	Unit::display(os);
	return Attack_Unit::display(os) << '\n';
}

weapon* Alien_range::swap_gun(weapon* g)
{
	weapon* tmp = gun;
	gun = g;
	return tmp;
}

Alien_friendly::Alien_friendly(std::string n, int x, int y, int cur_hp, int hp, int cur_mp, int mp, int r, int move, int f_w, Backpack&& bp) :
	Unit(n, x, y, cur_hp, hp, cur_mp, mp, r, move), Take_Unit(f_w, std::move(bp)) {}

std::ofstream& Alien_friendly::save(std::ofstream& ofile) const
{
	ofile << "enemy Alien_friendly ";
	Unit::save(ofile);
	Take_Unit::save(ofile);
	ofile << '\n';
	return ofile;
}

std::ostream& Alien_friendly::display(std::ostream& os) const
{
	os << "Alien_friendly, ";
	Unit::display(os);
	return Take_Unit::display(os) << '\n';
}