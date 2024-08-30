#ifndef STATION_H
#define STATION_H

class Station
{
public:
  /**
   * @brief Initialize Station with default parameters.
   *
   * This function will initialize the Station ID and initialize
   * class variables to default values.
   *
   * @param id Station ID
   */
  explicit Station(int id) : m_id(id), m_totalHeliumReceived(0), m_totalTrucksUnloaded(0) {}

  /**
   * @brief Get Station's ID
   *
   * This function will return the Station's ID.
   *
   * @return Station's ID
   */
  int getId() const { return m_id; }

  /**
   * @brief Get Station's total amount of helium collected.
   *
   * This function will return the amount of helium that has been
   * unloaded by Trucks at this Station.
   *
   * @return Total amount of helium collected from Trucks
   */
  int getTotalHeliumReceived() const { return m_totalHeliumReceived; }

  /**
   * @brief Set Station's total amount of helium collected.
   *
   * This function will set the amount of helium that has been
   * unloaded by Trucks at this Station.
   *
   * @param int Total amount of helium collected from Trucks
   */
  void setTotalHeliumReceived(const int helium) { m_totalHeliumReceived = helium; }

  /**
   * @brief Get Station's total number of trucks processed.
   *
   * This function will get the amount of Trucks that has been
   * successfully processed by this Station.
   *
   * @param int Total number of Trucks processed
   */
  int getTotalTrucksUnloaded() const { return m_totalTrucksUnloaded; }

  /**
   * @brief Increment Station's total number of trucks processed.
   *
   * This function will increment the amount of Trucks that has been
   * successfully processed by this Station by 1.
   *
   */
  void incrementTotalTrucksUnloaded() { m_totalTrucksUnloaded++; }

private:
  int m_id;                  // Station id number
  int m_totalHeliumReceived; // Total amount of helium collected from all trucks
  int m_totalTrucksUnloaded; // Total number of trucks serviced
};

#endif