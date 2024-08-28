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

TEST_CASE("Sample only - Unit test to check for random number generated correctly")
{

    Site site = Site();
    int randomNum = site.getRandomMinedDuration();
    // REQUIRE((randomNum >= 60) && (randomNum <= 300));  doesn't work why?
    // CHECK((randomNum >= 60) && (randomNum <= 300));
    REQUIRE(randomNum >= 60);
    REQUIRE(randomNum <= 300);
}

TEST_CASE("Test a truck cannot have more than max possible helium mined in 72 hours.")
{
    int numTrucks = 1;
    int numStations = 1;

    Simulator miningSim(numTrucks, numStations);
    miningSim.startSimulator();
    std::vector<Truck> trucks = miningSim.getTrucks();
    Truck truck = trucks.front();
    int maxPossibleHeliumMined = miningSim.calculateMaximumHeliumPossible(maxSimulationDuration);
    int maxHeliumMined = truck.getTotalMinedHelium();

    REQUIRE(maxHeliumMined <= maxPossibleHeliumMined);
}

TEST_CASE("Unit test to check min & max unloaded trips in 72 hours for every trucks")
{
    int numTrucks = 300; // We need to hardcode it so we can predict the min/max helium mined
    int numStations = 3; // We need to hardcode it so we can predict the min/max helium mined

    Simulator miningSim(numTrucks, numStations);
    miningSim.startSimulator();
    std::vector<Truck> trucks = miningSim.getTrucks();
    int maxTripPossible = miningSim.calculateMaximumTripsPossible(maxSimulationDuration);
    int minTripPossible = (maxTripPossible >= 1) ? 1 : 0; // If max trip possible > 1 then minimum has to be at least 1 trip

    for (auto truck : trucks)
    {
        // Get the truck's total unloaded to compare against min and max
        int totalNumberUnloads = truck.getTotalNumberUnloads();
        REQUIRE(totalNumberUnloads >= minTripPossible);
        REQUIRE(totalNumberUnloads <= maxTripPossible);

        // int maxMinedHelium = maxTripPossible * kMaxRandomHelium;
        // int minMinedHelium = minTripPossible * kMaxRandomHelium;
        // int total = truck.getTotalMinedHelium();
    }
}

TEST_CASE("Check sum of helium received by all the stations equal the sum of all the trucks mined helium in 72 hours")
{
    int numTrucks = 10;
    int numStations = 3;

    Simulator miningSim(numTrucks, numStations);
    miningSim.startSimulator();
    std::vector<Station> stations = miningSim.getStations();
    std::vector<Truck> trucks = miningSim.getTrucks();

    int totalStationHeliumSum = 0; // Total helium from all of the stations added together
    int totalTruckHeliumSum = 0;   // Total helium from all of the trucks added together

    int totalStationUnloadSum = 0; // Total trucks unloaded from all of the stations added together
    int totalTruckUnloadSum = 0;   // Total successful unload from all of the trucks added together

    for (auto station : stations)
    {
        totalStationHeliumSum += station.getTotalHeliumReceived();
        totalStationUnloadSum += station.getTotalTrucksUnloaded();
    }

    for (auto truck : trucks)
    {
        totalTruckHeliumSum += truck.getTotalMinedHelium();
        totalTruckUnloadSum += truck.getTotalNumberUnloads();
    }

    REQUIRE(totalStationHeliumSum == totalTruckHeliumSum);
    REQUIRE(totalStationUnloadSum == totalTruckUnloadSum);
}

TEST_CASE("Check how much helium mined for 1 truck in just 1 trip")
{
    int numTrucks = 1;   // We need to hardcode it so we can predict the min/max helium mined
    int numStations = 1; // We need to hardcode it so we can predict the min/max helium mined
}

TEST_CASE("Check how much helium mined if simulation duration is very short")
{
}

TEST_CASE("Check how much helium mined if simulation duration is 0 or negative")
{
}

TEST_CASE("Check to make sure every trucks will deposit at least 1 load.")
{
    // Lots of trucks and only 1 station in a mining duration time frame just
    // enough for 1/2 trip (eg., random mining time same for every truck and
    // duration is long enough for truck to travel to station but not get back
    // to mining site again.  Hence, each truck will only mine once and unload.
    // We should know the exact amount of helium mined.)
}