#include "ranedge/scheduler.hpp"

#include <cassert>
#include <cmath>

int main() {
    ranedge::CellConfig config{
        "test-cell",
        ranedge::RadioTech::Nr5g,
        80.0,
        1.0,
        {{"control", 3.0, 40.0, 6.0}, {"video", 1.0, 100.0, 20.0}}
    };
    const std::vector<ranedge::UserEquipment> users{
        {"ue-a", ranedge::RadioTech::Nr5g, "control", 50.0, 0.90, false},
        {"ue-b", ranedge::RadioTech::Nr5g, "video", 250.0, 0.60, false},
    };

    const auto allocations = ranedge::Scheduler{}.allocate(config, users);
    assert(allocations.size() == 2);
    assert(allocations[0].grantedMbps > allocations[1].grantedMbps * 0.5);
    assert(allocations[0].latencyMs <= allocations[1].latencyMs);
    return 0;
}
