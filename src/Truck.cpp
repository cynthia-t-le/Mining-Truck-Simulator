#include "../include/Truck.h"
#include "../include/Site.h"

// --------------------------------------------------------
// Public Member Functions
// --------------------------------------------------------
double Truck::calculateAverageQueueTime(const int totalQueueWait, const int totalElapsedTime) const
{
    return (static_cast<double>(totalQueueWait) / static_cast<double>(totalElapsedTime));
}

double Truck::convertToPercent(const double value) const
{
    return (value * 100.0);
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