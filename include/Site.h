#ifndef SITE_H
#define SITE_H

class Site
{
public:
  static constexpr int kMinMiningMinutes = 60;  // Minimum amount of mining druation (1 hr) in minutes
  static constexpr int kMaxMiningMinutes = 300; // Maximum amount of mining druation (5 hr) in minutes

  /**
   * @brief Generate a random number.
   *
   * This function will generate a random number between the values
   * 60 and 300 to represent the mining duration.
   *
   * @return Randomly generated number between 60 and 300.
   */
  static int getRandomMinedDuration();
};

#endif