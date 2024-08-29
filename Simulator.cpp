#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <format>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iomanip>
#include <cmath>

#include "Simulator.h"
#include "Site.h"
// #include "Ini.h"

std::vector<Truck *> dataVector; // Shared vector for truck data
std::mutex dataVectorMutex;      // Mutex to protect the shared queue
std::mutex debugPrintMutex;      // Mutex to protect printing out statistics at the end
std::mutex coutMutex;            // Share mutex when dumping cout debug msg

std::condition_variable dataCond;  // Condition variable for synchronization
std::atomic<bool> finished(false); // Atomic flag to signal that producers have finished

std::chrono::steady_clock::time_point startTime;

std::ofstream debugFile("Mining_Simulator_Debugging_Log.txt");
std::ofstream summaryOutFile("Mining_Simulator_Summary.txt");

// --------------------------------------------------------
// Public Member Functions
// --------------------------------------------------------
void Simulator::startSimulator()
{
    summaryOutFile << "Starting mining simulation!!! Number of trucks = " << numTrucks
                   << "; number of stations = " << numStations << std::endl
                   << std::endl;

    startTime = std::chrono::steady_clock::now(); // this is when mining simulation started

    // Start truck mining threads
    for (int i = 0; i < numTrucks; ++i)
    {
        // Capture current instance and call private
        // simulateTruck() member function
        miningTruckThreads.emplace_back([this, i]()
                                        { this->simulateTruck(i); });
        // miningTruckThreads.emplace_back(simulateTruck, i);
    }

    // Start station threads
    for (int i = 0; i < numStations; ++i)
    {
        // Capture current instance and call private
        // simulateStation() member function
        unloadStationThreads.emplace_back([this, i]()
                                          { this->simulateStation(i); });
        // unloadStationThreads.emplace_back(simulateStation, i);
    }

    // Wait for all trucks to finish
    for (auto &truckThread : miningTruckThreads)
    {
        truckThread.join();
    }

    // Signal mining simulation is finished
    finished = true;
    dataCond.notify_all();

    // Wait for all consumers to finish
    for (auto &stationThread : unloadStationThreads)
    {
        stationThread.join();
    }

    // Close file
    summaryOutFile.close();
    debugFile.close();
}

int Simulator::calculateMaximumTripsPossible(const int totalSimulationTime)
{
    return std::floor(totalSimulationTime / Simulator::ONE_CYCLE_TIME); // How many times the truck can do a completecycle
                                                                        //  of mining -> unloading -> back at mine site
}

int Simulator::calculateMaximumHeliumPossible(const int totalSimulationTime)
{
    // Maximum helium a truck can mine in the best case scenario
    int maximumHeliumPossible = 0;

    int numberCompleteCycles = calculateMaximumTripsPossible(totalSimulationTime);

    // Update maximumHeliumPossible with the number of full cycles a truck can complete
    maximumHeliumPossible += (numberCompleteCycles * Site::MAX_MINING_MINUTES * Simulator::HELIUM_MINING_RATE);

    int leftoverTime = (totalSimulationTime - (Simulator::ONE_CYCLE_TIME * numberCompleteCycles)); // Remainder time to determine maximum helium
                                                                                                   // truck can do on last iteration

    int maxTimeMiningDuration = (leftoverTime - Simulator::TRAVEL_TIME - Simulator::UNLOAD_TIME); // Time truck can spend on last iteration to be able to
                                                                                                  // unload before totalSimulationTime is complete
    maximumHeliumPossible += (maxTimeMiningDuration * Simulator::HELIUM_MINING_RATE);

    return maximumHeliumPossible;
}

