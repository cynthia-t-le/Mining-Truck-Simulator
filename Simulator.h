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
    static constexpr int kMaxMiningDurationMins = 72 * 60; // in minutes (72 hours * 60 minutes), during simulation 1 milliseconds = 1 minute
    static constexpr int kTruckTravelTimeMins = 30;        // in mins
    static constexpr int kUnloadTimeMins = 5;              // in mins
    static constexpr int kHeliumMiningRatePerMin = 1;      // 1 helium/min
    static constexpr int kOneCycleTimeMins = Site::MAX_MINING_MINUTES + (kTruckTravelTimeMins * 2) + kUnloadTimeMins;

    // Constructor
    Simulator(const int numTrucks, const int numStations) : numTrucks(numTrucks), numStations(numStations) {}

    // Public member functions
    void startSimulator();

    std::vector<Truck> getTrucks() const { return trucks; }

    void addTruck(Truck truck) { trucks.push_back(truck); }

    std::vector<Station> getStations() const { return stations; }

    void addStation(Station station) { stations.push_back(station); }

    // Public static member functions
    static int calculateMaximumTripsPossible(const int totalSimulationTime);

    static int calculateMaximumHeliumPossible(const int totalSimulationTime);

private:
    int numTrucks;                                 // Value defined by user input for total number of trucks
    int numStations;                               // Value defined by user input for total number of stations
    std::vector<std::thread> miningTruckThreads;   // To store all mining trucks and simulate each truck
    std::vector<std::thread> unloadStationThreads; // To store all unloading stations and simulate each station
    std::vector<Truck> trucks;                     // To store all trucks for unit testing purposes
    std::vector<Station> stations;                 // To store all stations for unit testing purposes

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