#include <random>

#include "Site.h"

constexpr int MIN_MINING_MINUTES = 60;  // Minimum amount of mining druation (1 hr) in minutes
constexpr int MAX_MINING_MINUTES = 300; // Maximum amount of mining druation (5 hr) in minutes

// Utility function to generate a random number between min (1hr) and max (5hr). We use this function to get a random duration
// of mining time at the sites.
int Site::getRandomMinedDuration()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(MIN_MINING_MINUTES, MAX_MINING_MINUTES);
    return dis(gen);
}