// --------------------------------------------------------
// Private Member Functions
// --------------------------------------------------------
void Simulator::simulateTruck(int id)
{
    printMessage(composeDebugMsg(std::format("Debug!!! in simulateTruck(); truck id = {}", id)));
    Truck miningTruck(id);
    int elapsedTime = 0;   // Initialize to 0 to simulate the start of simulation time
    addTruck(miningTruck); // Need this for unit test later
    Site miningSite = Site();

    while (elapsedTime < maxMiningDuration)
    {
        printMessage(composeDebugMsg(std::format("Debug!!! in while loop, truck id = {}; elapsed time = {}",
                                                 miningTruck.getId(), elapsedTime)));
        int sleepTime = 0;
        Truck::State curState = miningTruck.getCurrentState();
        switch (miningTruck.getCurrentState())
        {
        case Truck::State::MINING:
        {
            // Update truck's member vars accordingly
            miningTruck.setCurrentMiningTime(miningSite.getRandomMinedDuration());
            miningTruck.setCurrentMinedHelium(miningTruck.getCurrentMiningTime() * Simulator::HELIUM_MINING_RATE);
            miningTruck.setTotalMiningTime(miningTruck.getCurrentMinedHelium() + miningTruck.getTotalMiningTime());
            miningTruck.saveMiningDuration(miningTruck.getCurrentMinedHelium());

            sleepTime = miningTruck.getCurrentMiningTime(); // Use member var for sleepTime

            printMessage(composeDebugMsg(std::format("Debug!!! Mining truck id = {}; elapsed time = {}; mining time = {};"
                                                     " current helium = {}; total mining time = {}.",
                                                     miningTruck.getId(),
                                                     elapsedTime, sleepTime, miningTruck.getCurrentMinedHelium(),
                                                     miningTruck.getTotalMiningTime())));
            miningTruck.setCurrentState(Truck::State::TRAVEL_TO_UNLOAD_STATION);
            break;
        }
        case Truck::State::TRAVEL_TO_MINING_SITE:
            sleepTime = Simulator::TRAVEL_TIME;
            printMessage(composeDebugMsg(std::format("Debug!!! in switch, truck id = {}; "
                                                     "state = TRAVEL_TO_MINING_SITE; sleep time = {}.",
                                                     miningTruck.getId(), sleepTime)));
            miningTruck.setCurrentState(Truck::State::MINING);
            break;
        case Truck::State::TRAVEL_TO_UNLOAD_STATION:
            sleepTime = Simulator::TRAVEL_TIME;
            printMessage(composeDebugMsg(std::format(
                "Debug!!! in switch, truck id = {}; "
                "state = TRAVEL_TO_UNLOAD_STATION; sleepTime = {}.",
                miningTruck.getId(), sleepTime)));
            miningTruck.setCurrentState(Truck::State::UNLOADING);
            break;
        case Truck::State::UNLOADING:
            sleepTime = Simulator::UNLOAD_TIME;
            miningTruck.setTotalMinedHelium(miningTruck.getTotalMinedHelium() + miningTruck.getCurrentMinedHelium());
            printMessage(composeDebugMsg(std::format(
                "Debug!!! Unloading truck id = {}; elapsed time = {}; "
                "current_helium = {}; total_helium = {}; total mining time = {}",
                miningTruck.getId(), elapsedTime, miningTruck.getCurrentMinedHelium(),
                miningTruck.getTotalMinedHelium(), miningTruck.getTotalMiningTime())));
            miningTruck.setCurrentState(Truck::State::TRAVEL_TO_MINING_SITE);
            break;
        default:
            printMessage(composeDebugMsg(std::format(
                "Error!!! Unknown truck mining state.")));
            break;
        }

        // Use saved current state instead of get function because state machine will update current state to the next state
        if (curState == Truck::State::UNLOADING)
        {
            // Push truck to dataQueue
            std::unique_lock<std::mutex> lock(dataVectorMutex);
            dataVector.push_back(&miningTruck);
            int truckPositionInVector = dataVector.size() - 1; // will use this later to see if a station processed any truck before us.
            printMessage(composeDebugMsg(std::format(
                "Debug!!! Pushing truck id = {} to vector position = {}; with helium amount = {} at elapsed time = {} "
                "to dataQueue to unload helium.",
                miningTruck.getId(), truckPositionInVector,
                miningTruck.getCurrentMinedHelium(), elapsedTime)));
            miningTruck.setIsInDataQueue(true);
            dataCond.notify_one(); // Notify a station that new helium is available for unloading
            lock.unlock();         // Unlock shared data queue so station can check if there is a truck on it

            // Now sleep until station processed this truck before truck can continue.
            while (miningTruck.getIsInDataQueue())
            {
                if (didStationProcessTruck(miningTruck, truckPositionInVector)) // handle common case where Station has not wake up to process
                {
                    if (miningTruck.getIsInDataQueue())
                    {
                        int waitTime = 1; // Wait as little as possible to satisfied shortest wait queue requirement.
                        int currentTripQueueWait = miningTruck.getCurrentTripQueueWait();
                        miningTruck.setCurrentTripQueueWait(miningTruck.getCurrentTripQueueWait() + waitTime); // Update truck's current queue wait
                        printMessage(composeDebugMsg(std::format(
                            "Debug!!! Need to unload helium but must wait. Elapsed time = {}; "
                            "Truck id = {}; helium amount = {}; current queue wait time = {}.",
                            elapsedTime, miningTruck.getId(),
                            miningTruck.getCurrentMinedHelium(), currentTripQueueWait)));

                        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
                        elapsedTime += waitTime;
                    }
                }
                else
                {
                    printMessage(composeDebugMsg(std::format(
                        "Debug!!! Truck id = {} was pushed to vector but "
                        "no station processed other truck yet.",
                        miningTruck.getId())));
                }
            }
        }

        // Corner case check - if during last iteration a truck is mining for
        // a time that will be greater than 72 hours, cap the sleep duration so that it is 72 hours
        if ((elapsedTime + sleepTime) > maxMiningDuration)
        {
            // Overwrite sleepTime to ensure truck simulate over 72 hour
            sleepTime = (maxMiningDuration - elapsedTime);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime)); // Must simulate truck's action
        elapsedTime += sleepTime;
    }

    // Print results for each truck
    printTruckResults(miningTruck, elapsedTime);
}

