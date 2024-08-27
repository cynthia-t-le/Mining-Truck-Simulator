#include <cmath>

#include "Truck.h"
#include "Site.h"

// Constructor implementation
Truck::Truck(const int id, const int time) : m_id(id), m_creationTime(time), m_currentState(MINING), m_currentMiningTime(0),
                                             m_currentMinedHelium(0), m_currentTripQueueWait(0), m_totalMinedHelium(0),
                                             m_totalMiningTime(0), m_totalUnloadedTrips(0), m_totalQueueWait(0) {}

int Truck::getId() const
{
    return m_id;
}

int Truck::getCreationTime() const
{
    return m_creationTime;
}

Truck::State Truck::getCurrentState() const
{
    return m_currentState;
}

void Truck::setCurrentState(const State newState)
{
    m_currentState = newState;
}

int Truck::getCurrentMiningTime() const
{
    return m_currentMiningTime;
}

void Truck::setCurrentMiningTime(const int duration)
{
    m_currentMiningTime = duration;
}

int Truck::getCurrentMinedHelium()
{
    return m_currentMinedHelium;
}
void Truck::setCurrentMinedHelium(const int value)
{
    m_currentMinedHelium = value;
}

int Truck::getCurrentTripQueueWait()
{
    return m_currentTripQueueWait;
}

void Truck::setCurrentTripQueueWait(const int value)
{
    m_currentTripQueueWait = value;
}

int Truck::getTotalMiningTime() const
{
    return m_totalMiningTime;
}
void Truck::setTotalMiningTime(const int time)
{
    m_totalMiningTime = time;
}

int Truck::getTotalMinedHelium() const
{
    return m_totalMinedHelium;
}
void Truck::setTotalMinedHelium(const int value)
{
    m_totalMinedHelium = value;
}

int Truck::getTotalNumberUnloads() const
{
    return m_totalUnloadedTrips;
}

void Truck::incrementTotalNumberUnloads()
{
    m_totalUnloadedTrips++;
}

int Truck::getTotalQueueWait() const
{
    return m_totalQueueWait;
}

void Truck::setTotalQueueWait(const int value)
{
    m_totalQueueWait = value;
}

double Truck::calculateAverageQueueTime(const int totalQueueWait, const int totalElapsedTime)
{
    return (static_cast<double>(totalQueueWait) / static_cast<double>(totalElapsedTime));
}

void Truck::saveMiningDuration(const int value)
{
    m_miningDurations.push_back(value);
}

int Truck::calculateTotalMiningDuration() const
{
    int totalCalcMiningDur = 0;
    for (const auto &duration : m_miningDurations)
    {
        totalCalcMiningDur += duration;
    }
    return totalCalcMiningDur;
}

int Truck::calculateMaximumHeliumPossible(const int totalSimulationTime)
{
    // Maximum helium a truck can mine in the best case scenario
    int maximumHeliumPossible = 0;

    int totalCycleTime = (Site::MAX_MINING_MINUTES + (TRAVEL_TIME * 2) + UNLOAD_TIME); // Time it takes for one full cycle from mining to
                                                                                       // traveling to unloading to back to mine site

    int numberCompleteCycles = std::floor(totalSimulationTime / totalCycleTime); // How many times the truck can do a completecycle
                                                                                 //  of mining -> unloading -> back at mine site
    // Update maximumHeliumPossible with the number of full cycles a truck can complete
    maximumHeliumPossible += (numberCompleteCycles * Site::MAX_MINING_MINUTES * HELIUM_MINING_RATE);

    int leftoverTime = (totalSimulationTime - (totalCycleTime * numberCompleteCycles)); // Remainder time to determine maximum helium
                                                                                        // truck can do on last iteration

    int maxTimeMiningDuration = (leftoverTime - TRAVEL_TIME - UNLOAD_TIME); // Time truck can spend on last iteration to be able to
                                                                            // unload before totalSimulationTime is complete
    maximumHeliumPossible += (maxTimeMiningDuration * HELIUM_MINING_RATE);

    return maximumHeliumPossible;
}