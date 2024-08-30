# Unit Test Documentation


### Random Number Generator
- **Purpose**: Verify that the `getRandomMinedDuration` method of `Site` correctly produces a number between 60 and 300.
- **Setup**: A random number is generated through the static function call.
- **Steps**: 
  1. Call `Site::getRandomMinedDuration()`.
  2. Check that the result is [60, 300].
- **Expected Result**: The method should return a value between 60 and 300.

## Mining Simulation for 30 Trucks and 3 Station.
- **Purpose**: Verify that the `startSimulator` method of `Simulator` correctly simulates 30 Trucks and 3 Station.
- **Setup**: An instance of `Simulator` is created and calls the `startSimulator` method.
- **Steps**: 
  1. Call `Simulator miningSim(30, 3)` and then call `miningSim.startSimulator()`.
- **Expected Results**:
  1. The total number of trucks created by the simulator should equal 30.
  2. The total number of stations created by the simulator should equal 3.
  3. For each truck, the total mined helium should not be less than the minimum possible helium during 72 hours (assuming the helium rate will be 1 unit per minute, the minimum possible helium will be 660 units of helium).
  4. For each truck, the total mined helium should not exceed the maximum possible helium during 72 hours (assuming the helium rate will be 1 unit per minute, the maximum possible helium will be 3570 units of helium).
  5. For each truck, the number of unloads it completed during 72 hours should not be less than the minimum possible number of unloads (in the best case scenario, the minimum possible number of unloads will be 11 trips). 
  6. For each truck, the number of unloads it completed during 72 hours should not exceed the maximum possible number of unloads (in the best case scenario, the maximum possible number of unloads will be 34 trips). 
  7. At the end of the simulation, the summation of all the helium mined from all 30 trucks should equal the summation of all the helium unloaded at the stations.
  8. At the end of the simulation, the summation of all the successful unloaded trips made by the trucks should equal the summation of all the trucks processed by the stations. 