void Simulator::simulateStation(int id)
{
    printMessage(composeDebugMsg(std::format("Debug!!! in simulateStation(); station id = {} ", id)));
    Station unloadStation(id);
    addStation(unloadStation); // Need this for unit test later
    while (true)
    {
        std::unique_lock<std::mutex> lock(dataVectorMutex);
        dataCond.wait(lock, []
                      { return !dataVector.empty() || finished; });

        // If there's any truck on the data queue, then we know it's waiting to unload helium
        while (!dataVector.empty())
        {
            Truck *truck = dataVector.front();
            dataVector.erase(dataVector.begin()); // process truck
            // Successful unloading of truck, update station accordingly
            unloadStation.incrementTotalTrucksUnloaded();
            unloadStation.setTotalHeliumReceived(unloadStation.getTotalHeliumReceived() + truck->getCurrentMinedHelium());

            // Update truck so that truck thread can see that it has been successfully unloaded at station
            truck->incrementTotalNumberUnloads();
            truck->setTotalQueueWait(truck->getTotalQueueWait() + truck->getCurrentTripQueueWait());
            printMessage(composeDebugMsg(std::format("IN STATION DEBUG - truck id = {}, currentTripQueueWait = {},"
                                                     " totalQueueWait = {}\n",
                                                     truck->getId(),
                                                     truck->getCurrentTripQueueWait(), truck->getTotalQueueWait())));
            truck->setCurrentTripQueueWait(0);
            truck->setIsInDataQueue(false);
            int truckCurrentQueueTime = truck->getCurrentTripQueueWait();
            lock.unlock(); // Unlock the mutex while processing the item
            printMessage(composeDebugMsg(std::format(
                "Station = {}; truck id = {}; current queue wait time = {}; "
                "current helium collected = {}; total helium collected = {};"
                " total unloaded helium at station = {}",
                unloadStation.getId(), truck->getId(), truckCurrentQueueTime,
                truck->getCurrentMinedHelium(), truck->getTotalMinedHelium(),
                unloadStation.getTotalHeliumReceived())));

            std::this_thread::sleep_for(std::chrono::milliseconds(Simulator::UNLOAD_TIME)); // Simulate unloading time

            lock.lock(); // Re-lock the mutex before checking the condition again
        }

        if (finished && dataVector.empty())
        {
            break; // Exit if simulation time is finished and queue is empty
        }
    }

    // Print out results from each station after simulation is complete
    printStationResults(unloadStation);
}

