#include "PVP.h"
#include "VeeBot.h"

namespace _VEEBOT_{
	PVP::PVP(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, std::map<std::string, Player>& _players) :
		bot(_bot),
		client(_client),
		ch(_ch),
		//serv(client.getServer().operator SleepyDiscord::Server),
		players(_players)
	{
		bP1Accept = false;
		bP2Accept = false;
		bP1Chosen = false;
		bBattleStarted = false;
		bRoundStarted = false;
		PVPstartTimer.start();

		startTime = 60; //In minutes
		
		p1 = &players.find(bot.getMaster())->second;

		///serv = e.Server;
		///chan = e.Channel;

		p1RPSchoice = "";
		p2RPSchoice = "";


	}

	void PVP::processCommand(SleepyDiscord::Message& message)
	{
		//Typing pvp accept as player 1
		if (message.content == "!pvp accept" && message.author.username == p1->getDiscordName() && !bP1Accept && !bP2Accept)
		{
			bP1Accept = true;
			message.reply(&client, "KHEARTZ IS A PLEB");
			p2 = &players.find("holyniwa")->second; //CHOOSE P2 CAND
			//serv.
			
			//////////serv.GetUser(p2->getDiscordNameWODisc(), p2->getDisc()).SendMessage("You have been challenged to PVP! Do you accept? Type !pvp accept to battle your opponent!");
			P1ChooseTimer.stop();
			P2ChooseTimer.start();
			return;
		}
		//Typing pvp accept as player 2
		if (message.content == "!pvp accept" && bP1Accept && !bP2Accept && message.author.ID == p2->getDiscordName())
		{
			bP2Accept = true;
			message.reply(&client, "KHEARTZ IS A FUCKING PLEB AGAIN");
			/////////serv.GetUser(p2->getDiscordNameWODisc(), p2->getDisc()).SendMessage("Head over to #PVP to fight!");
			battleTimer.start();
			client.sendMessage(ch, "PVP HAS STARTED!");
			client.sendMessage(ch, "@" + p1->getDiscordNameWODisc() + " vs " + "@" + p2->getDiscordNameWODisc());
			P2ChooseTimer.stop();

			std::string msg = "";
			msg += "Player One stats: " + p1->getDigimon().getName() + ": ";
			msg += "HP: " + std::to_string(p1->getDigimon().getHP()) + "/" + std::to_string(p1->getDigimon().getMaxHP()) + "; ";
			msg += "MP: " + std::to_string(p1->getDigimon().getMP()) + "/" + std::to_string(p1->getDigimon().getMaxMP()) + "; ";
			msg += "Str: " + std::to_string(p1->getDigimon().getStr()) + "; " + "Def: " + std::to_string(p1->getDigimon().getDef()) + "; ";
			msg += "Speed: " + std::to_string(p1->getDigimon().getSpd()) + ".";

			message.reply(&client, msg);
			client.uploadFile(ch, "data/pictures/" + p1->getDigimon().getPic(), "");


			msg = "";

			msg += "Player Two stats: " + p2->getDigimon().getName() + ": ";
			msg += "HP: " + std::to_string(p2->getDigimon().getHP()) + "/" + std::to_string(p2->getDigimon().getMaxHP()) + "; ";
			msg += "MP: " + std::to_string(p2->getDigimon().getMP()) + "/" + std::to_string(p2->getDigimon().getMaxMP()) + "; ";
			msg += "Str: " + std::to_string(p2->getDigimon().getStr()) + "; " + "Def: " + std::to_string(p2->getDigimon().getDef()) + "; ";
			msg += "Speed: " + std::to_string(p2->getDigimon().getSpd()) + ".";

			message.reply(&client, msg);
			client.uploadFile(message.channelID, "data/pictures/" + p2->getDigimon().getPic(), "");
			bBattleStarted = true;
			// bRoundStarted = true;
			return;
		}
		//"!pvp choose Holyniwa#
		if (message.content.size() > 11 && message.content.substr(0, 11) == "!pvp choose")
		{
			string oppoName = "";
			if (11 < message.content.size() - 1)
			{
				oppoName = message.content.substr(15, message.content.size() - 11);
			}
			
			auto pIt = players.find(oppoName);
			if (pIt != players.end())
			{
				if (pIt->second.getName() == oppoName)
				{
					p2 = &pIt->second;

				}

			}
			bP1Accept = true;

		}

		if (message.content == "!pvp accept" && bP1Accept && !bP2Accept && message.author.username == p2->getDiscordName()) //double check getDiscordName against sleepy discord
		{
			bP1Accept = true;
		}
	}

