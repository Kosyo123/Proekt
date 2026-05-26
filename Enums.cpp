#include "Enums.h"

std::string priorityToString(Priority p) {
    switch (p) {
        case Priority::LOW:    return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH:   return "HIGH";
    }
    return "UNKNOWN";
}

std::string statusToString(Status s) {
    switch (s) {
        case Status::TODO:        return "TODO";
        case Status::IN_PROGRESS: return "IN_PROGRESS";
        case Status::DONE:        return "DONE";
    }
    return "UNKNOWN";
}

int priorityValue(Priority p) {
    switch (p) {
        case Priority::LOW:    return 0;
        case Priority::MEDIUM: return 1;
        case Priority::HIGH:   return 2;
    }
    return 0;
}
