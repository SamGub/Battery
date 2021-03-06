#include "stdafx.h"
#include <string>

#include <windows.h>
#include <iostream>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define UNKNOWN         0xFFFFFFFF		// used for the battery status flag--the "unknown" value

int64_t FormatHours(int64_t minutesNow);
std::string FormatMinutes(int64_t minutesNow);
std::string FormatSeconds(int64_t minutesNow);
std::string GetBatteryStatus(byte batteryFlag);
void OutputStatus(boost::posix_time::ptime* timeNow,
							 SYSTEM_POWER_STATUS* status,
							 const char* dayName);

const int SECONDS_ENDS_WITH_ZERO = 10;
const int SECOND_BEFORE_ZERO_ENDING = 9;
const int ONE_SECOND = 1000;
const int TEN_SECONDS = 10000;
const int DOUBLE_DIGITS = 10;
const int NOON = 12;

const int SECONDS_IN_ONE_MINUTE = 60;

const char LEADING_ZERO = '0';

int main() {
	SYSTEM_POWER_STATUS status;

	
	const char MONTH_NAMES[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
									  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	const char DAY_NAMES[7][6] = { "Sun", "Mon", "Tues", "Weds", "Thurs", "Fri", "Sat"};

	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

	const char* month = MONTH_NAMES[now.date().month().as_number()];
	const char* dayName = DAY_NAMES[now.date().day_of_week()];
	while ((now.time_of_day().seconds() % SECONDS_ENDS_WITH_ZERO) != SECOND_BEFORE_ZERO_ENDING) {
		// It's weird that this doesn't work with the zero-ending. Sometimes it does?

		std::system("cls");	// refresh the console
		OutputStatus(&now, &status, dayName);

		Sleep(ONE_SECOND);
	}
	while (true) {
		std::system("cls");	// refresh the console
		OutputStatus(&now, &status, dayName);

		Sleep(TEN_SECONDS);
	}
	return 0;
}

int64_t FormatHours(int64_t hoursNow) {
	int64_t hours;
	if (hoursNow > NOON) {
		hours = (hoursNow - NOON);
	}
	else {
		hours = hoursNow;
	}
	return hours;
}

std::string FormatMinutes(int64_t minutesNow) {
	std::string minutes;
	if (minutesNow < DOUBLE_DIGITS) {
		minutes = LEADING_ZERO + std::to_string(minutesNow);
	}
	else {
		minutes = std::to_string(minutesNow);
	}

	return minutes;
}

std::string FormatSeconds(int64_t secondsNow) {
	std::string seconds;
	if (secondsNow < DOUBLE_DIGITS) {
		seconds = LEADING_ZERO + std::to_string(secondsNow);
	}
	else {
		seconds = std::to_string(secondsNow);
	}

	return seconds;
}

std::string GetBatteryStatus(byte batteryFlag) {
	std::string status;
	switch (batteryFlag) {
		case 1: {
			status = "Not charging (high)";
			break;
		}
		case 2: {
			status = "Not charging (low)";
			break;
		}
		case 4: {
			status = "Not charging (critical)";
			break;
		}
		case 8: {
			status = "Charging";
			break;
		}
		case 9: {status = "Charging (high)";
			break;
		}
		case 10: {status = "Charging (low)";
			break;
		}
		case 12: {
			status = "Charging (crit)";
			break;
		}
		case 128: {
			status = "No system battery";
			break;
		}
		case 256: {
			status = "Unknown status";
			break;
		}
		default: {
			status = "Error";
			break;
		}
	}
	return status;
}

void OutputStatus(boost::posix_time::ptime* timeNow,
							 SYSTEM_POWER_STATUS* status,
							 const char* dayName) {

	*timeNow = boost::posix_time::second_clock::local_time();	// update the time.
	// this won't update the date for midnight.

	//t = std::time(NULL);
	//p = std::localtime(&t);	// deprecated because std::localtime() is not thread-safe

	GetSystemPowerStatus(&*status);
	int life = status->BatteryLifePercent;
	int secs = status->BatteryLifeTime;

	//time_t now = time(0);

	// convert now to string form
	//char* dt = ctime(&now);

	//time_t t = time(NULL);
	//struct tm * p = localtime(&t);
	int64_t hours = FormatHours(timeNow->time_of_day().hours());
	std::string minutes = FormatMinutes(timeNow->time_of_day().minutes());
	std::string seconds = FormatSeconds(timeNow->time_of_day().seconds());

	//strftime(s, 1000, "%A, %B %d %Y", p);
	std::cout << dayName << " "
		<< timeNow->date().month() << " "
		<< timeNow->date().day() << " "
		<< hours << ":"
		<< minutes << ":"
		<< seconds << "  ";

	std::cout << life << "%  ";

	std::cout << GetBatteryStatus(status->BatteryFlag);

	if (secs == UNKNOWN) {
		//cout << endl << "Amount of time remaining is unknown";
		//cout << " (?)";
	}
	//else cout << endl << secs << " seconds remaining";
	else std::cout << " (" << (secs / SECONDS_IN_ONE_MINUTE) << " mins)";
}