#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <thread>
#include <format>
#include <vector>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Truck.h"
#include "Station.h"
#include "Site.h"

std::queue<Truck> dataQueue;       // Shared queue for data
std::mutex queueMutex;             // Mutex to protect the shared queue
std::mutex debugPrintMutex;        // Mutex to protect printing out statistics at the end
std::condition_variable dataCond;  // Condition variable for synchronization
std::atomic<bool> finished(false); // Atomic flag to signal that producers have finished

std::mutex cout_mutex; // Share mutex when dumping cout debug msg

constexpr int maxMiningDuration = 72 * 60; // in minutes (72 hours * 60 minutes), during simulation 1 milliseconds = 1 minute

// auto startTime = std::chrono::steady_clock::now(); // this is when mining simulation started
std::chrono::steady_clock::time_point startTime;

std::ofstream debugFile("Mining_Simulator_Debugging_Log.txt");
std::ofstream outputFile("Mining_Simulator_Results.txt");

#ifdef DEBUG
void printMessage(const std::string &message, int threadID)
{
    // Lock the mutex before writing to std::cout
    std::lock_guard<std::mutex> guard(cout_mutex);

    // Now it's safe to write to std::cout without interference from other threads
    debugFile << "Thread " << threadID << ": " << message << std::endl;

    // Mutex is automatically released when the lock_guard goes out of scope
}

void printMessage(const std::string &message)
{
    // Lock the mutex before writing to std::cout
    std::lock_guard<std::mutex> guard(cout_mutex);

    // Now it's safe to write to std::cout without interference from other threads
    debugFile << message << std::endl;

    // Mutex is automatically released when the lock_guard goes out of scope
}
#else
void printMessage(const std::string &message, int threadID)
{
    // do nothing
}
void printMessage(const std::string &message)
{
    // do nothing
}
#endif

std::string composeDebugMsg(const std::string msg)
{
    std::ostringstream oss;
    oss << msg;
    return oss.str();
}

// Function to get a valid integer input from the user
int getValidIntegerInput(const std::string &prompt)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value <= 0)
        {
            std::cin.clear();                                                   // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a positive integer." << std::endl;
        }
        else
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard any remaining input
            return value;
        }
    }
}

