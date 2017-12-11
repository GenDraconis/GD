#include <fstream>
#include "VeeBot.h"


class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		if (message.startsWith("VeeBot")) {
			std::cout << message.content << "\n";
			sendMessage(message.channelID, "Hello " + message.author.username);
		}
	}
};


int main() {
	ifstream iFILE;
	iFILE.open("data/token.txt");
	std::string token;
	getline(iFILE, token);
	cout << token << endl;
	_VEEBOT_::VeeBot veebot(token);
	veebot.run();

	return 0;

}