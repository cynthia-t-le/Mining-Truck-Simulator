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

// Internal variables for Simulator
std::vector<Truck *> dataVector; // Shared vector for truck data
std::mutex dataVectorMutex;      // Mutex to protect the shared queue
std::mutex debugPrintMutex;      // Mutex to protect printing out statistics at the end
std::mutex coutMutex;            // Share mutex when dumping cout debug msg

std::condition_variable dataCond;  // Condition variable for synchronization
std::atomic<bool> finished(false); // Atomic flag to signal that producers have finished

std::ofstream debugFile("Mining_Simulator_Debugging_Log.txt");
std::ofstream summaryOutFile("Mining_Simulator_Summary.txt");

// --------------------------------------------------------
// Public Member Functions
// --------------------------------------------------------
void Simulator::startSimulator()
{
    summaryOutFile << "Starting mining simulation! Number of trucks = " << m_numTrucks
                   << ". Number of stations = " << m_numStations << std::endl
                   << std::endl;

    // Start truck mining threads
    for (int i = 0; i < m_numTrucks; ++i)
    {
        // Capture current instance and call private
        // simulateTruck() member function
        m_miningTruckThreads.emplace_back([this, i]()
                                          { this->simulateTruck(i); });
    }

    // Start station threads
    for (int i = 0; i < m_numStations; ++i)
    {
        // Capture current instance and call private
        // simulateStation() member function
        m_unloadStationThreads.emplace_back([this, i]()
                                            { this->simulateStation(i); });
    }

    // Wait for all trucks to finish
    for (auto &truckThread : m_miningTruckThreads)
    {
        truckThread.join();
    }

    // Signal mining simulation is finished
    finished = true;
    dataCond.notify_all();

    // Wait for all stations to finish
    for (auto &stationThread : m_unloadStationThreads)
    {
        stationThread.join();
    }

    // Close file
    summaryOutFile.close();
    debugFile.close();
}

int Simulator::calculateMaximumTripsPossible(const int totalSimulationTime)
{
    return std::floor(totalSimulationTime / Simulator::kMinOneCycleTimeMins); // How many times the truck can do a completecycle
                                                                              //  of mining -> unloading -> back at mine site
}

int Simulator::calculateMaximumHeliumPossible(const int totalSimulationTime)
{
    // Maximum helium a truck can mine in the best case scenario
    int maximumHeliumPossible = 0;

    int numberCompleteCycles = std::floor(totalSimulationTime / Simulator::kMaxOneCycleTimeMins); // How many times the truck can do a completecycle
                                                                                                  //  of mining -> unloading -> back at mine site

    // Update maximumHeliumPossible with the number of full cycles a truck can complete
    maximumHeliumPossible += (numberCompleteCycles * Site::kMaxMiningMinutes * Simulator::kHeliumMiningRatePerMin);

    int leftoverTime = (totalSimulationTime - (Simulator::kMaxOneCycleTimeMins * numberCompleteCycles)); // Remainder time to determine maximum helium
                                                                                                         // truck can do on last iteration

    int lastIterMaxMiningDur = (leftoverTime - Simulator::kTruckTravelTimeMins - Simulator::kUnloadTimeMins); // Time truck can spend on last iteration to be able to
                                                                                                              // unload before totalSimulationTime is complete
    maximumHeliumPossible += (lastIterMaxMiningDur * Simulator::kHeliumMiningRatePerMin);

    return maximumHeliumPossible;
}