bool Simulator::didStationProcessTruck(const Truck truck, const int position)
{
    std::unique_lock<std::mutex> lock(dataVectorMutex);
    if (dataVector.size() > position)
    {
        Truck *truckInVector = dataVector.at(position);
        lock.unlock();
        if (truckInVector != NULL && truckInVector->getId() == truck.getId())
        {
            return false; // No truck has been unloaded
        }
        else
        {
            return true;
        }
    }
    else
    {
        lock.unlock();
        return true;
    }
}

void Simulator::printTruckResults(const Truck &truck, const int truckElapsedTime) const
{
    double averageQueueTime = truck.calculateAverageQueueTime(truck.getTotalQueueWait(), maxMiningDuration);
    double truckEfficiency = static_cast<double>((truck.getTotalMinedHelium()) / static_cast<double>(Simulator::calculateMaximumHeliumPossible(maxMiningDuration)));
    std::lock_guard<std::mutex> lock(debugPrintMutex);

    summaryOutFile << "TRUCK " << truck.getId() << " FINAL RESULTS:" << std::endl
                   << "Total Helium Mined                       = " << truck.getTotalMinedHelium() << std::endl
                   << "Total Mining Duration                    = " << truck.getTotalMiningTime() << " minutes" << std::endl
                   << "Total Successful Unloaded Trips          = " << truck.getTotalNumberUnloads() << std::endl
                   << "Total Time Spent Waiting in Queue        = " << truck.getTotalQueueWait() << " minutes" << std::endl
                   << "Average Time Spent Waiting in Queue      = "
                   << std::fixed << std::setprecision(2) << truck.convertAverageQueueTimeToPercent(averageQueueTime) << "%" << std::endl
                   << "Calculated Mining Duration for Checking  = " << truck.calculateTotalMiningDuration() << " minutes" << std::endl
                   << "Maximum Helium Possible                  = " << Simulator::calculateMaximumHeliumPossible(maxMiningDuration) << std::endl
                   << "Truck Efficiency                         = "
                   << std::fixed << std::setprecision(2) << truck.convertTruckEfficiencyToPercent(truckEfficiency) << "%" << std::endl
                   << "Truck Ending Elapsed Time                = " << truckElapsedTime
                   << std::endl
                   << std::endl;
}

void Simulator::printStationResults(const Station &station) const
{
    std::lock_guard<std::mutex> lock(debugPrintMutex);
    summaryOutFile << "STATION " << station.getId() << " FINAL RESULTS:" << std::endl
                   << "Total Helium Received                    = " << station.getTotalHeliumReceived() << std::endl
                   << "Total Trucks Unloaded                    = " << station.getTotalTrucksUnloaded() << std::endl
                   << std::endl;
}

#ifdef DEBUG
void Simulator::printMessage(const std::string &message)
{
    // Lock the mutex before writing to std::cout
    std::lock_guard<std::mutex> guard(coutMutex);

    // Now it's safe to write to std::cout without interference from other threads
    debugFile << message << std::endl;
}
#else
void Simulator::printMessage(const std::string &message)
{
    // do nothing
}
#endif

std::string Simulator::composeDebugMsg(const std::string message)
{
    std::ostringstream oss;
    oss << message;
    return oss.str();
}