#pragma once

#include "ranedge/types.hpp"

#include <vector>

namespace ranedge {

class Scheduler {
public:
    std::vector<Allocation> allocate(const CellConfig& config, const std::vector<UserEquipment>& users) const;

private:
    static const SlicePolicy* findSlice(const CellConfig& config, const std::string& name);
    static double spectralEfficiency(RadioTech tech, double cqi);
};

} // namespace ranedge
