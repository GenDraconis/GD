#include "Trivia.h"
#include "Player.h"
#include "Utility.h"
#include "VeeBot.h"

namespace _VEEBOT_ {
	Trivia::Trivia(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, std::map<std::string, Player>& _players) :
		bot(_bot),
		client(_client),
		ch(_ch),
		players(_players)
	{
		rng = std::mt19937(rd());


		bIntroAnnounced = false;
		bQuestionAnnounced = false;
		startTime = 90; //in mins
		waitTime = 60; //in sec

		std::ifstream triviaFILE;
		triviaFILE.open("data/trivia.txt");

		timer.start();
		//ch = e.Channel;
		while (!triviaFILE.eof())
		{
			std::string tempCategory;
			std::getline(triviaFILE, tempCategory);
			std::string tempQuestion;
			std::getline(triviaFILE, tempQuestion);
			std::string tempAnswer;
			std::getline(triviaFILE, tempAnswer);
			questions.emplace_back(tempCategory, tempQuestion, tempAnswer);
		}
		//Randomize Question list
		std::shuffle(questions.begin(), questions.end(), rng);
		//e.Channel.SendMessage(questions.Count + " number of questions has been added to Trivia database.");
	}
	void Trivia::processMsg(SleepyDiscord::Message& message)
	{
		if (bQuestionAnnounced && toLower(message.content) == toLower(questions[idx].getAnswer()))
		{
			timer.stop();
			timer.start();
			bIntroAnnounced = false;
			bQuestionAnnounced = false;

			//Player p;
			//if (players.TryGetValue(e.User.Id, out p))
			auto pIt = players.find(message.author.ID);
			if (pIt != players.end())
			{
				//Console.WriteLine("Player found");
				std::cout << "Player found\n";
				pIt->second.addTriviaPoints(questions[idx].getPointsAdded());
				pIt->second.addBITs(questions[idx].getPointsAdded());
			}
			else
			{
				std::cout << "Player not found. Please register with bot.\n";
				/*Player p = new Player(e.User);
				p.addTriviaPoints(questions[idx].getPointsAdded());
				p.addBITs(questions[idx].getPointsAdded());*/
			}
			message.reply(&client, "ANSWER:" + questions[idx].getAnswer() + " by " + message.author.username + "! Awarded " + std::to_string(questions[idx].getPointsAdded()) + " trivia points/BIT(s).");
			bot.save();
		}
		return;
	}
	void Trivia::update()
	{
		// if(timer.getTimeInSec() >= (60 *  startTime - 30) && !bIntroAnnounced)
		if (timer.getTimeInSec() >= (60 * startTime - 45) && !bIntroAnnounced)
		{
			bIntroAnnounced = true;
			idx++;
			client.sendMessage(ch, questions[idx].getCategory());
		}

		else if (timer.getTimeInSec() >= 60 * startTime && !bQuestionAnnounced)
		{
			bQuestionAnnounced = true;
			client.sendMessage(ch, questions[idx].getQuestion());
		}
		else if (timer.getTimeInSec() > (60 * startTime) + waitTime && bQuestionAnnounced)
		{
			bQuestionAnnounced = false;
			bIntroAnnounced = false;
			timer.stop();
			timer.start();
			client.sendMessage(ch, "TIMES UP! The answer is: " + questions[idx].getAnswer());
		}
	}
	void Trivia::processCommand(SleepyDiscord::Message& message)
	{
		if (message.content == "!trivia pause" && message.author.ID == bot.getMaster())
		{
			timer.pause();
			message.reply(&client,"Trivia timer has been paused!");
		}
		else if (message.content == "!trivia unpause" && message.author.ID == bot.getMaster())
		{
			timer.unpause();
			message.reply(&client, "Trivia timer has been unpaused!");
		}
		else if (message.content.size() >= 15 && message.content.substr(0, 15) == "!trivia settime" && message.author.ID == bot.getMaster())
		{
			int newTime = 0;
			std::string newTimeS = "";
			if (15 < message.content.size() - 1)
			{
				newTimeS = message.content.substr(15, message.content.size() - 15);
			}
			else
			{
				return;
			}
			newTime = std::stoi(newTimeS);
			startTime = newTime;
			timer.unpause();
			message.reply(&client, "Trivia timer set to " + std::to_string(newTime) + " minute(s).");
		}

		else if (message.content == "!trivia timer")
		{
			long long int diff = startTime - timer.getTimeInSec() / static_cast<long long int>(60);
			message.reply(&client, "Next question starts in " + std::to_string(diff) + " minute(s).");

		}
		else if (message.content == "!trivia points") {
			auto pIt = players.find(message.author.ID);
			if (pIt == players.end()) {
				message.reply(&client, "Player not found. Please register with !rpg register in #rpg");
			}
			else {
				message.reply(&client, pIt->second.getDiscordName() + ", you have " +  std::to_string(pIt->second.getTriviaPoints()) + " points.");
			}
		}
		else if (message.content == "!trivia leaderboard")
		{
			message.reply(&client, "Feature currently down.");
			std::map<Player*, int> triviaboard;
			//foreach(KeyValuePair<ulong, Player> entry in players)
			for (auto& entry : players)
			{
				if (entry.second.getTriviaPoints() > 0)
				{
					triviaboard[&entry.second] = entry.second.getTriviaPoints();
				}
			}
			//Order map by value?
			//////////auto ordered = triviaboard.OrderByDescending(x = > x.Value);
			int list = 0;
			//foreach(KeyValuePair<Player, int> entry in ordered)
			/////////////////FIXTHIS////////////////
			/*for (auto& entry : ordered)
			{
				list++;
				message.reply(&client, entry.first.getDiscordName() + " " + entry.second);
				if (list >= 3)
				{
					return;
				}
			}*/
		}
	}
}