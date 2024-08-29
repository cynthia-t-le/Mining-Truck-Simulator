#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../Simulator.h"
#include "../Site.h"
#include "../Station.h"
#include "../Truck.h"

constexpr int maxSimulationDuration = 72 * 60;
constexpr int travelTimeToSite = 30;
constexpr int travelTimeToStation = 30;
constexpr int unloadingTime = 5;
constexpr int kMinRandomHelium = 60;  // per trip
constexpr int kMaxRandomHelium = 300; // per trip

// Compile with this command
// cd test
// g++ .\test_main.cpp ..\Simulator.cpp ..\Truck.cpp ..\Station.cpp ..\Site.cpp -o test -std=c++20 -pthread
// .\test.exe

TEST_CASE("Check for random number generated correctly.")
{

    Site site = Site();
    int randomNum = site.getRandomMinedDuration();
    REQUIRE(randomNum >= 60);
    REQUIRE(randomNum <= 300);
}

TEST_CASE("Mining simulation to check end results after 72 hours.")
{
    int numTrucks = 30;
    int numStations = 1;

    Simulator miningSim(numTrucks, numStations);
    miningSim.startSimulator();
    std::vector<Truck> trucks = miningSim.getTrucks();
    std::vector<Station> stations = miningSim.getStations();

    int maxPossibleHeliumMined = miningSim.calculateMaximumHeliumPossible(maxSimulationDuration);
    int maxTripPossible = miningSim.calculateMaximumTripsPossible(maxSimulationDuration);

    int totalStationHeliumSum = 0; // Total helium from all of the stations added together
    int totalTruckHeliumSum = 0;   // Total helium from all of the trucks added together

    int totalStationUnloadSum = 0; // Total trucks unloaded from all of the stations added together
    int totalTruckUnloadSum = 0;   // Total successful unload from all of the trucks added together

    for (auto truck : trucks)
    {
        // Check to see if truck's total mined helium exceeds the max possible helium mined in 72 hours
        int maxHeliumMined = truck.getTotalMinedHelium();
        REQUIRE(maxHeliumMined <= maxPossibleHeliumMined);

        // Get the truck's total unloaded to compare against min and max
        int totalNumberUnloads = truck.getTotalNumberUnloads();
        REQUIRE(totalNumberUnloads <= maxTripPossible);

        // Get the summation of all the helium mined from the trucks
        totalTruckHeliumSum += truck.getTotalMinedHelium();

        // Get the summation of all the successful unloaded trips made by all the trucks
        totalTruckUnloadSum += truck.getTotalNumberUnloads();
    }

    for (auto station : stations)
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