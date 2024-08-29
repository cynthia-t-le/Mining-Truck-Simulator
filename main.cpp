#include <iostream>

#include "Simulator.h"

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

int main()
{
    // To speed up simuation time, we assume 1 minute of simulation time is equal to 1 millisecond of CPU time
    const int numTrucks = getValidIntegerInput("Enter the number of mining trucks: ");
    const int numStations = getValidIntegerInput("Enter the number of unloading stations: ");

    Simulator miningSim(numTrucks, numStations);
    miningSim.startSimulator();

    return 0;
}