// --------------------------------------------------------
// Private Member Functions
// --------------------------------------------------------
void Simulator::simulateTruck(int id)
{
    int elapsedTime = 0; // Initialize to 0 to simulate the start of simulation time
    int sleepTime = 0;

    Truck miningTruck(id);
    addTruck(miningTruck); // Need this for unit test later
    printMessage(composeDebugMsg(std::format("Truck thread started and Truck ID = {}", id)));

    while (elapsedTime < kMaxMiningDurationMins)
    {
        printMessage(composeDebugMsg(std::format("Beginning of while loop, truck id = {}; elapsed time = {}",
                                                 miningTruck.getId(), elapsedTime)));
        Truck::State currentState = miningTruck.getCurrentState();
        switch (currentState)
        {
        case Truck::State::MINING:
        {
            // Update truck's member vars accordingly
            miningTruck.setCurrentMiningTime(Site::getRandomMinedDuration());                                           // Set randomly generated mining duration
            miningTruck.setCurrentMinedHelium(miningTruck.getCurrentMiningTime() * Simulator::kHeliumMiningRatePerMin); // Set helium mined during duration
            miningTruck.setTotalMiningTime(miningTruck.getCurrentMiningTime() + miningTruck.getTotalMiningTime());      // Update total mining time
            miningTruck.saveMiningDuration(miningTruck.getCurrentMiningTime());                                         // Add timing time to vector for unit testing

            sleepTime = miningTruck.getCurrentMiningTime(); // Use member var for sleepTime
            printMessage(composeDebugMsg(std::format("Mining truck id = {}; state = MINING state; mining time = {}; "
                                                     "current helium = {}; total mining time = {}; elapsed time = {}.",
                                                     miningTruck.getId(), sleepTime,
                                                     miningTruck.getCurrentMinedHelium(),
                                                     miningTruck.getTotalMiningTime(), elapsedTime)));
            miningTruck.setCurrentState(Truck::State::TRAVEL_TO_UNLOAD_STATION); // Update trucks state the next state
            break;
        }
        case Truck::State::TRAVEL_TO_MINING_SITE:
        {
            sleepTime = Simulator::kTruckTravelTimeMins;
            printMessage(composeDebugMsg(std::format("Mining truck id = {}; state = TRAVEL_TO_MINING_SITE; "
                                                     "sleep time = {}; elapsed time = {}.",
                                                     miningTruck.getId(), sleepTime, elapsedTime)));
            miningTruck.setCurrentState(Truck::State::MINING); // Update trucks state the next state
            break;
        }
        case Truck::State::TRAVEL_TO_UNLOAD_STATION:
        {
            sleepTime = Simulator::kTruckTravelTimeMins;
            printMessage(composeDebugMsg(std::format(
                "Mining truck id = {}; state = TRAVEL_TO_UNLOAD_STATION; "
                "sleepTime = {}; elapsed time = {}.",
                miningTruck.getId(), sleepTime, elapsedTime)));
            miningTruck.setCurrentState(Truck::State::UNLOADING); // Update trucks state the next state
            break;
        }
        case Truck::State::UNLOADING:
        {
            sleepTime = Simulator::kUnloadTimeMins;
            miningTruck.setTotalMinedHelium(miningTruck.getTotalMinedHelium() + miningTruck.getCurrentMinedHelium());
            printMessage(composeDebugMsg(std::format(
                "Mining truck id = {}; state = UNLOADING; current_helium = {}; "
                "total_helium = {}; total mining time = {}; elapsed time = {}.",
                miningTruck.getId(), miningTruck.getCurrentMinedHelium(),
                miningTruck.getTotalMinedHelium(), miningTruck.getTotalMiningTime(), elapsedTime)));

            miningTruck.setCurrentState(Truck::State::TRAVEL_TO_MINING_SITE); // Update trucks state the next state
            break;
        }
        default:
        {
            sleepTime = 0;
            printMessage(composeDebugMsg(std::format(
                "Error! Mining truck id = {} unknown truck state.", miningTruck.getId())));
            break;
        }
        }
        if (currentState == Truck::State::UNLOADING)
        {
            // Push truck to dataQueue
            std::unique_lock<std::mutex> lock(dataVectorMutex);
            dataVector.push_back(&miningTruck);
            int truckPositionInVector = dataVector.size() - 1; // will use this later to see if a station processed any truck before us.
            printMessage(composeDebugMsg(std::format(
                "Pushing mining truck id = {} to vector position = {}; with helium amount = {} "
                "at elapsed time = {} to dataQueue to unload helium.",
                miningTruck.getId(), truckPositionInVector, miningTruck.getCurrentMinedHelium(), elapsedTime)));

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
                            "Need to unload helium but must wait. Mining truck id = {}; "
                            "helium amount = {}; current queue wait time = {}; elapsed time = {}.",
                            miningTruck.getId(), miningTruck.getCurrentMinedHelium(),
                            currentTripQueueWait, elapsedTime)));

                        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
                        elapsedTime += waitTime;
                    }
                }
                else
                {
                    printMessage(composeDebugMsg(std::format(
                        "Mining truck id = {} was pushed to vector but "
                        "no station processed other truck yet.",
                        miningTruck.getId())));
                }
            }
        }
        // Corner case check - if during last iteration a truck is mining for
        // a time that will be greater than 72 hours, cap the sleep duration so that it is 72 hours
        if ((elapsedTime + sleepTime) > kMaxMiningDurationMins)
        {
            // Overwrite sleepTime to ensure truck simulate over 72 hour
            sleepTime = (kMaxMiningDurationMins - elapsedTime);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime)); // Must simulate truck's action
        elapsedTime += sleepTime;
    }

    // Print results for each truck after 72 hour period is over
    printTruckResults(miningTruck, elapsedTime);
}

