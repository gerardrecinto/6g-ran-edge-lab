#pragma once

#include "ranedge/anomaly_detector.hpp"
#include "ranedge/cell.hpp"

#include <cstdint>
#include <vector>

namespace ranedge {

struct SimulationOptions {
    std::uint64_t ticks = 12;
    double edgeCpuBudget = 1.0;
};

class Simulation {
public:
    Simulation();

    std::vector<KpiSnapshot> run(const SimulationOptions& options);

private:
    Cell cell_;
    AnomalyDetector detector_;

    std::vector<UserEquipment> usersForTick(std::uint64_t tick) const;
};

std::string renderTable(const std::vector<KpiSnapshot>& snapshots);
std::string renderJsonLines(const std::vector<KpiSnapshot>& snapshots);

} // namespace ranedge
