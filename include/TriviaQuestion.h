#pragma once

#include <string>

namespace _VEEBOT_ {
	class TriviaQuestion {
	private:
		std::string category;
		std::string question;
		std::string answer;
		short unsigned int pointsAdded;
	public:
		TriviaQuestion(std::string _category, std::string _question, std::string _answer){
			category = _category;
			question = _question;
			answer = _answer;
			if (category.substr(category.size() - 6, 6) == "(easy)")
			{
				pointsAdded = 1;
			}
			else if (category.substr(category.size() - 8, 8) == "(medium)")
			{
				pointsAdded = 2;
			}
			else if (category.substr(category.size() - 6, 6) == "(hard)")
			{
				pointsAdded = 3;
			}
			else {
				pointsAdded = 0;
			}
		}

		std::string getCategory() { return category; }
		std::string getQuestion() { return question; }
		std::string getAnswer() { return answer; }
		short unsigned int getPointsAdded() { return pointsAdded; }
	};
}