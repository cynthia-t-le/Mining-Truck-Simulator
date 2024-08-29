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
    static constexpr int maxMiningDuration = 72 * 60; // in minutes (72 hours * 60 minutes), during simulation 1 milliseconds = 1 minute
    static constexpr int TRAVEL_TIME = 30;            // in mins
    static constexpr int UNLOAD_TIME = 5;             // in mins
    static constexpr int HELIUM_MINING_RATE = 1;      // 1 helium/min
    static constexpr int ONE_CYCLE_TIME = Site::MAX_MINING_MINUTES + (Simulator::TRAVEL_TIME * 2) + Simulator::UNLOAD_TIME;

    // Constructor
    Simulator(const int numTrucks, const int numStations) : numTrucks(numTrucks), numStations(numStations) {}

    // Public member functions
    void startSimulator();

    std::vector<Truck> getTrucks() const { return _trucks; }

    void addTruck(Truck truck) { _trucks.push_back(truck); }

    std::vector<Station> getStations() const { return _stations; }

    void addStation(Station station) { _stations.push_back(station); }

    // Public static member functions
    static int calculateMaximumTripsPossible(const int totalSimulationTime);

    static int calculateMaximumHeliumPossible(const int totalSimulationTime);

private:
    std::vector<std::thread> miningTruckThreads;
    std::vector<std::thread> unloadStationThreads;
    int numTrucks;
    int numStations;
    std::vector<Truck> _trucks;
    std::vector<Station> _stations;

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