#pragma once

#include <algorithm>
#include <vector>

#include <memory>

#include "Digimon.h"
#include "Item.h"

#include "sleepy_discord/websocketpp_websocket.h"
#include "sleepy_discord/user.h"


namespace _VEEBOT_
{
	class Player
	{
	private:
		std::string name;
		std::string discordName;
		std::string discordNameNoDisc;
		SleepyDiscord::Snowflake<SleepyDiscord::User> uniqueDiscordID;

		std::unique_ptr<Digimon> digimon;
		int bits;
		int triviaPoints;
		unsigned short int disc;

		std::vector<Item> items;
	public:
		Player(std::vector<std::string> playerInfo, std::vector<std::string> digimonInfo);
		Player(SleepyDiscord::Snowflake<SleepyDiscord::User>& usr);
		bool Player::hasItem(std::string itemName);

		std::string getName() const { return name; }
		std::string getDiscordName() const { return discordName; }
		std::string getDiscordNameWODisc() const { return discordNameNoDisc; }
		SleepyDiscord::Snowflake<SleepyDiscord::User> getUniqueDiscordID() const { return uniqueDiscordID; }
		unsigned short int getDisc() const { return disc; }
		Digimon& getDigimon() const { return *digimon.get(); }
		int getBits() const { return bits; }
		int getTriviaPoints() const { return triviaPoints; }
		void addBITs(int b) { bits += b; }
		void addTriviaPoints(int b) { triviaPoints += b; }
		void setDiscordName(std::string d) { discordName = d; }
		void setDigimon(Digimon d) { digimon = std::make_unique<Digimon>(d); }
		//void setUniqueDiscordID(sleepyUser& v) { uniqueDiscordID = v; }
	};
}
