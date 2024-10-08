#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../include/Simulator.h"
#include "../include/Site.h"
#include "../include/Station.h"
#include "../include/Truck.h"

TEST_CASE("Random Number Generator.")
{

    int randomNum = Site::getRandomMinedDuration();
    REQUIRE(randomNum >= 60);
    REQUIRE(randomNum <= 300);
}

TEST_CASE("Mining Simulation for 30 trucks and 3 station.")
{
    int numTrucks = 30;
    int numStations = 3;

    Simulator miningSim(numTrucks, numStations);
    miningSim.startSimulator();
    std::vector<Truck> trucks = miningSim.getTrucks();
    std::vector<Station> stations = miningSim.getStations();

    REQUIRE(trucks.size() == numTrucks);
    REQUIRE(stations.size() == numStations);

    int maxPossibleHeliumMined = miningSim.calcMaxHeliumPossible();
    int maxTripPossible = miningSim.calcMaxTripsPossible();

    int minTripPossible = miningSim.calcMinTripsPossible();
    int minPossibleHeliumMined = miningSim.calcMinHeliumPossible();

    int totalStationHeliumSum = 0; // Total helium from all of the stations added together
    int totalTruckHeliumSum = 0;   // Total helium from all of the trucks added together

    int totalStationUnloadSum = 0; // Total trucks unloaded from all of the stations added together
    int totalTruckUnloadSum = 0;   // Total successful unload from all of the trucks added together

    for (auto &truck : trucks)
    {
        int heliumMined = truck.getTotalMinedHelium();
        int totalNumberUnloads = truck.getTotalNumberUnloads();

        // Get the truck's total mined helium and compare against min and max
        REQUIRE(heliumMined >= minPossibleHeliumMined);
        REQUIRE(heliumMined <= maxPossibleHeliumMined);

        // Get the truck's total unloaded to compare against min and max
        REQUIRE(totalNumberUnloads >= minTripPossible);
        REQUIRE(totalNumberUnloads <= maxTripPossible);

        // Get the summation of all the helium mined from the trucks
        totalTruckHeliumSum += truck.getTotalMinedHelium();

        // Get the summation of all the successful unloaded trips made by all the trucks
        totalTruckUnloadSum += truck.getTotalNumberUnloads();
    }

    for (auto &station : stations)
    {
        // Get the summation of all the helium mined unloaded at the stations
        totalStationHeliumSum += station.getTotalHeliumReceived();

        // Get the summation of all the trucks processed by the stations
        totalStationUnloadSum += station.getTotalTrucksUnloaded();
    }

    // Check if summation of all helium mined from trucks is equal to all helium unloaded by stations
    REQUIRE(totalStationHeliumSum == totalTruckHeliumSum);

    // Check if summation of all successful unloaded trips by trucks is equal to number of trucks processed
    // by the stations
    REQUIRE(totalStationUnloadSum == totalTruckUnloadSum);
}