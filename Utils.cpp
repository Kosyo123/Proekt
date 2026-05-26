#include "Utils.h"

std::string today() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    std::ostringstream oss;
    oss << (1900 + t->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + t->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << t->tm_mday;
    return oss.str();
}

bool dateIsBefore(const std::string& a, const std::string& b) {
    return a < b;
}