void simulateTruck(int id)
{
    printMessage(composeDebugMsg(std::format("Debug!!! in simulateTruck(); truck id = {}", id)));
    auto now = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    Truck miningTruck(id, elapsedTime);
    Site miningSite = Site();
    while (elapsedTime < maxMiningDuration)
    {
        printMessage(composeDebugMsg(std::format("Debug!!! in while loop, truck id = {}; elapsed time = {}",
                                                 miningTruck.getId(), elapsedTime)));
        int sleepTime = 0;
        Truck::State curState = miningTruck.getCurrentState();
        // int curHeliumMined;   // we assume 1 millisecond of mining is equal to 1 unit of helium
        switch (miningTruck.getCurrentState())
        {
        case Truck::State::MINING:
        {
            miningTruck.setCurrentMiningTime(miningSite.getRandomMinedDuration());
            miningTruck.setCurrentMinedHelium(miningTruck.getCurrentMiningTime() * Truck::HELIUM_MINING_RATE);
            miningTruck.setTotalMiningTime(miningTruck.getCurrentMinedHelium() + miningTruck.getTotalMiningTime());
            miningTruck.saveMiningDuration(miningTruck.getCurrentMinedHelium());
            sleepTime = miningTruck.getCurrentMiningTime();
            // int curHeliumMined = sleepTime;   // we assume 1 millisecond of mining is equal to 1 unit of helium
            // miningTruck.setCurrentMinedHelium(curHeliumMined);
            printMessage(composeDebugMsg(std::format("Debug!!! Mining truck id = {}; elapsed time = {}; mining time = {};"
                                                     " current helium = {}; total mining time = {}.",
                                                     miningTruck.getId(),
                                                     elapsedTime, sleepTime, miningTruck.getCurrentMinedHelium(),
                                                     miningTruck.getTotalMiningTime())));
            miningTruck.setCurrentState(Truck::State::TRAVEL_TO_UNLOAD_STATION);
            break;
        }
        case Truck::State::TRAVEL_TO_MINING_SITE:
            sleepTime = Truck::TRAVEL_TIME;
            printMessage(composeDebugMsg(std::format("Debug!!! in switch, truck id = {}; "
                                                     "state = TRAVEL_TO_MINING_SITE; sleep time = {}.",
                                                     miningTruck.getId(), sleepTime)));
            miningTruck.setCurrentState(Truck::State::MINING);
            break;
        case Truck::State::TRAVEL_TO_UNLOAD_STATION:
            sleepTime = Truck::TRAVEL_TIME;
            printMessage(composeDebugMsg(std::format("Debug!!! in switch, truck id = {}; "
                                                     "state = TRAVEL_TO_UNLOAD_STATION; sleepTime = {}.",
                                                     miningTruck.getId(), sleepTime)));
            miningTruck.setCurrentState(Truck::State::UNLOADING);
            break;
        case Truck::State::UNLOADING:
            sleepTime = Truck::UNLOAD_TIME;
            miningTruck.setTotalMinedHelium(miningTruck.getTotalMinedHelium() + miningTruck.getCurrentMinedHelium());
            printMessage(composeDebugMsg(std::format("Debug!!! Unloading truck id = {}; elapsed time = {}; "
                                                     "current_helium = {}; total_helium = {}; total mining time = {}",
                                                     miningTruck.getId(), elapsedTime, miningTruck.getCurrentMinedHelium(),
                                                     miningTruck.getTotalMinedHelium(), miningTruck.getTotalMiningTime())));
            miningTruck.setCurrentState(Truck::State::TRAVEL_TO_MINING_SITE);
            break;
        default:
            printMessage(composeDebugMsg(std::format("Error!!! Unknown truck mining state.")));
            break;
        }
        if (curState == Truck::State::UNLOADING)
        {
            printMessage(composeDebugMsg(std::format("Debug!!! Truck id = {}; needs to unload helium.", miningTruck.getId())));
            // Now check to see if dataQueue is empty or not. If not empty, then there's 1 or more trucks waiting to unload.  Take approp. actions
            while (!dataQueue.empty())
            { // sleep for unload time
                printMessage(composeDebugMsg(std::format("Debug!!! Need to unload helium but must wait. Elapsed time = {}; "
                                                         "Truck id = {}; helium amount = {}; current queue wait time = {}.",
                                                         elapsedTime, miningTruck.getId(), miningTruck.getCurrentMinedHelium(),
                                                         miningTruck.getCurrentTripQueueWait())));
                /*
                std::this_thread::sleep_for(std::chrono::milliseconds(unloadTime));  // Must simulate truck's action
                elapsedTime += unloadTime;
                */
                int waitTime = 1; // Wait as little as possible to satisfied shortest wait queue requirement.
                std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
                elapsedTime += waitTime;
                // Update truck's current queue wait
                miningTruck.setCurrentTripQueueWait(miningTruck.getCurrentTripQueueWait() + waitTime);
            }
            // printMessage(composeDebugMsg(std::format("Debug!!! Pushing truck id = {}; with helium amount = {} at elapsed time = {} "
            //                                          "with current queue wait time = {} to dataQueue to unload helium.",
            //                                          miningTruck.getId(), miningTruck.getCurrentMinedHelium(), elapsedTime, miningTruck.getCurrentTripQueueWait())));
            std::lock_guard<std::mutex> lock(queueMutex);
            dataQueue.push(miningTruck);
            // Update truck's total wait time
            miningTruck.setTotalQueueWait(miningTruck.getTotalQueueWait() + miningTruck.getCurrentTripQueueWait());
            miningTruck.incrementTotalNumberUnloads();
            printMessage(composeDebugMsg(std::format("Debug!!! Pushing truck id = {}; with helium amount = {} at elapsed time = {} "
                                                     "with current queue wait time = {} and total queue wait = {} to dataQueue to unload helium.",
                                                     miningTruck.getId(), miningTruck.getCurrentMinedHelium(), elapsedTime,
                                                     miningTruck.getCurrentTripQueueWait(), miningTruck.getTotalQueueWait())));
            // Reset current queue wait time since by the time truck gets here, it has been pushed
            // onto the queue and a station will process it since queue is no longer empty
            miningTruck.setCurrentTripQueueWait(0);
            dataCond.notify_one(); // Notify a station that new helium is available for unloading
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime)); // Must simulate truck's action
        elapsedTime += sleepTime;
    }

    // Print out results of each truck at the end
    std::lock_guard<std::mutex> lock(debugPrintMutex);
    outputFile << "TRUCK " << miningTruck.getId() << " FINAL RESULTS - "
               << "Truck object started mining at time = " << miningTruck.getCreationTime()
               << "; Total Helium Mined = " << miningTruck.getTotalMinedHelium()
               << "; Total Mining Time = " << miningTruck.getTotalMiningTime()
               << "; Total Successful Unloaded Trips = " << miningTruck.getTotalNumberUnloads()
               << "; Total Time Spent Waiting in Queue = " << miningTruck.getTotalQueueWait()
               << "; Average Time Spent Waiting in Queue = " << miningTruck.calculateAverageQueueTime(miningTruck.getTotalQueueWait(), elapsedTime)
               << "; Calculated Mining Duration for Checking = " << miningTruck.calculateTotalMiningDuration()
               << "; Maximum Helium Possible = " << miningTruck.calculateMaximumHeliumPossible(maxMiningDuration)
               << "; Truck Efficiency = "
               << static_cast<double>((miningTruck.getTotalMinedHelium()) / static_cast<double>(miningTruck.calculateMaximumHeliumPossible(maxMiningDuration)))
               << std::endl;
}

