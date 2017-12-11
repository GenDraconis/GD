#pragma once

#include <vector>
#include <map>
#include <random>
#include <fstream>

#include "Timer.h"
#include "TriviaQuestion.h"


#include "sleepy_discord/websocketpp_websocket.h"

//using sleepyUser = SleepyDiscord::Snowflake<SleepyDiscord::User>;
//using sleepyChannel = SleepyDiscord::Snowflake<SleepyDiscord::Channel>;

namespace _VEEBOT_ {
	class VeeBot;
	class Player;
	class Trivia {
	private:
		VeeBot& bot;
		SleepyDiscord::DiscordClient& client;
		//SleepyDiscord::Channel& ch;
		SleepyDiscord::Snowflake<SleepyDiscord::Channel> ch;

		std::vector<TriviaQuestion> questions;
		//std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player>& players;
		std::map<std::string, Player>& players;

		Timer timer;
		std::random_device rd;
		std::mt19937 rng;
		std::uniform_int_distribution<int> idist;

		unsigned short int startTime;
		unsigned short int waitTime;

		int idx;

		bool bIntroAnnounced;
		bool bQuestionAnnounced;
	public:
		//Trivia(SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, VeeBot& _bot, SleepyDiscord::DiscordClient& _client,
		//	std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player>& _players);
		Trivia(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, std::map<std::string, Player>& _players);
		void processMsg(SleepyDiscord::Message& message);
		void processCommand(SleepyDiscord::Message& message);
		void update();
	};
}