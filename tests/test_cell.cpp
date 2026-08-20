#include "ranedge/cell.hpp"

#include <cassert>

int main() {
    ranedge::CellConfig config{
        "test-cell",
        ranedge::RadioTech::Nr5g,
        100.0,
        1.0,
        {{"video", 1.0, 200.0, 20.0}}
    };
    const std::vector<ranedge::Allocation> allocations{
        {"ue-a", "video", 100.0, 12.0, 0.5}
    };

    ranedge::Cell baseline(config);
    const auto baselineKpi = baseline.collectKpis(1, allocations);

    ranedge::Cell lowBudget(config);
    lowBudget.setEdgeCpuBudget(0.5);
    const auto lowBudgetKpi = lowBudget.collectKpis(1, allocations);

    ranedge::Cell highBudget(config);
    highBudget.setEdgeCpuBudget(2.0);
    const auto highBudgetKpi = highBudget.collectKpis(1, allocations);

    // A smaller edge CPU budget (weaker/more contended host) must model more
    // pressure for identical radio load, and a larger budget must model less.
    assert(lowBudgetKpi.edgeCpuUtilization > baselineKpi.edgeCpuUtilization);
    assert(highBudgetKpi.edgeCpuUtilization < baselineKpi.edgeCpuUtilization);

    // The formula must still stay within the documented 0-99 range.
    assert(lowBudgetKpi.edgeCpuUtilization <= 99.0);
    assert(highBudgetKpi.edgeCpuUtilization >= 0.0);

    // A non-positive budget must not divide by zero or go negative.
    ranedge::Cell zeroBudget(config);
    zeroBudget.setEdgeCpuBudget(0.0);
    const auto zeroBudgetKpi = zeroBudget.collectKpis(1, allocations);
    assert(zeroBudgetKpi.edgeCpuUtilization <= 99.0);
    assert(zeroBudgetKpi.edgeCpuUtilization >= 0.0);

    return 0;
}
