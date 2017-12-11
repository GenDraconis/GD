#pragma once

#include <iostream>
#include <random>
#include <vector>
#include <random>
#include <map>
#include <fstream>
#include <numeric>
#include <memory>


#include "RPG.h"
#include "Trivia.h"
#include "PVP.h"

#include "sleepy_discord/websocketpp_websocket.h"
#include "sleepy_discord/user.h"


namespace _VEEBOT_
{

	class VeeBot : public SleepyDiscord::DiscordClient
	{
	private:
		////////DiscordClient bot;
		//SleepyDiscord::DiscordClient client = *this;

		std::unique_ptr<RPG> rpg;
		std::unique_ptr<PVP> pvp;
		std::unique_ptr<Trivia> trivia;

		//std::map<unsigned long int, Player> players; //Key: DiscordName Value: Player object
		//std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player> players; //Key: DiscordName Value: Player object
		std::map<std::string, Player> players;
		std::map<std::string, Player> playersByNormName; //Key: Name Value: Player object
		//std::map<unsigned long int, std::vector<Digimon>> digiDestinedToBe; //Key DiscordName; Digimon reference
		std::map<std::string, std::vector<Digimon>> digiDestinedToBe; //Key DiscordName; Digimon reference
		std::vector<Digimon> starterDigimon;

		std::random_device rd;
		std::mt19937 rng;
		std::uniform_int_distribution<int> idist;

		std::string masterSnowflake = "82980657826893824";
	public:
		VeeBot(std::string token);
		//void Bot_MessageReceived(SleepyDiscord::Message& message);
		using SleepyDiscord::DiscordClient::DiscordClient;
		void onMessage(SleepyDiscord::Message message);
		void run() {
			while (true)
			{
				if (rpg != nullptr)
				{
					rpg->update();
				}
				if (trivia != nullptr)
				{
					trivia->update();
				}
				if (pvp != nullptr)
				{
					pvp->update();
				}
			}
			
		}
		std::string getMaster() const { return masterSnowflake; }
		//private void Bot_MessageReceived(object sender, MessageEventArgs e)
		void commandMsg(SleepyDiscord::Message& message);
		void save();
		
	};
}