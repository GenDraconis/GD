#include "Digimon.h"

namespace _VEEBOT_ {
	Digimon::Digimon(const std::vector<std::string>& digiInfo)
	{
		name = digiInfo[0];
		HP = std::stoi(digiInfo[1]);
		maxHP = std::stoi(digiInfo[2]);
		MP = std::stoi(digiInfo[3]);
		maxMP = std::stoi(digiInfo[4]);
		str = std::stoi(digiInfo[5]);
		def = std::stoi(digiInfo[6]);
		speed = std::stoi(digiInfo[7]);
		rank = std::stoi(digiInfo[8]);
		dvPoints = std::stoi(digiInfo[9]);
		special = digiInfo[10];
		type = digiInfo[11];
		role = "TEST";
		picLoc = name + ".png";
		critChance = 1 * (1 - exp(-(0.002 / rank * rank * 0.75)));
		//role = digiInfo[12];
	}
	Digimon::Digimon(const Digimon& other)
	{
		name = other.name;
		HP = other.HP;
		maxHP = other.maxHP;
		MP = other.MP;
		maxMP = other.maxMP;
		str = other.str;
		def = other.def;
		speed = other.speed;
		rank = other.rank;
		dvPoints = other.dvPoints;
		special = other.special;
		type = other.type;
		role = other.role;
		picLoc = other.picLoc;
	}
}