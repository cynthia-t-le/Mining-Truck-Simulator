// Truck.h
#ifndef TRUCK_H
#define TRUCK_H

#include <vector>

class Truck
{
public:
  enum State
  {
    MINING,
    TRAVEL_TO_UNLOAD_STATION,
    UNLOADING,
    TRAVEL_TO_MINING_SITE
  };

  // Constructor
  explicit Truck(const int id) : m_id(id), m_currentState(MINING), m_currentMiningTime(0), m_currentMinedHelium(0),
                                 m_currentTripQueueWait(0), m_totalMinedHelium(0), m_totalMiningTime(0),
                                 m_totalUnloadedTrips(0), m_isInDataQueue(false), m_totalQueueWait(0) {}

  // Public member functions
  int getId() const { return m_id; }

  State getCurrentState() const { return m_currentState; }

  void setCurrentState(const State newState) { m_currentState = newState; }

  int getCurrentMiningTime() const { return m_currentMiningTime; }

  void setCurrentMiningTime(const int time) { m_currentMiningTime = time; }

  int getCurrentMinedHelium() { return m_currentMinedHelium; }

  void setCurrentMinedHelium(const int value) { m_currentMinedHelium = value; }

  int getCurrentTripQueueWait() { return m_currentTripQueueWait; }

  void setCurrentTripQueueWait(const int value) { m_currentTripQueueWait = value; }

  int getTotalMiningTime() const { return m_totalMiningTime; }

  void setTotalMiningTime(const int time) { m_totalMiningTime = time; }

  int getTotalMinedHelium() const { return m_totalMinedHelium; }

  void setTotalMinedHelium(const int value) { m_totalMinedHelium = value; }

  int getTotalNumberUnloads() const { return m_totalUnloadedTrips; }

  void incrementTotalNumberUnloads() { m_totalUnloadedTrips++; }

  int getTotalQueueWait() const { return m_totalQueueWait; }

  void setTotalQueueWait(const int value) { m_totalQueueWait = value; }

  bool getIsInDataQueue() const { return m_isInDataQueue; }

  void setIsInDataQueue(const bool flag) { m_isInDataQueue = flag; }

  void saveMiningDuration(const int value) { m_miningDurations.push_back(value); }

  double calculateAverageQueueTime(const int totalQueueWait, const int totalElapsedTime) const;

  double convertAverageQueueTimeToPercent(const double averageQueueTime) const;

  int calculateTotalMiningDuration() const;

  double convertTruckEfficiencyToPercent(const double truckEfficiency) const;

private:
  int m_id;                           // Truck id
  State m_currentState;               // Current truck state
  int m_currentMiningTime;            // Current mining trip time in minutes
  int m_currentMinedHelium;           // Helium mined in current trip
  int m_currentTripQueueWait;         // Current time spent waiting in the queue at an unloading station
  int m_totalMiningTime;              // Total time spent mining at the sites in mins
  int m_totalMinedHelium;             // Total helium mined so far (eg., 1 minute = 1 unit of helium)
  int m_totalUnloadedTrips;           // Total number of unload trips
  int m_totalQueueWait;               // Total number of time truck spent waiting in the queue (eg., 1 count = 1 min)
  bool m_isInDataQueue;               // Let us know if this truck is in the shared data queue waiting to be processed by station
  std::vector<int> m_miningDurations; // for statistic purposes
};

#endif