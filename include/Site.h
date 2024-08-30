#ifndef SITE_H
#define SITE_H

class Site
{
public:
  static constexpr int kMinMiningMinutes = 60;  // Minimum amount of mining druation (1 hr) in minutes
  static constexpr int kMaxMiningMinutes = 300; // Maximum amount of mining druation (5 hr) in minutes

  // Public static member functions
  static int getRandomMinedDuration(); // Utility function to generate a random number between min (60 mins) and max (300 mins).
                                       // Use this function to get a random number of mining time at the sites.
};

#endif