void Simulator::simulateStation(int id)
{
    Station unloadStation(id);
    addStation(unloadStation); // Need this for unit test later

    printMessage(composeDebugMsg(std::format("Station thread started and Station ID = {}", id)));

    while (true)
    {
        std::unique_lock<std::mutex> lock(dataVectorMutex);
        dataCond.wait(lock, []
                      { return !dataVector.empty() || finished; });

        // If there's any truck on the data queue, then we know it's waiting to unload helium
        while (!dataVector.empty())
        {
            Truck *truck = dataVector.front();
            dataVector.erase(dataVector.begin()); // Process truck and remove it from dataVector

            // Successful unloading of truck, update station accordingly
            unloadStation.incrementTotalTrucksUnloaded();
            unloadStation.setTotalHeliumReceived(unloadStation.getTotalHeliumReceived() + truck->getCurrentMinedHelium());

            // Update truck so that truck thread can see that it has been successfully unloaded at station
            truck->incrementTotalNumberUnloads();
            truck->setTotalQueueWait(truck->getTotalQueueWait() + truck->getCurrentTripQueueWait());
            printMessage(composeDebugMsg(std::format("Station id = {}; unloading truck id = {}; currentTripQueueWait = {}; "
                                                     "current helium collected = {}; total helium collected = {}; totalQueueWait = {}; "
                                                     "totalSuccessfulUnloads = {}.",
                                                     unloadStation.getId(), truck->getId(), truck->getCurrentMinedHelium(),
                                                     truck->getTotalMinedHelium(), truck->getCurrentTripQueueWait(), truck->getTotalQueueWait(),
                                                     truck->getTotalNumberUnloads())));
            truck->setCurrentTripQueueWait(0); // Reset truck's current queue wait back to 0
            truck->setIsInDataQueue(false);    // Reset flag so that Truck sees it has been processed
            lock.unlock();                     // Unlock the mutex while simulating unloading time

            std::this_thread::sleep_for(std::chrono::milliseconds(Simulator::kUnloadTimeMins)); // Simulate unloading time

            lock.lock(); // Re-lock the mutex before checking the condition again
        }

        if (finished && dataVector.empty())
        {
            break; // Exit if simulation time is finished and vector is empty
        }
    }

    // Print out results from each station after simulation is complete
    printStationResults(unloadStation);
}

bool Simulator::didStationProcessTruck(const Truck truck, const int position)
{
    std::unique_lock<std::mutex> lock(dataVectorMutex);
    // Check if Truck position in case Station has already processed it and it is no
    // longer in the shared dataVector and will return out of range exception
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
            return true; // A truck has been unloaded
        }
    }
    else
    {
        lock.unlock();
        return true; // A truck has been unloaded
    }
}

void Simulator::printTruckResults(const Truck &truck, const int truckElapsedTime) const
{
    double averageQueueTime = truck.calculateAverageQueueTime(truck.getTotalQueueWait(), kMaxMiningDurationMins);
    double truckEfficiency = static_cast<double>((truck.getTotalMinedHelium()) / static_cast<double>(Simulator::calculateMaximumHeliumPossible(kMaxMiningDurationMins)));
    std::lock_guard<std::mutex> lock(debugPrintMutex);

    summaryOutFile << "TRUCK " << truck.getId() << " FINAL RESULTS:" << std::endl
                   << "Total Helium Mined                       = " << truck.getTotalMinedHelium() << std::endl
                   << "Total Mining Duration                    = " << truck.getTotalMiningTime() << " minutes" << std::endl
                   << "Calculated Mining Duration for Checking  = " << truck.calculateTotalMiningDuration() << " minutes" << std::endl
                   << "Total Successful Unloaded Trips          = " << truck.getTotalNumberUnloads() << std::endl
                   << "Total Time Spent Waiting in Queue        = " << truck.getTotalQueueWait() << " minutes" << std::endl
                   << "Average Time Spent Waiting in Queue      = "
                   << std::fixed << std::setprecision(2) << truck.convertAverageQueueTimeToPercent(averageQueueTime) << "%" << std::endl
                   << "Maximum Helium Possible                  = " << Simulator::calculateMaximumHeliumPossible(kMaxMiningDurationMins) << std::endl
                   << "Maximum Unloaded Trips Possible          = " << Simulator::calculateMaximumTripsPossible(kMaxMiningDurationMins) << std::endl
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