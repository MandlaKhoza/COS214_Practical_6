#include "TimeStamp.h"
#include <ctime>
#include <sstream>
#include <iomanip>

std::string getCurrentTimestamp(){

    std::time_t now = std::time(NULL);
    std::tm* localTime = std::localtime(&now);

    std::stringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}