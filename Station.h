#ifndef STATION_H
#define STATION_H

class Station
{
public:
  explicit Station(int id);

  int getId() const;

  int getTotalHeliumReceived() const;

  void setTotalHeliumReceived(const int helium);

  int getTotalTrucksUnloaded() const;

  void incrementTotalTrucksUnloaded();

private:
  int m_id;                  // Station id number
  int m_totalHeliumReceived; // Total amount of helium collected from all trucks
  int m_totalTrucksUnloaded; // Total number of trucks serviced
};

#endif