#include "pch.h"
#include<gtest/gtest.h>
#include <iostream>
#include <tuple>
#include <optional>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

struct Person {

	std::string surname;
	std::string name;
	std::optional<std::string> patronymic;
};

std::ostream& operator<< (std::ostream& stream, Person& p)
{
	stream << p.surname << " " << p.name;
	if (p.patronymic)
		std::cout << " " << *p.patronymic;
	return stream;
}

bool operator< (const Person& p1, const Person& p2) {
	return std::tie(p1.surname, p1.name, p1.patronymic) < std::tie(p2.surname, p2.name, p2.patronymic);
}

bool operator== (const Person& p1, const Person& p2) {
	return std::tie(p1.surname, p1.name, p1.patronymic) == std::tie(p2.surname, p2.name, p2.patronymic);
}

struct PhoneNumber {

	int country_code;
	int city_code;
	std::string number;
	std::optional<int> add_number;

};

std::ostream& operator<<(std::ostream& stream, PhoneNumber& pn) {

	stream << "+" << pn.country_code << "(" << pn.city_code << ")" << pn.number;
	if (pn.add_number) {
		std::cout << " " << pn.add_number.value();
	}
	return stream;
}

bool operator< (const PhoneNumber& pn1, const PhoneNumber& pn2) {

	return std::tie(pn1.country_code, pn1.city_code, pn1.number, pn1.add_number) < std::tie(pn2.country_code, pn2.city_code, pn2.number, pn2.add_number);
}

bool operator== (const PhoneNumber& pn1, const PhoneNumber& pn2) {

	return std::tie(pn1.country_code, pn1.city_code, pn1.number, pn1.add_number) == std::tie(pn2.country_code, pn2.city_code, pn2.number, pn2.add_number);
}


class PhoneBook {

private:
	std::vector<std::pair<Person, PhoneNumber>> PnBook;

public:
	PhoneBook(std::ifstream& file) {
		try {
			if (!file.is_open()) {
				throw std::logic_error("Can't open the file");
			}
			else
			{
				const size_t SUBROWS = 7;
				std::string bString;

				while (!file.eof()) {
					getline(file, bString, '\n');

					std::vector<std::string> vString;
					size_t iStart = 0;

					for (size_t i = 0; i < SUBROWS; i++)
					{
						size_t iEnd = bString.find(',', iStart);
						std::string pairString = bString.substr(iStart, iEnd - iStart);

						if (pairString.empty())
							pairString = "";

						iStart = iEnd + 1;
						vString.push_back(pairString);
					}

					Person somePerson;
					somePerson.surname = vString[0];
					somePerson.name = vString[1];
					somePerson.patronymic = vString[2];

					PhoneNumber somePhoneNum;
					somePhoneNum.country_code = std::stoi(vString[3]);
					somePhoneNum.city_code = std::stoi(vString[4]);
					somePhoneNum.number = vString[5];

					std::stringstream stream;
					int add_number;
					stream << vString[6];
					stream >> add_number;

					if (add_number < 0) {
						somePhoneNum.add_number = std::nullopt;
					}
					else {
						somePhoneNum.add_number = add_number;
					}

					PnBook.push_back(std::pair<Person, PhoneNumber>(somePerson, somePhoneNum));

				}
			}
		}
		catch (std::logic_error& Error) {
			std::cout << Error.what() << std::endl;
		}

		file.close();
	}

	void SortByName() {
		std::sort(PnBook.begin(), PnBook.end(),
			[](std::pair<Person, PhoneNumber>& one, std::pair<Person, PhoneNumber>& two) {
				return one.first < two.first;
			});
	}

	void SortByPhone() {
		std::sort(PnBook.begin(), PnBook.end(),
			[](std::pair<Person, PhoneNumber>& one, std::pair<Person, PhoneNumber>& two) {
				return one.second < two.second;
			});
	}

	std::tuple<std::string, PhoneNumber> GetPhoneNumber(std::string surName) {

		PhoneNumber pSearch;
		std::string answer;
		size_t counter = 0;

		for (size_t i = 0; i < PnBook.size(); i++) {
			if (PnBook[i].first.surname == surName) {
				counter++;
				pSearch = PnBook[i].second;
			}
		}
		if (counter == 1) {
			answer.clear();
		}
		if (counter == 0) {
			answer = "not found";
		}

		if (counter > 1)
		{
			answer = "found more than 1";
		}
		return std::tuple<std::string, PhoneNumber>(answer, pSearch);
	}

	void ChangePhoneNumber(Person person, PhoneNumber number) {

		for (size_t i = 0; i < PnBook.size(); i++)
		{
			if (PnBook[i].first == person)
			{
				PnBook[i].second = number;
			}
		}
	}

	friend std::ostream& operator<<(std::ostream& os, PhoneBook pBook);
};

std::ostream& operator<<(std::ostream& os, const PhoneBook pBook) {
	os << "\n";
	for (size_t i = 0; i < pBook.PnBook.size(); i++)
	{
		Person person = pBook.PnBook[i].first;
		PhoneNumber number = pBook.PnBook[i].second;

		os << person << " " << number << "\n";
	}
	return os;

}

std::ifstream file("PhoneBook1.txt");

class PhoneBookTest : public ::testing::Test {
protected:
	PhoneBook* pnbook;
	PhoneBookTest() {
		pnbook = new PhoneBook(file);
	}
	~PhoneBookTest() {
		delete pnbook;
	}
};

TEST_F(PhoneBookTest, Test1)
{
	pnbook->SortByPhone();
	SUCCEED();
}

TEST_F(PhoneBookTest, Test2)
{
	pnbook->SortByName();
	SUCCEED();
}

TEST_F(PhoneBookTest, Test3)
{
	pnbook->ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" },
		PhoneNumber{ 7, 123, "15344458", std::nullopt });
	SUCCEED();
}

TEST_F(PhoneBookTest, Test4)
{
	pnbook->ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" },
		PhoneNumber{ 16, 465, "9155448", 13 });
	SUCCEED();

}

TEST_F(PhoneBookTest, Test5)
{
	pnbook->GetPhoneNumber("Mironova");
	SUCCEED();

}

int main(int argc, char** argv) {

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
