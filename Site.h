#ifndef SITE_H
#define SITE_H

class Site
{
public:
  static constexpr int MIN_MINING_MINUTES = 60;  // Minimum amount of mining druation (1 hr) in minutes
  static constexpr int MAX_MINING_MINUTES = 300; // Maximum amount of mining druation (5 hr) in minutes

  Site() {};

  int getRandomMinedDuration();
};

#endif