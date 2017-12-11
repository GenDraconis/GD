#include "VeeBot.h"

namespace _VEEBOT_ {
	VeeBot::VeeBot(std::string token) :
		SleepyDiscord::DiscordClient(token)
		//client(token, 3)
	{
		std::cout << "Starting Veebot\n";

		//Read in Starter Digimon Information
		std::ifstream digimonFILE;
		digimonFILE.open("data/starter.txt");

		while (digimonFILE.is_open() && !digimonFILE.eof())
		{
			std::vector<std::string> digimonInfo;
			std::string temp;// = digimonFILE.ReadLine();
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //Name
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //HP
			digimonInfo.push_back(temp); //MaxHP
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //MP
			digimonInfo.push_back(temp); //MaxMP
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //Str
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //Def
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //Spd
			temp = "1";
			digimonInfo.push_back(temp); //Rank
			temp = "0";
			digimonInfo.push_back(temp); //DVPoints
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //Special
			getline(digimonFILE, temp);
			digimonInfo.push_back(temp); //Type
////			starterDigimon.emplace_back(digimonInfo);
		}
		digimonFILE.close();
		std::cout << "Loaded Digimon\n";
		//DigiDestinedToBe


		//Read in Player information
		std::ifstream playerFILE;
		playerFILE.open("data/players.txt");

		int tempUID = 0;
		while (playerFILE.is_open() && !playerFILE.eof())
		{
			/*
			KHeartz KHeartz#1475 12 34
			Veemon
			*/
			std::vector<std::string> playerInfo;
			for (int i = 0; i < 5; i++)
			{
				std::string tempPlayerInfo;
				getline(playerFILE, tempPlayerInfo);
				if (i == 2 && tempPlayerInfo == "0")
				{
					tempPlayerInfo = std::to_string(tempUID);
					playerInfo.push_back(tempPlayerInfo);
					tempUID++;
				}
				else {
					playerInfo.push_back(tempPlayerInfo);
				}
			}
			std::vector<std::string> digimonInfo;
			for (int i = 0; i < 12; i++)
			{
				std::string tempDigimonInfo;
				getline(playerFILE, tempDigimonInfo);
				digimonInfo.push_back(tempDigimonInfo);
			}
			//Player p = new Player(playerInfo, digimonInfo);
			if (playerInfo[0] == "") { break; }
			players.emplace(playerInfo[2], Player(playerInfo, digimonInfo));
			// playersByNormName.Add(p.getName(), p);
		}
		playerFILE.close();

		/*while (true)
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

		}*/
		// bot.Wait();
	}
	//void VeeBot::Bot_MessageReceived(SleepyDiscord::Message& message)
	
