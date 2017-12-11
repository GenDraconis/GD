#pragma once

#include <string>



namespace _VEEBOT_
{
	class Player;
	class Item
	{
	private:
		std::string name;
		int numOf;
	public:
		Item(std::string _name);
		void use(Player& P);
		std::string& getName()  { return name; }
		int getNumOf() { return numOf; }
	};
}
