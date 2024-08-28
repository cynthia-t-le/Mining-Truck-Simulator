#ifndef SITE_H
#define SITE_H

class Site
{
public:
  static constexpr int MIN_MINING_MINUTES = 60;  // Minimum amount of mining druation (1 hr) in minutes
  static constexpr int MAX_MINING_MINUTES = 300; // Maximum amount of mining druation (5 hr) in minutes

  // Constructor
  Site() {};

  // Public member functions
  int getRandomMinedDuration(); // Utility function to generate a random number between min (60 mins) and max (300 mins).
                                // Use this function to get a random duration of mining time at the sites.
};

#endif