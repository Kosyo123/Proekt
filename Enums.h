#pragma once
#include <string>

enum class Priority { LOW, MEDIUM, HIGH };
enum class Status   { TODO, IN_PROGRESS, DONE };

std::string priorityToString(Priority p);
std::string statusToString(Status s);
int         priorityValue(Priority p);
