#include "Station.h"

Station::Station(int id) : m_id(id), m_totalHeliumReceived(0), m_totalTrucksUnloaded(0) {}

int Station::getId() const
{
    return m_id;
}

int Station::getTotalHeliumReceived() const
{
    return m_totalHeliumReceived;
}

void Station::setTotalHeliumReceived(const int helium)
{
    m_totalHeliumReceived = helium;
}

int Station::getTotalTrucksUnloaded() const
{
    return m_totalTrucksUnloaded;
}

void Station::incrementTotalTrucksUnloaded()
{
    m_totalTrucksUnloaded++;
}