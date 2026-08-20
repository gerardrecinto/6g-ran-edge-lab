#include "ranedge/cell.hpp"

#include "ranedge/scheduler.hpp"

#include <algorithm>
#include <numeric>

namespace ranedge {

Cell::Cell(CellConfig config) : config_(std::move(config)) {}

const CellConfig& Cell::config() const {
    return config_;
}

void Cell::setEdgeCpuBudget(double budget) {
    config_.edgeCpuBudget = budget;
}

std::vector<Allocation> Cell::schedule(const std::vector<UserEquipment>& users) const {
    return Scheduler{}.allocate(config_, users);
}

KpiSnapshot Cell::collectKpis(std::uint64_t tick, const std::vector<Allocation>& allocations) const {
    std::vector<double> latencies;
    latencies.reserve(allocations.size());

    double throughput = 0.0;
    double prb = 0.0;
    for (const auto& allocation : allocations) {
        throughput += allocation.grantedMbps;
        prb += allocation.prbUtilization;
        latencies.push_back(allocation.latencyMs);
    }

    std::sort(latencies.begin(), latencies.end());
    const std::size_t p95Index = latencies.empty() ? 0 : std::min(latencies.size() - 1, static_cast<std::size_t>(latencies.size() * 0.95));
    const double p95Latency = latencies.empty() ? 0.0 : latencies[p95Index];
    const double handoverFailures = std::count_if(allocations.begin(), allocations.end(), [](const Allocation& allocation) {
        return allocation.latencyMs > 28.0 && allocation.grantedMbps < 30.0;
    });

    // edgeCpuBudget models relative edge host headroom: a smaller budget
    // (weaker/more contended host) pushes modeled CPU pressure up faster for
    // the same radio load, a larger budget absorbs it. 1.0 is the baseline
    // and reproduces the previous fixed formula exactly.
    const double cpuBudget = std::max(0.1, config_.edgeCpuBudget);
    const double edgeCpuUtilization = std::min(99.0, (18.0 + prb * 62.0) / cpuBudget);

    return {
        tick,
        config_.id,
        throughput,
        p95Latency,
        prb,
        handoverFailures,
        edgeCpuUtilization,
        {}
    };
}

std::string radioTechName(RadioTech tech) {
    switch (tech) {
        case RadioTech::Lte:
            return "LTE";
        case RadioTech::Nr5g:
            return "5G NR";
        case RadioTech::SixGResearch:
            return "6G research";
        case RadioTech::Wifi7:
            return "Wi-Fi 7";
    }
    return "unknown";
}

} // namespace ranedge
