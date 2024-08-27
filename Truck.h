// Truck.h
#ifndef TRUCK_H
#define TRUCK_H

#include <vector>

// Doxygen:  assume 1 minute mining = 1 unit of helium for all mining sites
class Truck
{
public:
  static constexpr int TRAVEL_TIME = 30;       // in mins
  static constexpr int UNLOAD_TIME = 5;        // in mins
  static constexpr int HELIUM_MINING_RATE = 1; // 1 helium/min

  enum State
  {
    MINING,
    TRAVEL_TO_UNLOAD_STATION,
    UNLOADING,
    TRAVEL_TO_MINING_SITE
  };

  Truck(const int id, const int time);

  int getId() const;

  int getCreationTime() const;

  State getCurrentState() const;

  void setCurrentState(const State newState);

  int getCurrentMiningTime() const;

  void setCurrentMiningTime(const int duration);

  int getCurrentMinedHelium();

  void setCurrentMinedHelium(const int value);

  int getCurrentTripQueueWait();

  void setCurrentTripQueueWait(const int value);

  int getTotalMiningTime() const;

  void setTotalMiningTime(const int time);

  int getTotalMinedHelium() const;

  void setTotalMinedHelium(const int value);

  int getTotalNumberUnloads() const;

  void incrementTotalNumberUnloads();

  int getTotalQueueWait() const;

  void setTotalQueueWait(const int value);

  double calculateAverageQueueTime(const int totalQueueWait, const int totalElapsedTime);

  void saveMiningDuration(const int value);

  int calculateTotalMiningDuration() const;

  int calculateMaximumHeliumPossible(const int miningDuration);

private:
  int m_id;                           // Truck id
  int m_creationTime;                 // Time difference between when simulation starts and when Truck object is created
  State m_currentState;               // Current truck state
  int m_currentMiningTime;            // Current mining trip time in minutes
  int m_currentMinedHelium;           // Helium mined in current trip
  int m_currentTripQueueWait;         // Current time spent waiting in the queue at an unloading station
  int m_totalMiningTime;              // Total time spent mining at the sites in mins
  int m_totalMinedHelium;             // Total helium mined so far (eg., 1 minute = 1 unit of helium)
  int m_totalUnloadedTrips;           // Total number of unload trips
  int m_totalQueueWait;               // Total number of time truck spent waiting in the queue (eg., 1 count = 1 min)
  std::vector<int> m_miningDurations; // for statistic purposes
};

#endif