#ifndef STATION_H
#define STATION_H

class Station
{
public:
  // Constructor
  explicit Station(int id) : m_id(id), m_totalHeliumReceived(0), m_totalTrucksUnloaded(0) {}

  // Public member functions
  int getId() const { return m_id; }

  int getTotalHeliumReceived() const { return m_totalHeliumReceived; }

  void setTotalHeliumReceived(const int helium) { m_totalHeliumReceived = helium; }

  int getTotalTrucksUnloaded() const { return m_totalTrucksUnloaded; }

  void incrementTotalTrucksUnloaded() { m_totalTrucksUnloaded++; }

private:
  int m_id;                  // Station id number
  int m_totalHeliumReceived; // Total amount of helium collected from all trucks
  int m_totalTrucksUnloaded; // Total number of trucks serviced
};

#endif