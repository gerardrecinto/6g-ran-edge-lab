#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ranedge {

enum class RadioTech {
    Lte,
    Nr5g,
    SixGResearch,
    Wifi7
};

struct SlicePolicy {
    std::string name;
    double priority;
    double minThroughputMbps;
    double maxLatencyMs;
};

struct UserEquipment {
    std::string id;
    RadioTech tech;
    std::string slice;
    double demandMbps;
    double channelQuality;
    bool mobilityEvent;
};

struct CellConfig {
    std::string id;
    RadioTech tech;
    double bandwidthMhz;
    double edgeCpuBudget;
    std::vector<SlicePolicy> slices;
};

struct Allocation {
    std::string ueId;
    std::string slice;
    double grantedMbps;
    double latencyMs;
    double prbUtilization;
};

struct KpiSnapshot {
    std::uint64_t tick;
    std::string cellId;
    double throughputMbps;
    double p95LatencyMs;
    double prbUtilization;
    double handoverFailures;
    double edgeCpuUtilization;
    std::vector<std::string> alerts;
};

std::string radioTechName(RadioTech tech);

} // namespace ranedge
