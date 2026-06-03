#include "ranedge/scheduler.hpp"

#include <algorithm>
#include <cmath>

namespace ranedge {

const SlicePolicy* Scheduler::findSlice(const CellConfig& config, const std::string& name) {
    const auto it = std::find_if(config.slices.begin(), config.slices.end(), [&](const SlicePolicy& slice) {
        return slice.name == name;
    });
    return it == config.slices.end() ? nullptr : &(*it);
}

double Scheduler::spectralEfficiency(RadioTech tech, double cqi) {
    const double clamped = std::clamp(cqi, 0.05, 1.0);
    switch (tech) {
        case RadioTech::Lte:
            return 2.4 * clamped;
        case RadioTech::Nr5g:
            return 5.2 * clamped;
        case RadioTech::SixGResearch:
            return 7.1 * clamped;
        case RadioTech::Wifi7:
            return 4.8 * clamped;
    }
    return clamped;
}

std::vector<Allocation> Scheduler::allocate(const CellConfig& config, const std::vector<UserEquipment>& users) const {
    std::vector<Allocation> allocations;
    allocations.reserve(users.size());

    double totalWeight = 0.0;
    for (const auto& user : users) {
        const auto* slice = findSlice(config, user.slice);
        const double priority = slice == nullptr ? 1.0 : slice->priority;
        totalWeight += priority * spectralEfficiency(user.tech, user.channelQuality);
    }

    const double capacityMbps = config.bandwidthMhz * spectralEfficiency(config.tech, 0.82);
    for (const auto& user : users) {
        const auto* slice = findSlice(config, user.slice);
        const double priority = slice == nullptr ? 1.0 : slice->priority;
        const double weight = priority * spectralEfficiency(user.tech, user.channelQuality);
        const double fairShare = totalWeight <= 0.0 ? 0.0 : capacityMbps * (weight / totalWeight);
        const double grant = std::min(user.demandMbps, fairShare);
        const double latencyFloor = slice == nullptr ? 12.0 : slice->maxLatencyMs * 0.45;
        const double congestion = grant < user.demandMbps ? (user.demandMbps - grant) / std::max(user.demandMbps, 1.0) : 0.0;
        const double mobilityPenalty = user.mobilityEvent ? 4.5 : 0.0;

        allocations.push_back({
            user.id,
            user.slice,
            grant,
            latencyFloor + congestion * 25.0 + mobilityPenalty,
            capacityMbps <= 0.0 ? 0.0 : grant / capacityMbps
        });
    }

    return allocations;
}

} // namespace ranedge
