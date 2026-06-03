#include "ranedge/simulation.hpp"

#include <cassert>

int main() {
    ranedge::Simulation simulation;
    const auto snapshots = simulation.run({10});
    assert(snapshots.size() == 10);
    assert(snapshots.front().throughputMbps > 0.0);

    bool sawAlert = false;
    for (const auto& item : snapshots) {
        sawAlert = sawAlert || !item.alerts.empty();
    }
    assert(sawAlert);

    const auto table = ranedge::renderTable(snapshots);
    assert(table.find("edge_cpu") != std::string::npos);
    return 0;
}
