# Mining Simulator

Mining Simulator is an executable that will simulate a lunar Helium-3 space mining operation over a period of a continuous 72 hours. It will produce an output file that reports the efficiency of each mining truck and each unloading station.

## Build Dependencies
- C++ compiler supporting C++20, tested with
  - g++ 13.2.0 to compile for Windows
- catch2

## How to Create the Executable
To create the executable for the simulator, follow the steps below:
```bash
# Open a terminal (Command Prompt or PowerShell)

# Clone the repository
git clone https://github.com/cynthia-t-le/Mining-Truck-Simulator.git

# Navigate to the project directory
cd Mining-Truck-Simulator

# Navigate to the src folder
cd src

# Compile and produce the executable
# include -DDEBUG if you want to produce the debugging output file, otherwise remove it from the command
g++ -DDEBUG main.cpp Truck.cpp Simulator.cpp Site.cpp -o ../bin/MiningSimulator -std=c++20 -pthread

# The executable should now be in your "C:/../Mining-Truck-Simulator/bin" path as MiningSimulator.exe
```

To create the unit test executable, follow the steps below:
```bash
# Open a terminal (Command Prompt or PowerShell)

# Navigate to the project directory
cd Mining-Truck-Simulator

# Navigate to the test folder
cd test

# Compile and produce the executable
g++ .\test_main.cpp ..\src\Simulator.cpp ..\src\Truck.cpp ..\src\Site.cpp -o UnitTest -std=c++20 -pthread

# The executable should now be in your directory as UnitTest.exe
```

## Run the Simulator
The previous section "How to Create the Executable" must be completed in order to continue. To run the simulator, follow the steps below:
```bash
# Open a terminal (Command Prompt or PowerShell)

# Navigate to the project directory
cd Mining-Truck-Simulator

# Navigate to the bin folder
cd bin

# Confirm MiningSimulator.exe is in the working path and run the executable
.\MiningSimulator.exe

# Enter the number of trucks and stations you want to simulate when prompted
10    # 10 trucks
2     # 2 stations

# Wait for program to finish and check that "Mining_Simulator_Summary.txt" is produced
# in the "C/../Mining-Truck-Simulator/log" folder path
# "Mining_Simulator_Debugging_Log.txt" is only produced if -DDEBUG flag was
# included when compiling

# View "Mining_Simulator_Summary.txt" to see the end results of each truck's
# efficiency and each station's efficiency
```

## Run the Unit Tests
The previous section "How to Create the Executable" must be completed in order to continue. To run the unit tests, follow the steps below:
```bash
# Open a terminal (Command Prompt or PowerShell)

# Navigate to the project directory
cd Mining-Truck-Simulator

# Navigate to the test director
cd test

# Confirm UnitTest.exe is in the working path and run the executable
.\UnitTest.exe

# Wait for program to finish and confirm that the terminal shows
===============================================================================
All tests passed
```



