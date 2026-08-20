#pragma once

#include "ranedge/types.hpp"

#include <vector>

namespace ranedge {

class Cell {
public:
    explicit Cell(CellConfig config);

    const CellConfig& config() const;
    void setEdgeCpuBudget(double budget);
    std::vector<Allocation> schedule(const std::vector<UserEquipment>& users) const;
    KpiSnapshot collectKpis(std::uint64_t tick, const std::vector<Allocation>& allocations) const;

private:
    CellConfig config_;
};

} // namespace ranedge
