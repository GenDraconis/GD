#include "Player.h"

namespace _VEEBOT_ {
	////Player(Discord.User& usr)
	Player::Player(std::vector<std::string> playerInfo, std::vector<std::string> digimonInfo) :
		uniqueDiscordID(playerInfo[2])
	{

		bits = 0;
		triviaPoints = 0;
		name = "Unknown";
		discordName = "Unknown";
		name = playerInfo[0];
		discordName = playerInfo[1];
		bits = std::stoi(playerInfo[3]);
		triviaPoints = std::stoi(playerInfo[4]);

		digimon = std::make_unique<Digimon>(digimonInfo);


		////////std::string oppoDiscordName = getDiscordName();
		discordNameNoDisc = "";
		disc = 0;
		/*8888888888888for (int i = 0; i < oppoDiscordName.Length; i++)
		{
		if (i == oppoDiscordName.Length - 1) //Cant find #
		{
		continue;
		}
		if (oppoDiscordName[i] == '#')
		{
		std::string temp = oppoDiscordName.Substring(i + 1, oppoDiscordName.Length - discordNameNoDisc.Length - 1);
		disc = std::stoi(temp);
		//findUsers.Add(name, disc);
		break;
		}
		discordNameNoDisc += oppoDiscordName[i];
		}
		*/
	}
	Player::Player(SleepyDiscord::Snowflake<SleepyDiscord::User>& usr)
	{
		////	std::string _discordName = usr.ToString();
		std::string _discordName = usr;
		bits = 0;
		triviaPoints = 0;
		discordName = _discordName;
		//????fix?
		for (unsigned int i = 0; i < discordName.size(); i++)
		{
			char c = discordName[i];
			if (c == '#')
			{
				break;
			}
			name += c;
		}
		//name = name.ToLower();
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		///////uniqueDiscordID = usr.Id;
	}
	bool Player::hasItem(std::string itemName)
	{
		for (auto& i : items) {
			if (i.getName() == itemName) {
				return true;
			}
		}
		return false;
	}
	
	//void setDigimon(Digimon& d) { digimon = d; }

}