void simulateStation(int id)
{
    printMessage(composeDebugMsg(std::format("Debug!!! in simulateStation(); station id = {} ", id)));
    Station unloadStation(id);
    while (true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        dataCond.wait(lock, []
                      { return !dataQueue.empty() || finished; });

        // If there's any truck on the data queue, then we know it's waiting to unload helium
        while (!dataQueue.empty())
        {
            Truck truck = dataQueue.front();
            dataQueue.pop();
            int truckCurrentQueueTime = truck.getCurrentTripQueueWait();
            // truck.setCurrentTripQueueWait(0); // CANNOT DO THIS HERE UNLESS WE CAN SOMEHOW LOCK TRUCK OBJ TO UPDATE MEMBER VARIABLE
            lock.unlock(); // Unlock the mutex while processing the item
            printMessage(composeDebugMsg(std::format("Station = {}; truck id = {}; current queue wait time = {}; "
                                                     "current helium collected = {}; total helium collected = {};"
                                                     " total unloaded helium at station = {}",
                                                     unloadStation.getId(), truck.getId(), truckCurrentQueueTime,
                                                     truck.getCurrentMinedHelium(), truck.getTotalMinedHelium(),
                                                     unloadStation.getTotalHeliumReceived())));

            std::this_thread::sleep_for(std::chrono::milliseconds(Truck::UNLOAD_TIME)); // Simulate unloading time
            unloadStation.setTotalHeliumReceived(unloadStation.getTotalHeliumReceived() + truck.getCurrentMinedHelium());
            lock.lock(); // Re-lock the mutex before checking the condition again
        }

        if (finished && dataQueue.empty())
        {
            break; // Exit if simulation time is finished and queue is empty
        }
    }

    // Print out results from each station after simulation is complete
    std::lock_guard<std::mutex> lock(debugPrintMutex);
    outputFile << std::format("STATION {};  FINAL RESULTS - Total Helium Unloaded = {}.",
                              // std::cout << std::format("STATION {};  FINAL RESULTS - Total Helium Unloaded = {}.",
                              unloadStation.getId(), unloadStation.getTotalHeliumReceived())
               << std::endl;
}

int main()
{
    std::vector<std::thread> miningTruckThreads;
    std::vector<std::thread> unloadStationThreads;

    // To speed up simuation time, we assume 1 minute of simulation time is equal to 1 millisecond of CPU time
    const int numTrucks = getValidIntegerInput("Enter the number of mining trucks: ");
    const int numStations = getValidIntegerInput("Enter the number of unloading stations: ");

    outputFile << "Starting mining simulation!!! Number of trucks = " << numTrucks
               << "; number of stations = " << numStations << std::endl;

    startTime = std::chrono::steady_clock::now(); // this is when mining simulation started

    // Start truck mining threads
    for (int i = 0; i < numTrucks; ++i)
    {
        miningTruckThreads.emplace_back(simulateTruck, i);
    }

    // Start station threads
    for (int i = 0; i < numStations; ++i)
    {
        unloadStationThreads.emplace_back(simulateStation, i);
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

    // Dump out some statistics
    /*
    for (Station station : unloadStationThreads) {
    }
    */

    // Close file
    outputFile.close();
    debugFile.close();

    return 0;
}