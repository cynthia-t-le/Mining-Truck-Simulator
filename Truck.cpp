#include "Truck.h"
#include "Site.h"

// --------------------------------------------------------
// Public Member Functions
// --------------------------------------------------------
double Truck::calculateAverageQueueTime(const int totalQueueWait, const int totalElapsedTime) const
{
    return (static_cast<double>(totalQueueWait) / static_cast<double>(totalElapsedTime));
}

double Truck::convertAverageQueueTimeToPercent(const double averageQueueTime) const
{
    return (averageQueueTime * 100.0);
}

int Truck::calculateTotalMiningDuration() const
{
    int totalCalcMiningDur = 0;
    for (const auto &miningDuration : m_miningDurations)
    {
        totalCalcMiningDur += miningDuration;
    }
    return totalCalcMiningDur;
}

double Truck::convertTruckEfficiencyToPercent(const double truckEfficiency) const
{
    return (truckEfficiency * 100.0);
}