	void VeeBot::onMessage(SleepyDiscord::Message message)
	{
		//if (e.Message.IsAuthor)
		//	return;

		//Console.WriteLine("{0} - {1} said: {2}", e.Channel, e.User.Name, e.Message.Text);
		std::cout << message.author.username << " " << std::string(message.author.ID) << " said: " << message.content << "\n";
		if (message.content.size() > 0 && message.content[0] == '!')
		{
			commandMsg(message);

			//Event-specific commands
			if (trivia != nullptr && message.content.size() >= 7 && message.content.substr(0, 7) == "!trivia")
			{
				trivia->processCommand(message);
			}
			if (rpg != nullptr && message.content.size() >= 4 && message.content.substr(0, 4) == "!rpg")
			{
				rpg->processCommand(message);
			}
			if (pvp != nullptr && message.content.size() >= 4 && message.content.substr(0, 4) == "!pvp")
			{
				pvp->processCommand(message);
			}
		}
		else
		{
			//Non-commands (e.g. answers, etc...)
			if (trivia != nullptr)
			{
				trivia->processMsg(message);
			}
			if (rpg != nullptr)
			{
				rpg->processMsg(message);
				rpg->raidUpdate(message);
			}
			if (pvp != nullptr)
			{
				pvp->processMsg(message);
			}
			//Check to see if digiDestinedToBe is ready to choose
			if (digiDestinedToBe.size() > 0)
			{
				if (message.content != "1" && message.content != "2" && message.content != "3")
				{
					return;
				}
				int selection = std::stoi(message.content) - 1;
				std::vector<Digimon> D;
				auto pIt = digiDestinedToBe.find(message.author.ID);


				if (pIt != digiDestinedToBe.end())
				{
					//Player p = new Player(e.User);
					auto p = players.emplace(message.author.ID, Player(message.author.ID));

					//playersByNormName.Add(p.getName(), p);
					
					Digimon d = Digimon(D[selection]);
					p.first->second.setDigimon(d);
					message.reply(this, message.author.usernameAsMention() + ", please take good care of " + d.getName() + ".");
					digiDestinedToBe.erase(pIt);
					save();
				}
				else
				{

				}
			}
		}
	}
	void VeeBot::commandMsg(SleepyDiscord::Message& message)
	{
		//Turning on RPG
		if (message.content == "!rpg on") {
			if (rpg != nullptr)
			{
				message.reply(this, "RPG is already on!");
				return;
			}
			if (message.author.ID == masterSnowflake)
			{
				rpg = std::make_unique<RPG>(*this, *this, message.channelID, players);
				message.reply(this, "RPG has been turned on!");
			}
			else
			{
				message.reply(this, "Please notify @Kheartz#1467 to turn on RPG!");
			}
		}
		//Turning off RPG
		else if (message.content == "!rpg off")
		{
			if (rpg == nullptr)
			{
				message.reply(this, "RPG is already off!");
				return;
			}
			if (message.author.ID == masterSnowflake)
			{
				rpg->getRaidLog().close();
				rpg = nullptr;
				message.reply(this, "RPG has been turned off!");
			}
			else
			{
				message.reply(this, "Please notify @Kheartz#1467 to turn off RPG!");
			}
		}

		//Turning on Trivia
		else if (message.content == "!trivia on")
		{
			if (trivia != nullptr)
			{
				message.reply(this, "Trivia is already active.");
				return;
			}
			if (message.author.ID == masterSnowflake)
			{
				//e.Channel.SendMessage("Turning on Trivia...");
				trivia = std::make_unique<Trivia>(*this, *this, message.channelID, players);
				message.reply(this, "Trivia has been turned on!");
			}
			else
			{
				message.reply(this, "Please notify @Kheartz#1467 to turn on Trivia!");
			}
		}
		//Turning off Trivia
		else if (message.content == "!trivia off")
		{
			if (trivia == nullptr)
			{
				message.reply(this, "Trivia is already off!");
				return;
			}
			if (message.author.ID == masterSnowflake)
			{
				trivia = nullptr;
				message.reply(this,"Trivia has been turned off!");
			}
			else
			{
				message.reply(this, "Please notify @Kheartz#1467 to turn off Trivia!");
			}
		}
		else if (message.content == "!pvp on")
		{
			if (pvp != nullptr)
			{
				message.reply(this, "PVP is already active.");
				return;
			}
			if (message.author.ID == masterSnowflake)
			{
				//e.Channel.SendMessage("Turning on Trivia...");
				//PVP TEMPORARY SPACE;
				pvp = std::make_unique<PVP>(*this, *this, message.channelID, players);
				//PVP(VeeBot& _bot, SleepyDiscord::DiscordClient& _client, SleepyDiscord::Channel& _ch, SleepyDiscord::Server& _serv, std::map<SleepyDiscord::Snowflake<SleepyDiscord::User>, Player>& _players)
				message.reply(this, "PVP has been turned on!");
			}
			else
			{
				message.reply(this, "Please notify @Kheartz#1467 to turn on PVP!");
			}
		}
		else if (message.content == "!pvp off")
		{
			//
		}
		else if (message.content.size() >= 4 && message.content.substr(0, 4) == "!rpg" && rpg != nullptr)
		{
			rpg->processMsg(message);
		}
		else if (message.content.size() >= 7 && message.content.substr(0, 7) == "!trivia" && trivia != nullptr)
		{
			trivia->processMsg(message);
		}
		else if (message.content == "!save")
		{
			message.reply(this, "Saving Data...");
			save();
		}
		else if (message.content == "!name")
		{
			
			auto pIt = players.find(message.author.ID);

			if (pIt != players.end())
			{
				message.reply(this, "You are registered and your name is " + pIt->second.getName() + ".");
			}
			else {
				message.reply(this, "You are not registered. To register with the bot, please type \"!register\"");
			}
		}
		else if (message.content == "!bits")
		{
			auto pIt = players.find(message.author.ID);
			if (pIt != players.end())
			{
				message.reply(this, message.author.usernameAsMention() + ", you have " + std::to_string(pIt->second.getBits()) + " BITs.");
			}
			else {
				message.reply(this, "You are not registered. To register with the bot, please type \"!register\"");
			}
		}
		else if (message.author.ID == "!register")
		{

			string name = "";
			for (unsigned int i = 0; i < message.author.username.size(); i++)
			{
				char c = message.author.username[i];
				if (c == '#')
				{
					break;
				}
				name += c;
			}
			//name = name.ToLower();
			auto pIt = players.find(message.author.ID);
			if (pIt != players.end())
			{
				//If name is in database and discord name is registered
				message.reply(this, message.author.usernameAsMention() + ", you are already fully registered with bot.");
				
				//If name is in database but discord name is not registered
				/*else
				{
					P.setDiscordName(e.User.ToString());
					e.Channel.SendMessage(e.User.Mention + ", your Discord is now linked!");
					players.Remove(P.getUniqueDiscordID());
					playersByNormName.Remove(name);
					players.insert(e.User.Id, P);
					playersByNormName.Add(name, P);
					save(players);
				}*/
			}
			else {
				std::vector<Digimon> choiceList;
				auto pIt = digiDestinedToBe.find(message.author.ID);
				if (pIt != digiDestinedToBe.end())
				{
					string msgg = "You have already been selected as digidestined," + message.author.usernameAsMention() + ". Please select your digimon: ";
					msgg += "(1) " + choiceList[0].getName() + ", ";
					msgg += "(2) " + choiceList[1].getName() + ", ";
					msgg += "(3) " + choiceList[2].getName() + ".";
					message.reply(this, msgg);
					return;
				}
				//Randomize Digimon
				int n = starterDigimon.size();
				std::vector<int> randVec(n);
				std::iota(std::begin(randVec), std::end(randVec), 0);
				std::shuffle(randVec.begin(), randVec.end(), rng);
				
				choiceList.push_back(starterDigimon[randVec[0]]);
				choiceList.push_back(starterDigimon[randVec[1]]);
				choiceList.push_back(starterDigimon[randVec[2]]);
				string msg = "Welcome to the Digital World, " + message.author.usernameAsMention() + ". Please select your digimon: ";
				msg += "(1) " + choiceList[0].getName() + ", ";
				msg += "(2) " + choiceList[1].getName() + ", ";
				msg += "(3) " + choiceList[2].getName() + ".";
				digiDestinedToBe[message.author.ID] = choiceList;
				message.reply(this, msg);
			}
		}
		else if (message.content == "!users" && message.author.ID == masterSnowflake)
		{
			//SleepyDiscord::DMChannel dm = createDirectMessageChannel(message.author.ID);
			//SleepyDiscord::Snowflake<SleepyDiscord::Channel> dmChID = dm.ID;
			//sendMessage(dmChID, "Raid starting soon! Head over to #rpg with fellow Digidestined!");
			uploadFile(message.channelID, "D:/Desktop/Dev/Projects/VeeBot++/VeeBot++/data/pic.png", "test");
			//return;
			//std::map<std::string, unsigned short int> findUsers;
			
			//e.Server.GetUser("Binex", 6701).SendMessage("Hello there!");

			/*for(auto& entry : players)
			{
				string fullName = entry.second.getDiscordName();
				string name = "";
				unsigned short disc = 0;
				for (unsigned int i = 0; i < fullName.size(); i++)
				{
					if (i == fullName.size() - 1) //Cant find #
					{
						continue;
					}
					if (fullName[i] == '#')
					{
						string temp = fullName.substr(i + 1, fullName.size() - name.size() - 1);
						disc = std::stoi(temp);
						findUsers[name] = disc;
						continue;
					}
					name += fullName[i];
				}
			}
			for(auto& entry : findUsers)
			{
				message.reply(this, entry.first + "#" + std::to_string(entry.second));
			}
			message.reply(this, "Sending message to you, " + message.author.usernameAsMention());*/
		}
		else if (message.content == "!linkID" && message.author.ID == masterSnowflake)
		{
			////linkID(e, players);
		}
		return;
	}
	void VeeBot::save()
	{
		std::string file = "data/players.txt";
		std::ifstream  saveFILE;
		saveFILE.open(file);
		//foreach(KeyValuePair<ulong, Player> player in _players)
		for (auto& player : players)
		{
			//Console.WriteLine(player.Value.getName() + " " + player.Value.getDiscordName() + " " + player.Value.getBits() + " " + player.Value.getTriviaPoints());
			// saveFILE.WriteLine(player.Value.getName() + " " + player.Value.getDiscordName() + " " + player.Value.getBits() + " " + player.Value.getTriviaPoints());
			std::cout << player.second.getName();
			std::cout << player.second.getDiscordName();
			std::cout << std::string(player.second.getUniqueDiscordID());
			std::cout << player.second.getBits();
			std::cout << player.second.getTriviaPoints();
			std::cout << player.second.getDigimon().getName();
			std::cout << player.second.getDigimon().getHP();
			std::cout << player.second.getDigimon().getMaxHP();
			std::cout << player.second.getDigimon().getMP();
			std::cout << player.second.getDigimon().getMaxMP();
			std::cout << player.second.getDigimon().getStr();
			std::cout << player.second.getDigimon().getDef();
			std::cout << player.second.getDigimon().getSpd();
			std::cout << player.second.getDigimon().getRank();
			std::cout << player.second.getDigimon().getDVPoints();
			std::cout << player.second.getDigimon().getSpecial();
			std::cout << player.second.getDigimon().getType();
		}
		saveFILE.close();
	}
	/*void VeeBot::linkID(const SleepyDiscord::Message& message, std::map<unsigned long long int, Player>& _players)
	{
		if (e.User.Id == std::atoll(masterSnowflake))
		{
			for (auto& user : message.Server.users)
			{
				if (_players.ContainsKey(user.Id))
				{
					continue;
				}
				else
				{
					foreach(var player in _players)
					{
						if (user.ToString() == player.Value.getDiscordName())
						{
							player.Value.setUniqueDiscordID(user.Id);
							e.Channel.SendMessage(player.Value.getDiscordName() + " has been newly linked.");
						}
					}
				}
			}
			e.Channel.SendMessage("Server-wide link process has been completed.");
		}
		else
		{
			if (_players.ContainsKey(e.User.Id)) {
				e.Channel.SendMessage(e.User.ToString() + " you are already registered and linked.");
			}
			else {
				foreach(var player in _players)
				{
					if (player.ToString() == e.User.ToString())
					{
						player.Value.setUniqueDiscordID(e.User.Id);
						break;
					}
					e.Channel.SendMessage(e.User.ToString() + " you are now linked.");
				}
			}
		}
		// Console.WriteLine(_players["KHeartz#1467"].getUniqueDiscordID());
		save(_players);
	}*/
}