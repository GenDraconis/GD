#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <random>
#include <fstream>


#include "Player.h"
#include "Digimon.h"
#include "Timer.h"

#include "sleepy_discord/websocketpp_websocket.h"

using namespace std;

namespace _VEEBOT_
{

	class VeeBot;
	class RPG
	{
	private:
		
		VeeBot& bot;
		SleepyDiscord::DiscordClient& client;
		//SleepyDiscord::Channel& ch;
		SleepyDiscord::Snowflake<SleepyDiscord::Channel>& ch;

		bool bActive;
		//std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player>& players;
		std::map<std::string, Player>& players;
		std::vector<Player*> subscribedPlayers;
		std::vector<Digimon> enemies;
		bool bRaidAnnounced;
		bool bRaidStarted;
		bool bTurnPrepared;
		bool bBattle;
		int spawnTime;
		int beginTime;
		int beserkTurn;
		int turnCount = 0;
		Timer raidTimer;
		Timer turnTimer;
		
		std::random_device rd;
		std::mt19937 rng;
		std::uniform_int_distribution<int> idist;

		std::vector<Player*> digimonDead;
		std::vector<Player*> raidParticipants;
		std::vector<Player*> waitingRaiders;

		bool enemySpecial;

		std::unordered_map<std::string, std::string> battleChoice;

		int rewardPoints;

		std::ofstream RAIDLOG;

		//Server serv;
		//Channel ch;
	public:
		//RPG(SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, VeeBot& _bot, SleepyDiscord::DiscordClient& _client, std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player>& _players);
		RPG(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, std::map<std::string, Player>& _players);
		void processMsg(SleepyDiscord::Message& message);
		void commands(SleepyDiscord::Message& message);
		void raidUpdate(SleepyDiscord::Message& message);
		void processCommand(SleepyDiscord::Message& message);
		void update();
		void checkDigivolution(Player* raider);
		void digivolve(Player* raider);
		void resetEnemies();
		bool isActive() { return bActive; }
		std::ofstream& getRaidLog() { return RAIDLOG; }
	};
}
