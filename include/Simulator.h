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

    /**
     * @brief Initialize simulator with number of trucks and stations.
     *
     * This function will initialize the number of trucks and stations
     * that will be utilized during the 72 hour duration.
     *
     * @param numTrucks Number of Trucks for 72 hour simulation
     * @param numStations Number of Stations for 72 hour simulation
     */
    Simulator(const int numTrucks, const int numStations) : m_numTrucks(numTrucks), m_numStations(numStations) {}

    /**
     * @brief Creates the Truck and Station objects and starts simulation.
     *
     * This function will create numTrucks Truck threads and numStations
     * Station threads. It will then execute the threads and start the 72
     * hour simulation.
     */
    void startSimulator();

    /**
     * @brief Return vector of all Truck objects.
     *
     * This function will return all of the Trucks within the Simulator class.
     *
     * @return The vector of all the Trucks per Simulator
     */
    std::vector<Truck> getTrucks() const { return m_trucks; }

    /**
     * @brief Append Truck object to vector.
     *
     * This function will push the Truck object onto the vector.
     *
     * @param truck Truck object to push to the vector.
     */
    void addTruck(const Truck &truck) { m_trucks.push_back(truck); }

    /**
     * @brief Return vector of all Station objects.
     *
     * This function will return all of the Stations within the Simulator class.
     *
     * @return The vector of all the Stations per Simulator
     */
    std::vector<Station> getStations() const { return m_stations; }

    /**
     * @brief Append Station object to vector.
     *
     * This function will push the Station object onto the vector.
     *
     * @param station Station object to push to the vector
     */
    void addStation(const Station &station) { m_stations.push_back(station); }

    /**
     * @brief Calculate minimum number of unloads Truck can do.
     *
     * This function will calculate the minimum number of unloads a single
     * Truck can do during entire simulation time.
     *
     * @return The minimum number of unloads a single Truck can do during
     * entire simulation time.
     */
    static int calcMinTripsPossible(); // Per truck

    /**
     * @brief Calculate maximum number of unloads Truck can do.
     *
     * This function will calculate the maximum number of unloads a single
     * Truck can do during entire simulation time in the best case scenario.
     *
     * @return The maximum number of unloads a single Truck can do during
     * entire simulation time.
     */
    static int calcMaxTripsPossible(); // Per truck

    /**
     * @brief Calculate minimum possible helium mined.
     *
     * This function will calculate the minimum amount of helium a Truck
     * can do during entire simulation time.
     *
     * @return The minimum amount of helium a single Truck can do during
     * entire simulation time.
     */
    static int calcMinHeliumPossible(); // Per truck

    /**
     * @brief Calculate maximum possible helium mined.
     *
     * This function will calculate the maximum amount of helium a Truck
     * can do during entire simulation time in the best case scenario.
     *
     * @return The maximum amount of helium a single Truck can do during
     * entire simulation time.
     */
    static int calcMaxHeliumPossible(); // Per truck

private:
    int m_numTrucks;                                 // Value defined by user input for total number of trucks
    int m_numStations;                               // Value defined by user input for total number of stations
    std::vector<std::thread> m_miningTruckThreads;   // To store all mining trucks and simulate each truck
    std::vector<std::thread> m_unloadStationThreads; // To store all unloading stations and simulate each station
    std::vector<Truck> m_trucks;                     // To store all trucks for unit testing purposes
    std::vector<Station> m_stations;                 // To store all stations for unit testing purposes

    /**
     * @brief Truck simulating 72 hour mining.
     *
     * This function will mimic a singular Truck mining for 72 hours
     * along with any waiting time if applicable during the unloading stage.
     *
     * @param id Truck ID
     */
    void simulateTruck(int id);

    /**
     * @brief Station simulating unloading Trucks.
     *
     * This function will mimic a singular Station unloading helium from
     * a Truck object.
     *
     * @param id Station ID
     */
    void simulateStation(int id);

    /**
     * @brief Checks if Station processed Truck or if Truck is waiting
     * to be processed.
     *
     * This function will check if a Station has processed the Truck
     * or if it waiting by checking if the Truck is still at the same
     * index in the shared data vector when it was first pushed by the Truck.
     *
     * @param truck Truck object to check if it is still in the
     * same position in the shared data vector
     * @param position Index at which the Truck object was pushed in at
     * @return Flag to check if Truck has been unloaded by a Station
     */
    bool didStationProcessTruck(const Truck truck, const int position);

    /**
     * @brief Print results of all the Trucks.
     *
     * This function will print the results of all the Trucks after the 72
     * hour simulation and output it to a text file.
     *
     * @param truck Truck results to print
     * @param truckElapsedTime The time at which the Truck ends mining in minutes
     */
    void printTruckResults(const Truck &truck, int truckElapsedTime) const;

    /**
     * @brief Print results of all the Stations.
     *
     * This function will print the results of all the Stations after the 72
     * hour simulation and output it to a text file.
     *
     * @param station Station results to print
     */
    void printStationResults(const Station &station) const;

    // Private static member functions
    /**
     * @brief Print message to designated text file.
     *
     * This function will print the message to the designated text file
     * if the DEBUG flag was included during compiling.
     *
     * @param message Message to output
     */
    static void printMessage(const std::string &message);

    /**
     * @brief Build string message.
     *
     * This function will create a string that can be used to print out.
     *
     * @param message Message to create a string of
     */
    static std::string composeDebugMsg(const std::string message);
};

#endif