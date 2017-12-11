#pragma once

#include <map>
#include <random>

#include "sleepy_discord/websocketpp_websocket.h"
#include "Timer.h"

namespace _VEEBOT_
{
	class Player;
	class VeeBot;
	class PVP
	{
	private:
		VeeBot& bot;
		SleepyDiscord::DiscordClient& client;
		//SleepyDiscord::Channel& ch;
		SleepyDiscord::Snowflake<SleepyDiscord::Channel>& ch;
		//SleepyDiscord::Server& serv;
		Player* p1; //Bot chooses this randomly
		Player* p2; //Bot choose this randomly

		Timer PVPstartTimer; //Starts the event--Bot chooses player 1 candidate
		Timer P1ChooseTimer; //Timer for player one accept
		Timer P2ChooseTimer; //Timer for player two to accept
		Timer battleTimer; //Time for battle to begin

		int startTime; //Time for PVP event to initiate
		int acceptTime; //Time to accept

								// private List<Player> players;


		bool bP1Chosen; //Flag for if the bot has chosen a player --- ??needed?
		bool bP1Accept; //Flag for if candidate P1 has accepted
		bool bP2Accept; //Flag for if candidate P2 has accepted
		bool bBattleStarted; //Flag for if the battle has started
		bool bRoundStarted;
		//std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player>& players;
		std::map<std::string, Player>& players;
		////Server serv;
		/////Channel chan;

		std::string p1RPSchoice;
		std::string p2RPSchoice;

		std::random_device rd;
		std::mt19937 rng;
		std::uniform_int_distribution<int> idist;

	public:

		PVP(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, std::map<std::string, Player>& _players);
		void processCommand(SleepyDiscord::Message& message);
		void processMsg(SleepyDiscord::Message& message);
		void update();
		void sendPM(Player& p, std::string& message);


	};
}
/*
KHEARTZ WILL TURN ON PVP

TIMER
TIMER HITS A CERTAIN TIME
RANDOMLY CHOOSE A PLAYER IN PLAYER LIST THAT IS `ONLINE`
ANOTHER TIMER
FOR P1 TO CHOOSE A P2 OPPONENT
----CRITERIA
--------SAME RANK















*/