#pragma once

#include <string>
#include <vector>

namespace _VEEBOT_
{
	class Digimon{
	private:
		std::string name;
		int HP;
		int MP;
		int rank; //1Rookie; 2Champ; 3Ult; 4Mega
		int maxHP;
		int maxMP;
		int speed;
		int str;
		int def;
		int dvPoints;
		double critChance;
		std::string special; //Name of special ability -- pretty much cosmetic
		std::string type; //Type: Vaccine, Data, Virus
		std::string role; //DMG, UTIL, BAL, TANK
		std::string picLoc;
	public:
		Digimon(const std::vector<std::string>& digiInfo);
		Digimon(const Digimon& other);
		
		std::string getName() { return name; }
		int getHP() { return HP; }
		int getMP() { return MP; }
		int getRank() { return rank; }
		int getMaxHP() { return maxHP; }
		int getMaxMP() { return maxMP; }
		int getStr() { return str; }
		int getDef() { return def; }
		int getSpd() { return speed; }
		double getCritChance() { return critChance; }
		int getDVPoints() { return dvPoints; }
		std::string getSpecial() { return special; }
		std::string getType() { return type; }
		std::string getRole() { return role; }
		std::string getPic() { return picLoc; }

		void setName(std::string s) { name = s; picLoc = name + ".png"; }
		void setHP(int v) { HP = v; }
		void setMaxHP(int v) { maxHP = v; }
		void setMP(int v) { MP = v; }
		void setMaxMP(int v) { maxMP = v; }
		void setStr(int v) { str = v; }
		void setDef(int v) { def = v; }
		void setSpeed(int v) { speed = v; }
		void setSpecial(std::string s) { special = s; }
		void setRank(int v) { rank = v; }
		void setDVPoints(int v) { dvPoints = v; }
		void setCritChance(double c) { critChance = c; }

		void incHP(int v) { HP += v; }
		void incMaxHP(int v) { maxHP += v; }
		void incMaxMP(int v) { maxMP += v; }
		void incSpd(int v) { speed += v; }
		void incMP(int v) { MP += v; }
		void incStr(int v) { str += v; }
		void incDef(int v) { str += v; }
		void incDVPoints(int v) { dvPoints += v; }
	};
}
