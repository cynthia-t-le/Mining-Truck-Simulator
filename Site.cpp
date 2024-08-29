#include <random>

#include "Site.h"

// --------------------------------------------------------
// Public Member Functions
// --------------------------------------------------------
int Site::getRandomMinedDuration()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(kMinMiningMinutes, kMaxMiningMinutes);
    return dis(gen);
}