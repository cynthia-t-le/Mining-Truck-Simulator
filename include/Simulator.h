#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <thread>

#include "Truck.h"
#include "Station.h"
#include "Site.h"

class Simulator
{
public:
    // Static constants
    static constexpr int kMaxMiningDurationMins = 72 * 60;                                                                                    // in minutes (72 hours * 60 minutes), during simulation 1 milliseconds = 1 minute
    static constexpr int kTruckTravelTimeMins = 30;                                                                                           // in mins
    static constexpr int kUnloadTimeMins = 5;                                                                                                 // in mins
    static constexpr int kHeliumMiningRatePerMin = 1;                                                                                         // 1 helium/min
    static constexpr int kMaxOneCycleTimeMins = Site::kMaxMiningMinutes + (Simulator::kTruckTravelTimeMins * 2) + Simulator::kUnloadTimeMins; // Max one cycle time duration
    static constexpr int kMinOneCycleTimeMins = Site::kMinMiningMinutes + (Simulator::kTruckTravelTimeMins * 2) + Simulator::kUnloadTimeMins; // Min one cycle time duration

    // Constructor
    Simulator(const int numTrucks, const int numStations) : m_numTrucks(numTrucks), m_numStations(numStations) {}

    // Public member functions
    void startSimulator();

    std::vector<Truck> getTrucks() const { return m_trucks; }

    void addTruck(const Truck &truck) { m_trucks.push_back(truck); }

    std::vector<Station> getStations() const { return m_stations; }

    void addStation(Station station) { m_stations.push_back(station); }

    // Public static member functions
    static int calcMinTripsPossible(); // Per truck

    static int calcMaxTripsPossible(); // Per truck

    static int calcMinHeliumPossible(); // Per truck

    static int calcMaxHeliumPossible(); // Per truck

private:
    int m_numTrucks;                                 // Value defined by user input for total number of trucks
    int m_numStations;                               // Value defined by user input for total number of stations
    std::vector<std::thread> m_miningTruckThreads;   // To store all mining trucks and simulate each truck
    std::vector<std::thread> m_unloadStationThreads; // To store all unloading stations and simulate each station
    std::vector<Truck> m_trucks;                     // To store all trucks for unit testing purposes
    std::vector<Station> m_stations;                 // To store all stations for unit testing purposes

    // Private member functions
    void simulateTruck(int id);

    void simulateStation(int id);

    bool didStationProcessTruck(const Truck truck, const int position);

    void printTruckResults(const Truck &truck, int truckElapsedTime) const;

    void printStationResults(const Station &station) const;

    // Private static member functions
    static void printMessage(const std::string &message);

    static std::string composeDebugMsg(const std::string message);
};

#endif