#include "ranedge/anomaly_detector.hpp"

#include <algorithm>
#include <cassert>

int main() {
    ranedge::AnomalyDetector detector(3);
    ranedge::KpiSnapshot healthy{1, "cell-a", 300.0, 10.0, 0.40, 0.0, 35.0, {}};
    assert(detector.inspect(healthy).empty());
    assert(detector.inspect({2, "cell-a", 310.0, 11.0, 0.42, 0.0, 36.0, {}}).empty());
    const auto alerts = detector.inspect({3, "cell-a", 290.0, 31.0, 0.92, 2.0, 89.0, {}});

    assert(std::find(alerts.begin(), alerts.end(), "latency-regression") != alerts.end());
    assert(std::find(alerts.begin(), alerts.end(), "edge-cpu-pressure") != alerts.end());
    assert(std::find(alerts.begin(), alerts.end(), "mobility-instability") != alerts.end());
    return 0;
}