	void PVP::processMsg(SleepyDiscord::Message& message)
	{
		if (bBattleStarted && message.author.ID == p1->getDiscordName())
		{
			if (message.content == "paper" || message.content == "scissors" || message.content == "rock")
			{
				p1RPSchoice = message.content;
				client.sendMessage(ch, "Player one has selected " + p1RPSchoice);
			}
		}
		else if (bBattleStarted && message.author.ID == p2->getDiscordName())
		{
			if (message.content == "paper" || message.content == "scissors" || message.content == "rock")
			{
				p2RPSchoice = message.content;
				client.sendMessage(ch, "Player two has selected " + p2RPSchoice);
			}
		}
	}
	void PVP::update()
	{
		if (bBattleStarted)
		{
			if (p1RPSchoice == "" || p2RPSchoice == "")
			{
				return;
			}
			Player* wPlayer = p1;
			Player* lPlayer = p2;
			if (p1RPSchoice != "" && p2RPSchoice != "") {
				if (p1RPSchoice == p2RPSchoice)
				{
					client.sendMessage(ch, "Both players have selected " + p1RPSchoice);
					client.sendMessage(ch, "Players, please choose again!");//lPlayer.
					//Reset choices
					p1RPSchoice = "";
					p2RPSchoice = "";
					return;
				}
			}
			//If P1 wins
			if ((p1RPSchoice == "paper" && p2RPSchoice == "rock") ||
				(p1RPSchoice == "rock" && p2RPSchoice == "scissors") ||
				(p1RPSchoice == "scissors" && p2RPSchoice == "paper"))
			{
				wPlayer = p1;
				lPlayer = p2;
			}
			//If P2 winsgetDigimon
			if ((p2RPSchoice == "paper" && p1RPSchoice == "rock") ||
				(p2RPSchoice == "rock" && p1RPSchoice  == "scissors") ||
				(p2RPSchoice == "scissors" && p1RPSchoice == "paper"))
			{
				wPlayer = p2;
				lPlayer = p1;
			}

			client.sendMessage(ch, wPlayer->getDiscordNameWODisc() + " has won the cointoss!");

			//Start the fight
			//wP attacks lP
			int dmg = (int)(wPlayer->getDigimon().getStr() / (lPlayer->getDigimon().getRank() * lPlayer->getDigimon().getDef() * 0.25));

			//check for type adv
			if ((wPlayer->getDigimon().getType() == "virus" && lPlayer->getDigimon().getType() == "data") ||
				(wPlayer->getDigimon().getType() == "data" && lPlayer->getDigimon().getType() == "vaccine") ||
				(wPlayer->getDigimon().getType() == "vaccine" && lPlayer->getDigimon().getType() == "virus"))
			{
				dmg = (int)(dmg * 1.5);
				dmg *= 1000;
			}

			client.sendMessage(ch, wPlayer->getDigimon().getName() + " deals damage to " + lPlayer->getDigimon().getName() + " for " + std::to_string(dmg) + " damage!");

			lPlayer->getDigimon().incHP(-dmg);

			//Show stats again

			string msg = "";
			msg += "Player One stats: " + p1->getDigimon().getName() + ": ";
			msg += "HP: " + std::to_string(p1->getDigimon().getHP()) + "/" + std::to_string(p1->getDigimon().getMaxHP()) + "; ";
			msg += "MP: " + std::to_string(p1->getDigimon().getMP()) + "/" + std::to_string(p1->getDigimon().getMaxMP()) + "; ";
			msg += "Str: " + std::to_string(p1->getDigimon().getStr()) + "; " + "Def: " + std::to_string(p1->getDigimon().getDef()) + "; ";
			msg += "Speed: " + std::to_string(p1->getDigimon().getSpd()) + ".";

			client.sendMessage(ch, msg);
			client.uploadFile(ch, "data/pictures/" + p1->getDigimon().getPic(), "");

				msg = "";

			msg += "Player Two stats: " + p2->getDigimon().getName() + ": ";
			msg += "HP: " + std::to_string(p2->getDigimon().getHP()) + "/" + std::to_string(p2->getDigimon().getMaxHP()) + "; ";
			msg += "MP: " + std::to_string(p2->getDigimon().getMP()) + "/" + std::to_string(p2->getDigimon().getMaxMP()) + "; ";
			msg += "Str: " + std::to_string(p2->getDigimon().getStr()) + "; " + "Def: " + std::to_string(p2->getDigimon().getDef()) + "; ";
			msg += "Speed: " + std::to_string(p2->getDigimon().getSpd()) + ".";

			client.sendMessage(ch, msg);
			client.uploadFile(ch,"/data/pictures/" + p2->getDigimon().getPic(), "");

				p1RPSchoice = "";
			p2RPSchoice = "";
			//bRoundStarted = false;

			if (lPlayer->getDigimon().getHP() <= 0)
			{
				client.sendMessage(ch, p2->getName() + " has been defeated!");
				client.sendMessage(ch, p1->getName() + " has eardned 10000000000000 BITs!!!!11");
				bBattleStarted = false;
				bP1Accept = false;
				bP1Chosen = false;
				bP2Accept = false;
				PVPstartTimer.start();
				p1->getDigimon().setHP(p1->getDigimon().getMaxHP());
				p1->getDigimon().setMP(p1->getDigimon().getMaxMP());
				p2->getDigimon().setHP(p1->getDigimon().getMaxHP());
				p2->getDigimon().setMP(p1->getDigimon().getMaxMP());
			}


		}
		//Start the event
		if (PVPstartTimer.getTimeInSec() >= 10 && !bP1Chosen)
		{
			bP1Chosen = true;
			//int idx = 0;
			//idx = rng.Next(0, players.Count - 1);
			// p1 = players.Values.ElementAt(idx);
			sendPM(*p1, "You have been chosen to PvP. Please type \"!pvp accept\" to accept."s);
			PVPstartTimer.stop();
		}

		//Player 1's timer to accept
		if (P1ChooseTimer.getTimeInSec() > 10 && !bP1Accept)
		{
			client.sendMessage(ch, "Player 1 timeout...Choosing another player!");
			//Choose another P1
			////////////p1 = &players["kheartz"];
			P1ChooseTimer.stop();
			P1ChooseTimer.start();
			//serv.GetUser(p1->getDiscordName(), disc).SendMessage(message);
		}
		//Player 2's timer to accept
		if (P2ChooseTimer.getTimeInSec() > 10 && !bP2Accept)
		{
			client.sendMessage(ch, "Player 2 timeout...Choosing another player!");
			//Choose another P2 randomly
			p2 = &players.find("holyniwa")->second;
			P2ChooseTimer.stop();
			P2ChooseTimer.start();
			//serv.GetUser(p1->getDiscordName(), disc).SendMessage(message);
			//show their digimon.
		}
		//Battle is ready to commence
		if (battleTimer.getTimeInSec() > 10 && bBattleStarted)
		{
			bBattleStarted = true;
		}
	}
	void PVP::sendPM(Player& p, std::string& message)
	{
		//serv.GetUser(p.getDiscordNameWODisc(), p.getDisc()).SendMessage(message);
		auto dm = client.createDirectMessageChannel(p.getUniqueDiscordID());
		std::string dmChanID = dm.header["id"];
		client.sendMessage(dmChanID, message);
	}
}