#include "RPG.h"
#include "VeeBot.h"

namespace _VEEBOT_ {
	RPG::RPG(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Snowflake<SleepyDiscord::Channel>& _ch, std::map<std::string, Player>& _players) :
		bot(_bot),
		client(_client),
		ch(_ch),
		players(_players)
	{
		bActive = true;

		rng = std::mt19937(rd());
		idist = std::uniform_int_distribution<int>(1, 100);

		////serv = e.Server;
		////ch = e.Channel;

		bRaidAnnounced = false;
		bRaidStarted = false;
		bTurnPrepared = false;
		bBattle = false;

		raidTimer.start();
		spawnTime = 180;
		beginTime = 300;
		turnCount = 0;
		beserkTurn = 5;
		rewardPoints = 0;

		enemySpecial = false;

		//Load in Subscribers
		std::ifstream subFILE;
		subFILE.open("data/subscribe.txt");

		//string file = @".\data\subscribe2.txt";
		// StreamWriter saveFILE = new StreamWriter(file);

		while (subFILE.is_open() && !subFILE.eof())
		{
			std::string disID;
			getline(subFILE, disID);
			//foreach(KeyValuePair<ulong, Player> entry in players)
			for (auto& player : players)
			{
				//if(entry.Value.getDiscordName() == name)
				if (player.second.getUniqueDiscordID() == disID)
				{
					subscribedPlayers.push_back(&player.second);
					// e.Channel.SendMessage(entry.Value.getDiscordName() + " " + entry.Value.getUniqueDiscordID());
					// saveFILE.WriteLine(entry.Value.getUniqueDiscordID());
				}
			}
		}
		//saveFILE.Close();


		RAIDLOG;// = new StreamWriter(@". / data / rpglog.txt");
		RAIDLOG.open("data/rpglog.txt");
		//Load in Enemies
		std::ifstream enemiesFILE;
		enemiesFILE.open("data/enemy.txt");// = new StreamReader(@". / data / enemy.txt");
		while (enemiesFILE.is_open() && !enemiesFILE.eof())
		{
			std::vector<std::string> digiInfo;// = new List<string>();
			std::string temp = "";
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //Name
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //HP
			digiInfo.push_back(temp); //MaxHP
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //MP
			digiInfo.push_back(temp); //MaxMP
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //Str
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //Def
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //Speed
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //Rank
			getline(enemiesFILE, temp);
			digiInfo.push_back("0"); //DV INFO
			digiInfo.push_back(temp); //Special
			getline(enemiesFILE, temp);
			digiInfo.push_back(temp); //Type
			enemies.emplace_back(digiInfo);
		}


	}
	void RPG::processMsg(SleepyDiscord::Message& message)
	{
		commands(message);
		return;
	}
	void RPG::commands(SleepyDiscord::Message& message)
	{
		
		if (message.content == "!rpg stats")
		{
			auto& pIt = players.find(message.author.ID);
			std::string msg = "";
			//if (players.TryGetValue(e.User.Id, out P))
			if (pIt != players.end())
			{
				Player& P = pIt->second;
				msg += message.author.usernameAsMention() + ", your " + P.getDigimon().getName() + " has the following stats: ";
				msg += "HP: " + std::to_string(P.getDigimon().getHP()) + "/" + std::to_string(P.getDigimon().getMaxHP()) + "; ";
				msg += "MP: " + std::to_string(P.getDigimon().getMP()) + "/" + std::to_string(P.getDigimon().getMaxMP()) + "; ";
				msg += "Str: " + std::to_string(P.getDigimon().getStr()) + "; " + "Def: " + std::to_string(P.getDigimon().getDef()) + "; ";
				msg += "Speed: " + std::to_string(P.getDigimon().getSpd()) + ".";
				message.reply(&client, msg);
				/////e.Channel.SendMessage(msg);
				//////e.Channel.SendFile(@". / data / pictures / " + P.getDigimon().getPic());
			}
			else
			{
				msg += message.author.usernameAsMention() + ", you do not have a registered Digimon.. Type \"!register\" to become DigiDestined!";
				message.reply(&client, msg);
			}

		}
		else if (message.content == "!rpg heal")
		{
			message.reply(&client, message.author.usernameAsMention() + ", RPG heals have been turned off for beta testing.");
			return;
			
			std::string msg = "";
			auto pIt = players.find(message.author.ID);
			if (pIt != players.end())
			{
				Player* P = &pIt->second;
				if (P->getBits() >= 3 && (P->getDigimon().getHP() != P->getDigimon().getMaxHP() || P->getDigimon().getMP() != P->getDigimon().getMaxMP()))
				{
					for(auto& raider : raidParticipants)
					{
						if (raider->getDiscordName() == P->getDiscordName())
						{
							message.reply(&client, message.author.usernameAsMention() + ", you are in the middle of a raid! You may not heal!");
							return;
						}
					}
					P->addBITs(-3);
					P->getDigimon().setHP(P->getDigimon().getMaxHP());
					P->getDigimon().setMP(P->getDigimon().getMaxMP());
					message.reply(&client, message.author.usernameAsMention() + ", your partner has been fully healed!");
				}
				else
				{
					message.reply(&client, message.author.usernameAsMention() + ", you do not qualify for Heal. Check your BITs your partner's health!");
				}
			}
			else
			{
				msg += message.author.usernameAsMention() + ", you do not have a registered Digimon.. Type \"!register\" to become DigiDestined!";
				message.reply(&client, msg);
			}
		}
		else if (message.content == "!rpg pause" && message.author.ID == bot.getMaster())
		{
			raidTimer.pause();
			message.reply(&client, "RPG timer has been paused!");
		}
		else if (message.content == "!rpg unpause" && message.author.ID == bot.getMaster())
		{
			raidTimer.unpause();
			message.reply(&client, "RPG timer has been unpaused!");
		}
		else if (message.content.size() >= 12 && message.content.substr(0, 12) == "!rpg settime" && message.author.ID == bot.getMaster())
		{
			int newTime = 0;
			std::string newTimeS = "";
			if (12 < message.content.size() - 1)
			{
				newTimeS = message.content.substr(12, message.content.size() - 12);
			}
			else
			{
				return;
			}
			newTime = std::stoi(newTimeS);
			spawnTime = newTime;
			raidTimer.unpause();
			message.reply(&client, "RPG timer set to " + std::to_string(newTime) + " minutes.");
		}

		else if (message.content == "!rpg timer")
		{
			long long int diff = spawnTime - raidTimer.getTimeInSec() / static_cast<long long>(60);
			message.reply(&client, "Next raid starts in " + std::to_string(diff) + " minutes.");

		}
		else if (message.content == "!rpg destroy" && message.author.ID == bot.getMaster() && bRaidStarted)
		{
			enemies[0].setHP(0);
		}
		else if (message.content == "!rpg restore" && message.author.ID == bot.getMaster())
		{
			//foreach(KeyValuePair<ulong, Player> entry in players)
			for(auto& entry : players)
			{
				entry.second.getDigimon().setHP(entry.second.getDigimon().getMaxHP());
				entry.second.getDigimon().setMP(entry.second.getDigimon().getMaxMP());
			}
			message.reply(&client, "All players have been restored");
		}
		else if (message.content == "!rpg join" && bRaidAnnounced && !bRaidStarted)
		{
			auto pIt = players.find(message.author.ID);
			if (pIt != players.end())
			{
				for(auto& raider : raidParticipants)
				{
					if (raider->getDiscordName() == pIt->second.getDiscordName())
					{
						message.reply(&client, message.author.usernameAsMention() + ", you have already joined this raid.");
						return;
					}
				}
				if (pIt->second.getDigimon().getHP() <= 0)
				{
					message.reply(&client, "Your digimon has no will to fight! Type \"!rpg heal\" to heal your partner!");
					return;
				}
				else
				{
					// if (p.getUniqueDiscordID() == UInt64.Parse("82980657826893824"))
					// {
					raidParticipants.push_back(&pIt->second);
					message.reply(&client, message.author.usernameAsMention() + " and " + pIt->second.getDigimon().getName() + " has joined the raid!");
					//   }
					return;
				}
			}
			else
			{
				message.reply(&client, message.author.usernameAsMention() + ". Sorry, only DigiDestined may join the raid!");
				return;
			}
		}
		else if (message.content == "!rpg subscribe")
		{
			auto pIt = players.find(message.author.ID);
			if (pIt != players.end())
			{
				for(auto& P : subscribedPlayers)
				{
					if (message.author.ID == P->getUniqueDiscordID())
					{
						message.reply(&client, message.author.usernameAsMention() + ", you already subscribed to RPG alerts!");
						return;
					}
				}
				subscribedPlayers.push_back(&pIt->second);
				message.reply(&client, message.author.usernameAsMention() + ", you have subscribed to RPG alerts!");
				ofstream subFILE;
				subFILE.open("data/subscribe.txt");
				for(auto& p : subscribedPlayers)
				{
					subFILE << std::string(p->getUniqueDiscordID()) << "\n";
				}
				subFILE.close();
			}
			else
			{
				message.reply(&client, message.author.usernameAsMention() + ", you are not registered to the bot, Please type \"!regster\".");
			}
		}
		else if (message.content == "!rpg unsubscribe")
		{

			auto pIt = subscribedPlayers.begin();
			if (pIt != subscribedPlayers.end()) {
				message.reply(&client, message.author.usernameAsMention() + ", you are not subscribed to RPG alerts!");
			}
		}
	}
	void RPG::raidUpdate(SleepyDiscord::Message& message)
	{
		/******************BEGINCHOICE*********************/
		if (bTurnPrepared && waitingRaiders.size() > 0 && !bBattle)
		{
			for(auto& raider : waitingRaiders)
			{
				if (raider->getUniqueDiscordID() == message.author.ID && raider->getDigimon().getHP() > 0)
				{
					if (message.content == "2")
					{
						if (raider->getDigimon().getMP() <= 0)
						{
							client.sendMessage(ch, raider->getName() + ", your " + raider->getDigimon().getName() + " is out of MP! Please choose another option.");
							return;
						}
						else
							battleChoice[raider->getUniqueDiscordID()] = message.content;
					}
					else if (message.content == "1")
					{
						battleChoice[raider->getUniqueDiscordID()] = message.content;
					}
				}
			}
			//return "";
		}
		/************************************************/
		return;
		// return "";
	}
	void RPG::processCommand(SleepyDiscord::Message& message)
	{

	}
	void RPG::update()
	{
		std::string specials = " ";
		std::string enrage = " ";

		/********************BATTLE************************/
		if (turnTimer.getTimeInSec() > 20 && !bBattle && bRaidStarted)
		{
			bBattle = true;
			turnTimer.stop();
			turnCount++; //Increase the turn counter
			RAIDLOG << "---------------------------TURN NUMBER: " + std::to_string(turnCount) + "---------------------------\n";
			int totalSpeed = 0;
			int totalDmg = 0;
			int indDmg = 0;
			int defBoost = 1;
			std::string defeatedDigimon = " ";



			// vector<Player*>::size_type i = 0;
			for(auto& raider : raidParticipants)
			{
				if (battleChoice.size() == 0)
				{
					break;
				}
				std::string x = "";
				//if (battleChoice.TryGetValue(raider->getUniqueDiscordID(), out x))
				/*if(battleChoice.find(raider->getUniqueDiscordID()) == battleChoice.end())
				{
					continue;
				}
				else
				{
					
				}*/
				
				indDmg = 0;
				////SET SPEED
				totalSpeed += raider->getDigimon().getSpd();
				////BATTLE CHOICE 1
				if (battleChoice[raider->getUniqueDiscordID()] == "1")
				{
					if ((raider->getDigimon().getType().substr(0, 3) == "Vac" && enemies[0].getType().substr(0, 3) == "Vir") ||
						(raider->getDigimon().getType().substr(0, 3) == "Vir" && enemies[0].getType().substr(0, 3) == "Dat") ||
						(raider->getDigimon().getType().substr(0, 3) == "Dat" && enemies[0].getType().substr(0, 3) == "Vac"))
					{
						indDmg = (int)(raider->getDigimon().getRank() * raider->getDigimon().getStr() * 1.5 / (enemies[0].getRank() * enemies[0].getDef()));
						if (raider->getDigimon().getRank() > enemies[0].getRank())
						{
							indDmg = (int)(indDmg / (3 * ((double)raider->getDigimon().getRank()) / ((double)enemies[0].getRank())));
						}

						if (idist(rng) < raider->getDigimon().getCritChance())
						{
							indDmg *= 2;
						}

						totalDmg += indDmg;

						//RAIDLOG.WriteLine(raider->getName() + "'s " + raider->getDigimon().getName() + " 1 " + indDmg + "*TYPEADV*");
						RAIDLOG << raider->getName() << "'s " << raider->getDigimon().getName() << " 1 " << indDmg << "*TYPEADV*\n";

					}
					else {
						indDmg = raider->getDigimon().getRank() * raider->getDigimon().getStr() / (enemies[0].getRank() * enemies[0].getDef());
						if (raider->getDigimon().getRank() > enemies[0].getRank())
						{
							indDmg = (int)(indDmg / (3 * ((double)raider->getDigimon().getRank()) / ((double)enemies[0].getRank())));
						}

						if (idist(rng) < raider->getDigimon().getCritChance())
						{
							indDmg *= 2;
						}
						totalDmg += indDmg;

						RAIDLOG << raider->getName() << "'s " << raider->getDigimon().getName() << " 1 " << indDmg << "\n";
					}
					//waitingRaiders.erase(waitingRaiders.begin() + i);
				}
				///BATTLE CHOICE 2
				else if (battleChoice[raider->getUniqueDiscordID()] == "2")
				{
					////////Type Adv///////
					if ((raider->getDigimon().getType().substr(0, 3) == "Vac" && enemies[0].getType().substr(0, 3) == "Vir") ||
						(raider->getDigimon().getType().substr(0, 3) == "Vir" && enemies[0].getType().substr(0, 3) == "Dat") ||
						(raider->getDigimon().getType().substr(0, 3) == "Dat" && enemies[0].getType().substr(0, 3) == "Vac"))
					{
						indDmg = (int)(raider->getDigimon().getRank() * raider->getDigimon().getStr() * 2 * 1.5 / (enemies[0].getRank() * enemies[0].getDef()));
						if (raider->getDigimon().getRank() > enemies[0].getRank())
						{
							indDmg = (int)(indDmg / (3 * ((double)raider->getDigimon().getRank()) / ((double)enemies[0].getRank())));
						}
						totalDmg += indDmg;
						specials += " (TYPE BONUS) ";

						RAIDLOG << raider->getName() << "'s " << raider->getDigimon().getName() << " 2 " << indDmg << "*TYPEADV*\n";


					}
					////////Normal Dmg/////////
					else {
						indDmg = raider->getDigimon().getRank() * raider->getDigimon().getStr() * 2 / (enemies[0].getRank() * enemies[0].getDef());
						if (raider->getDigimon().getRank() > enemies[0].getRank())
						{
							indDmg = (int)(indDmg / (3 * ((double)raider->getDigimon().getRank()) / ((double)enemies[0].getRank())));
						}
						totalDmg += indDmg;
						/*##############################################################################*/
						
						RAIDLOG << raider->getName() << "'s " << raider->getDigimon().getName() << " 2 " << indDmg;
					}

					specials += raider->getName() + "'s " + raider->getDigimon().getName() + " used " + raider->getDigimon().getSpecial() +
						" for " + std::to_string(indDmg) + " damage ";
					raider->getDigimon().incMP(-(int)(30 * raider->getDigimon().getRank() + (20 * (raider->getDigimon().getRank() - 1))));
					if (raider->getDigimon().getMP() < 0)
					{
						raider->getDigimon().setMP(0);
					}
				}
				else
					totalDmg += 0;
			}
			/////////////////////BATTLE/////////////////////////
			if (totalSpeed > enemies[0].getSpd())
			{////PLAYERS GO FIRST
				enemies[0].incHP(-(totalDmg / enemies[0].getRank()));
				if (enemies[0].getHP() <= 0)
				{ //if he dies, he cant attack
					enemies[0].setHP(0);
				}
				else //if he doesnt die
				{
					//	vector<Player>::size_type i = 0;
					enemySpecial = false;

					if (std::uniform_int_distribution<int>(0, 3)(rng) == 1)
					{
						enemySpecial = true;
					}
					for(auto& raider : raidParticipants)
					{
						double type = 1;
						if ((enemies[0].getType().substr(0, 3) == "Vac" && raider->getDigimon().getType().substr(0, 3) == "Vir") ||
							(enemies[0].getType().substr(0, 3) == "Vir" && raider->getDigimon().getType().substr(0, 3) == "Dat") ||
							(enemies[0].getType().substr(0, 3) == "Dat" && raider->getDigimon().getType().substr(0, 3) == "Vac"))
						{
							type = 1.5;
						}
						if (enemySpecial)
						{
							RAIDLOG << enemies[0].getName() << enemies[0].getSpecial() << "\n";

							//Extra damage to higher ranks
							if (raider->getDigimon().getRank() > enemies[0].getRank())
							{
								raider->getDigimon().incHP(-(int)(enemies[0].getRank() * enemies[0].getStr() * 2 * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost) * (raider->getDigimon().getRank() / enemies[0].getRank()));
							}
							else {
								raider->getDigimon().incHP(-(int)(enemies[0].getRank() * enemies[0].getStr() * 2 * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost));
							}
						}
						else {
							if (raider->getDigimon().getRank() > enemies[0].getRank())
							{
								raider->getDigimon().incHP(-(int)(enemies[0].getRank() * enemies[0].getStr() * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost) * (raider->getDigimon().getRank() / enemies[0].getRank()));
							}
							else {
								raider->getDigimon().incHP(-(int)(enemies[0].getRank() * enemies[0].getStr() * 2 * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost));
							}
						}
						if (raider->getDigimon().getHP() <= 0 || raider->getDigimon().getHP() > raider->getDigimon().getMaxHP())
						{
							raider->getDigimon().setHP(0);
							digimonDead.push_back(raider);
						}
					}
				}
			}
			else { //ENEMY GOES FIRST
				   //vector<Player>::size_type i = 0;
				enemySpecial = false;
				if (std::uniform_int_distribution<int>(0, 3)(rng) == 1)
				{
					enemySpecial = true;
				}
				if (turnCount == beserkTurn)
				{
					enemies[0].incStr(enemies[0].getStr());
				}
				for(auto& raider : raidParticipants)
				{
					double type = 1;
					if ((enemies[0].getType().substr(0, 3) == "Vac" && raider->getDigimon().getType().substr(0, 3) == "Vir") ||
						(enemies[0].getType().substr(0, 3) == "Vir" && raider->getDigimon().getType().substr(0, 3) == "Dat") ||
						(enemies[0].getType().substr(0, 3) == "Dat" && raider->getDigimon().getType().substr(0, 3) == "Vac"))
					{
						type = 1.5;
					}
					//RAIDLOG + enemies[0].getName() + "BASE DMG:" + enemies[0].getStr();

					if (enemySpecial)
					{

						raider->getDigimon().incHP(-(int)(enemies[0].getRank() * enemies[0].getStr() * 2 * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost));
						RAIDLOG << enemies[0].getName() << " " + raider->getName() << " " << (enemies[0].getRank() * enemies[0].getStr() * 2 * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost) << enemies[0].getSpecial() << "\n";
					
					}
					else {
						raider->getDigimon().incHP(-(int)(enemies[0].getRank() * enemies[0].getStr() * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost));
						RAIDLOG << enemies[0].getName() << " " << raider->getName() << " " << (enemies[0].getRank() * enemies[0].getStr() * 2 * type) / (raider->getDigimon().getRank() * raider->getDigimon().getDef() * defBoost) << "\n";
					
					}
					if (type == 1.5)
						RAIDLOG << " *TYPEADV* \n";
					RAIDLOG << " \n";
					if (raider->getDigimon().getHP() <= 0)
					{
						raider->getDigimon().setHP(0);
						digimonDead.push_back(raider);
						RAIDLOG << "DEAD: " << raider->getName() << "'s " << raider->getDigimon().getName() << "\n";
					}
				}
				enemies[0].incHP(-totalDmg);
			}
			/////////////////WRAP UP//////////////////

			for(auto& dead : digimonDead)
			{
				defeatedDigimon += " " + dead->getName();
			}
			if (digimonDead.size() >= raidParticipants.size())
				return; //"";
			else
				digimonDead.clear();
			bTurnPrepared = false;
			bBattle = false;

			if (enemies[0].getHP() <= 0)
			{
				enemies[0].setHP(0);
			}
			if (turnCount % beserkTurn == 0)
			{
				enrage = enemies[0].getName() + " GOES INTO BESERK MODE! ";
				enemies[0].incStr((int)(enemies[0].getStr() * 1.5));
				RAIDLOG << "***BESERK MODE ACTIVATED***\n";
			}
			if (digimonDead.size() != 0)
			{
				if (enemySpecial)
				{
					client.sendMessage(ch, specials + enrage + enemies[0].getName() + " UNLEASHES HIS SPECIAL MOVE: " + enemies[0].getSpecial() + " and has defeated: " + defeatedDigimon + ". " +
						enemies[0].getName() + " has " + std::to_string(enemies[0].getHP()) + " HP left. Please use !rpg stats to monitor your Digimon!");
					return;
				}
				else {
					client.sendMessage(ch, specials + enrage + enemies[0].getName() + " has attacked everyone in the raid and has defeated: " + defeatedDigimon + ". " +
						enemies[0].getName() + " has " + std::to_string(enemies[0].getHP()) + " HP left. Please use !rpg stats to monitor your Digimon!");
					return;
				}

			}
			else {
				if (enemySpecial)
				{
					client.sendMessage(ch, specials + enrage + enemies[0].getName() + " UNLEASHES HIS SPECIAL MOVE: " + enemies[0].getSpecial() + " and has defeated: " + defeatedDigimon + ". " +
						enemies[0].getName() + " has " + std::to_string(enemies[0].getHP()) + " HP left. Please use !rpg stats to monitor your Digimon!");
					return;
				}
				else {
					client.sendMessage(ch, specials + enrage + enemies[0].getName() + " has attacked everyone in the raid and has defeated: " + defeatedDigimon + ". " +
						enemies[0].getName() + " has " + std::to_string(enemies[0].getHP()) + " HP left. Please use !rpg stats to monitor your Digimon!");
					return;
				}
			}
		}
		////////////////PLAYER WIN//////////////////////
		if (enemies[0].getHP() <= 0)
		{
			bRaidAnnounced = false;
			bRaidStarted = false;
			bBattle = false;
			bTurnPrepared = false;
			raidTimer.stop();
			raidTimer.start();
			//enemies[0].HP = enemies[0] + 10 * raider->getDigimon().getRank() * 6
			for(auto& raider : raidParticipants)
			{
				////////////////////
				/*
				if (Math.Abs(raider->getDigimon().getRank() - enemies[0].getRank()) <= 1)
				{
				int newBits = (int)((enemies[0].getMaxHP() * ((double)(raider->getDigimon().getRank()) / (double)(enemies[0].getRank())) / 100));
				raider->addBITs(newBits);
				raider->getDigimon().incDVPoints(rewardPoints);
				if (rng.Next(0, Math.Abs(raider->getDigimon().getRank() - enemies[0].getRank() + 2)) == 1)
				raider->getDigimon().incMaxHP((int)Math.Ceiling(enemies[0].getRank() / raider->getDigimon().getRank() * (0.04 * raider->getDigimon().getMaxHP())));

				if (rng.Next(0, Math.Abs(raider->getDigimon().getRank() - enemies[0].getRank() + 2)) == 1)
				raider->getDigimon().incMaxMP((int)Math.Ceiling(enemies[0].getRank() / raider->getDigimon().getRank() * (0.04 * raider->getDigimon().getMaxMP())));

				if (rng.Next(0, Math.Abs(raider->getDigimon().getRank() - enemies[0].getRank() + 2)) == 1)
				raider->getDigimon().incSpd((int)Math.Ceiling((enemies[0].getRank() / raider->getDigimon().getRank() * (0.04 * raider->getDigimon().getSpd()))));

				if (rng.Next(0, Math.Abs(raider->getDigimon().getRank() - enemies[0].getRank() + 2)) == 1)
				raider->getDigimon().incStr(2 * raider->getDigimon().getRank());

				if (rng.Next(0, Math.Abs(raider->getDigimon().getRank() - enemies[0].getRank() + 5)) == 1)
				raider->getDigimon().incDef(1 * raider->getDigimon().getRank());
				}
				else
				{
				//////////// raider->addBITs(5);
				raider->addBITs(1);
				}
				*/
				raider->addBITs(1);
			}
			RAIDLOG << "----------------END RAID----------------\n";
			std::string stringDigivolveTemp = "";
			for(auto& raider : raidParticipants)
			{
				checkDigivolution(raider);

			}
			raidParticipants.clear();
			digimonDead.clear();
			enemies[0].setHP(enemies[0].getMaxHP());
			enemies[0].setMP(enemies[0].getMaxMP());


			//enemies[0].HP += 10 * raider->getDigimon().getRank() * 6;
			bot.save();
			/*bot.sendMessage(ch, "Congratulations! " + enemies[0].getName() + " has been defeated! Everyone has been rewarded " +
			rewardPoints + " points! Great job, Tamers! " + stringDigivolveTemp);*/
			////////client.sendMessage(ch.ID, "Congratulations! " + enemies[0].getName() + " has been defeated! Everyone has been rewarded " + X + " points! Great job, Tamers! " + stringDigivolveTemp);
			client.sendMessage(ch, "Congratulations! " + enemies[0].getName() + " has been defeated! Everyone has been rewarded points! Great job, Tamers! " + stringDigivolveTemp);
			//BETA
			resetEnemies();
			for(auto& entry : players)
			{
				entry.second.getDigimon().setHP(entry.second.getDigimon().getMaxHP());
				entry.second.getDigimon().setMP(entry.second.getDigimon().getMaxMP());
			}
			client.sendMessage(ch, "All players have been restored for Beta Testing!");
			return;
		}
		/**********************PREPARE TURN********************/
		if (!bTurnPrepared && bRaidStarted && enemies[0].getHP() > 0)
		{
			waitingRaiders.clear();
			for(auto& raider : raidParticipants)
			{
				waitingRaiders.push_back(raider);
			}
			battleChoice.clear();
			bTurnPrepared = true;
			turnTimer.start();
			client.sendMessage(ch, "What would you like to do, raiders? (1)Attack (2)Special");
			return;
		}
		if (raidTimer.getTimeInSec() > 60 * spawnTime && !bRaidAnnounced)
		{
			turnCount = 0;
			digimonDead.clear();
			bRaidAnnounced = true;
			string difficulty;
			//int randDigi = rng.Next(0, enemies.Count - 1);
			int n = enemies.size();
			std::vector<int> randVec(n);
			std::iota(std::begin(randVec), std::end(randVec), 0);
			std::shuffle(randVec.begin(), randVec.end(), rng);

			switch (enemies[0].getRank())
			{
			case 1:
				difficulty = "(Difficulty: Rookie)";
				break;
			case 2:
				difficulty = "(Difficulty: Champion)";
				break;
			case 3:
				difficulty = "(Difficulty: Ultimate)";
				break;
			case 4:
				difficulty = "(Difficulty: Mega)";
				break;
			default:
				difficulty = "(Difficulty: Unknown)";
				break;
			}

			for(auto& p : subscribedPlayers)
			{
				if (std::stoll(p->getUniqueDiscordID()) < 1000) //Forgot what this was for.
				{
					continue;
				}
				////////////serv.GetUser(p->getUniqueDiscordID()).SendMessage("Raid starting soon! Head over to #rpg with fellow Digidestined!");
				////////////serv.GetUser(p->getUniqueDiscordID()).SendMessage("To unscribe to these messages, type \"!rpg unsubscribe\" in the #rpg channel.");
				auto dm = client.createDirectMessageChannel(p->getUniqueDiscordID());
				std::string dmChID = dm.header["id"];
				client.sendMessage(dmChID, "Raid starting soon!Head over to #rpg with fellow Digidestined!");
				client.sendMessage(dmChID, "To unscribe to these messages, type \"!rpg unsubscribe\" in the #rpg channel.");
				/*Dictionary<string, ushort> findUsers = new Dictionary<string, ushort>();
				string fullName = p.getDiscordName();
				string name = "";
				ushort disc = 0;
				for (int i = 0; i < fullName.Length; i++)
				{
				if (i == fullName.Length - 1) //Cant find #
				{
				continue;
				}
				if (fullName[i] == '#')
				{
				string temp = fullName.Substring(i + 1, fullName.Length - name.Length - 1);
				disc = UInt16.Parse(temp);
				findUsers.Add(name, disc);
				continue;
				}
				name += fullName[i];
				}
				foreach (KeyValuePair<string, ushort> entry in findUsers)
				{

				//serv.GetUser(entry.Key, entry.Value).SendMessage("Raid starting soon! Head over to #rpg with fellow Digidestined!");
				//serv.GetUser(entry.Key, entry.Value).SendMessage("To unscribe to these messages, type \"!rpg unsubscribe\" in the #rpg channel.");
				}
				*/
			}

			client.sendMessage(ch, "***RAID STARTING IN " + std::to_string(beginTime / 60) + " MINUTES! Type \"!rpg join\" to help save the Digital World!*** " + difficulty);
			return;
		}
		//Start the raid
		else if (raidTimer.getTimeInSec() > (60 * spawnTime + beginTime) && bRaidAnnounced && !bRaidStarted)
		{
			bRaidStarted = true;
			RAIDLOG << "----------------START RAID----------------\n";
			RAIDLOG << "ENEMY: " << enemies[0].getName() << "\n";

			//Change the Enemy's stats depending on the number of Raiding participants
			/*enemies[0].HP += ((int)(raidParticipants.size() / 5))*(enemies[0].HP * 0.15);*/
			for(auto& raider : raidParticipants)
			{
				enemies[0].incHP(10 * raider->getDigimon().getRank() * 6);
			}
			rewardPoints = enemies[0].getMaxHP() / 100;
			enemies[0].incStr(((int)(raidParticipants.size() / 10)));
			enemies[0].incDef(((int)(raidParticipants.size() / 10)));

			RAIDLOG << "HP: " + enemies[0].getHP() << "\n";
			RAIDLOG << enemies[0].getStr() << "\n";
			RAIDLOG << enemies[0].getDef() << "\n";

			RAIDLOG << "NO. OF RAIDERS: " << raidParticipants.size() << "\n";
			for(auto& raider : raidParticipants)
			{
				RAIDLOG << raider->getName(); //Display all raiders to log
			}
			RAIDLOG << " \n";
			client.sendMessage(ch, enemies[0].getName() + " has arrived! Prepare for battle, DigiDestined!");
			client.uploadFile(ch,"data/pictures/" + enemies[0].getPic(), "");
				return;
		}
		else if (bRaidStarted)
		{
			//Destroy raid if there are no raiders or if everyone is dead.
			if (raidParticipants.size() == 0 || digimonDead.size() >= raidParticipants.size())
			{
				bRaidAnnounced = false;
				bRaidStarted = false;
				raidTimer.stop();
				raidTimer.start();
				turnTimer.stop();
				bBattle = false;
				bTurnPrepared = false;
				raidParticipants.clear();
				digimonDead.clear();
				RAIDLOG << "----------------END RAID----------------\n";
				client.sendMessage(ch, "The Digital World has been destroyed! Maybe next time, Tamers!");

				///BETA
				resetEnemies();
				for(auto& entry : players)
				{
					entry.second.getDigimon().setHP(entry.second.getDigimon().getMaxHP());
					entry.second.getDigimon().setMP(entry.second.getDigimon().getMaxMP());
				}
				client.sendMessage(ch, "All players have been restored for Beta Testing!");
				return;
			}
			// else
			// return raidUpdate(message, points);
		}
	}
	void RPG::checkDigivolution(Player* raider)
	{
		if (raider->getDigimon().getName() == "Salamon")
		{
			if (raider->getDigimon().getDVPoints() >= 75)
			{
				digivolve(raider);
				return;
			}
			else return;
		}
		else if (raider->getDigimon().getDVPoints() >= 50 * (int)(exp(raider->getDigimon().getRank())))
		{
			digivolve(raider);
			return;
		}
		else return;
	}
	void RPG::digivolve(Player* player)
	{
		std::ifstream RPGFILE;
		RPGFILE.open("data/digivolution.txt");           

		string oldDigimon = "";
		string newDigimon = "";
		string special = "";
		while (!RPGFILE.eof())
		{
			getline(RPGFILE, oldDigimon);
			if (oldDigimon == player->getDigimon().getName())
			{

				getline(RPGFILE, special);
				getline(RPGFILE, newDigimon);
				getline(RPGFILE, special);
				player->getDigimon().setName(newDigimon);
				player->getDigimon().setMaxHP(player->getDigimon().getMaxHP() * 2);
				player->getDigimon().setHP(player->getDigimon().getMaxHP());
				player->getDigimon().setMaxMP(player->getDigimon().getMaxMP() * 2);
				player->getDigimon().setMP(player->getDigimon().getMaxMP());
				player->getDigimon().setSpeed(player->getDigimon().getSpd() * 2);
				player->getDigimon().setStr(player->getDigimon().getStr() * 2);
				player->getDigimon().setDef(player->getDigimon().getDef() * 2);
				player->getDigimon().setRank(player->getDigimon().getRank() + 1);
				player->getDigimon().setDVPoints(0);
				player->getDigimon().setSpecial(special);
				break;
			}
		}
		RPGFILE.close();
		bot.save();
		client.sendMessage(ch, "***" + player->getName() + "'s " + oldDigimon + " has digivolved to " + player->getDigimon().getName() + "!!!***");
		return;
	}
	void RPG::resetEnemies()
	{
		std::ifstream enemiesFILE;
		enemiesFILE.open("data/enemy.txt");
			while (!enemiesFILE.eof())
			{
				std::vector<std::string> digiInfo;
				string temp = "";
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //Name
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //HP
				digiInfo.push_back(temp); //MaxHP
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //MP
				digiInfo.push_back(temp); //MaxMP
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //Str
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //Def
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //Speed
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //Rank
				getline(enemiesFILE, temp);
				digiInfo.push_back("0"); //DV INFO
				digiInfo.push_back(temp); //Special
				getline(enemiesFILE, temp);
				digiInfo.push_back(temp); //Type
				enemies.emplace_back(digiInfo);
			}
		return;
	}
	
}