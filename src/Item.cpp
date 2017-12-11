#include "Item.h"
#include "Player.h"

namespace _VEEBOT_ {
	Item::Item(std::string _name)
	{
		name = _name;
		numOf = 1;
	}
	void Item::use(Player& P)
	{
		if (name == "Off. Chip")
		{
			P.getDigimon().setStr(static_cast<int>(P.getDigimon().getStr() + P.getDigimon().getStr() * 0.025));
		}
		else if (name == "Quick. Chip")
		{
			P.getDigimon().setStr(static_cast<int>(P.getDigimon().getSpd() + P.getDigimon().getSpd() * 0.02));
		}
		else if (name == "Digi-Egg of Miracles")
		{

		}
		else if (name == "Jogress Chip Pt. 1/4")
		{

		}
		else if (name == "Jogress Chip Pt. 2/4")
		{

		}
		else if (name == "Jogress Part (3/4")
		{

		}
		else if (name == "Jogress Part(4/4)")
		{

		}
		else if (name == "Digivice")
		{

		}
		if (name != "Digivice")
		{
			numOf--;
		}
	}
}