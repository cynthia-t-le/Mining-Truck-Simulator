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
  /**
   * @brief Initialize Truck with default parameters.
   *
   * This function will initialize the Truck ID and initialize
   * class variables to default values.
   *
   * @param id Truck ID
   */
  explicit Truck(const int id) : m_id(id), m_currentState(MINING), m_currentMiningTime(0), m_currentMinedHelium(0),
                                 m_currentTripQueueWait(0), m_totalMinedHelium(0), m_totalMiningTime(0),
                                 m_totalUnloadedTrips(0), m_isInDataQueue(false), m_totalQueueWait(0) {}

  /**
   * @brief Get Truck's ID
   *
   * This function will return the Truck's ID.
   *
   * @return Truck's ID
   */
  int getId() const { return m_id; }

  /**
   * @brief Get Truck's current state.
   *
   * This function will return the current state of the Truck.
   *
   * @return Truck's current state
   */
  State getCurrentState() const { return m_currentState; }

  /**
   * @brief Set Truck's current state.
   *
   * This function will set the current state of the Truck.
   *
   * @param newState Truck's new state
   */
  void setCurrentState(const State newState) { m_currentState = newState; }

  /**
   * @brief Get Truck's current mining duration.
   *
   * This function will return the current mining duration of the Truck.
   *
   * @return Truck's current mining duration
   */
  int getCurrentMiningTime() const { return m_currentMiningTime; }

  /**
   * @brief Set Truck's current mining duration.
   *
   * This function will set the current mining duration of the Truck.
   *
   * @param time Truck's new mining duration
   */
  void setCurrentMiningTime(const int time) { m_currentMiningTime = time; }

  /**
   * @brief Get Truck's current mined helium.
   *
   * This function will return the current mined helium of the Truck.
   *
   * @return Truck's current mined helium
   */
  int getCurrentMinedHelium() { return m_currentMinedHelium; }

  /**
   * @brief Set Truck's current mined helium.
   *
   * This function will set the current mined helium of the Truck.
   *
   * @param value Truck's current mined helium
   */
  void setCurrentMinedHelium(const int value) { m_currentMinedHelium = value; }

  /**
   * @brief Get Truck's current wait time.
   *
   * This function will return the current time the Truck has
   * been waiting to be processed by a Station.
   *
   * @return Truck's current wait time at a Station
   */
  int getCurrentTripQueueWait() { return m_currentTripQueueWait; }

  /**
   * @brief Set Truck's current wait time.
   *
   * This function will set the current time the Truck has
   * been waiting to be processed by a Station.
   *
   * @param time Truck's current wait time at a Station
   */
  void setCurrentTripQueueWait(const int value) { m_currentTripQueueWait = value; }

  /**
   * @brief Get Truck's total mining time.
   *
   * This function will return the total time a Truck has
   * been mining.
   *
   * @return Truck's total mining time
   */
  int getTotalMiningTime() const { return m_totalMiningTime; }

  /**
   * @brief Set Truck's total mining time.
   *
   * This function will set the total time a Truck has
   * been mining.
   *
   * @param time Truck's total mining time
   */
  void setTotalMiningTime(const int time) { m_totalMiningTime = time; }

  /**
   * @brief Get Truck's total mined helium.
   *
   * This function will return the total helium a Truck has
   * mined.
   *
   * @return Truck's total mined helium
   */
  int getTotalMinedHelium() const { return m_totalMinedHelium; }

  /**
   * @brief Set Truck's total mined helium.
   *
   * This function will set the total helium a Truck has
   * mined.
   *
   * @param value Truck's total mined helium
   */
  void setTotalMinedHelium(const int value) { m_totalMinedHelium = value; }

  /**
   * @brief Get Truck's total successful unloads.
   *
   * This function will return the total number of times a Truck
   * has successfully unloaded at a Station.
   *
   * @return Truck's total successful unloads
   */
  int getTotalNumberUnloads() const { return m_totalUnloadedTrips; }

  /**
   * @brief Increment Truck's total successful unloads.
   *
   * This function will increment the total number of times a Truck
   * has successfully unloaded at a Station by 1.
   *
   */
  void incrementTotalNumberUnloads() { m_totalUnloadedTrips++; }

  /**
   * @brief Get Truck's total waiting time.
   *
   * This function will return the total time a Truck has
   * been waiting to be processed by a Station.
   *
   * @return Truck's total wait time at a Station
   */
  int getTotalQueueWait() const { return m_totalQueueWait; }

  /**
   * @brief Set Truck's total waiting time.
   *
   * This function will set the total time a Truck has
   * been waiting to be processed by a Station.
   *
   * @param value Truck's total wait time at a Station
   */
  void setTotalQueueWait(const int value) { m_totalQueueWait = value; }

  /**
   * @brief Get if Truck is waiting at a Station.
   *
   * This function will return true if a Truck is waiting in
   * the shared data vector to be processed by a Station. Otherwise,
   * it will return false.
   *
   * @return If Truck is waiting at a Station
   */
  bool getIsInDataQueue() const { return m_isInDataQueue; }

  /**
   * @brief Set if Truck is waiting at a Station.
   *
   * This function will set the Truck to be waiting in the
   * shared data vector to be processed by a Station.
   *
   * @param flag If Truck is in shared data vector waiting
   * to be processed by a Station
   */
  void setIsInDataQueue(const bool flag) { m_isInDataQueue = flag; }

  /**
   * @brief Save current mining duration.
   *
   * This function will save the randomly generated mining duration
   * to a vector to keep track of all the values produced.
   *
   * @param value Single mining duration of the Truck
   */
  void saveMiningDuration(const int value) { m_miningDurations.push_back(value); }

  /**
   * @brief Calculate average waiting time.
   *
   * This function will calculate the average time a Truck spent waiting
   * at a Station to be processed.
   *
   * @param totalQueueWait Total time Truck spent waiting
   * @param totalElapsedTime Total time the Truck spent mining
   * @return Average waiting time of the Truck
   */
  double calculateAverageQueueTime(const int totalQueueWait, const int totalElapsedTime) const;

  /**
   * @brief Converts value to a percentage
   *
   * This function will convert the value
   * report it as a percentage.
   *
   * @param value Value to convert to a percentage as a double
   * @return Average waiting time of the Truck in percent
   */
  double convertToPercent(const double value) const;

  /**
   * @brief Calculate total mining time
   *
   * This function will calculate the total mining time of a Truck.
   * This value could be larger than 72 hours as the Truck could
   * end the 72 hour simulation in the mining state.
   *
   * @return Total mining duration of the Truck
   */
  int calculateTotalMiningDuration() const;

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