#include "ranedge/anomaly_detector.hpp"

#include <numeric>
#include <sstream>

namespace ranedge {

AnomalyDetector::AnomalyDetector(std::size_t window) : window_(window) {}

double AnomalyDetector::rollingLatency() const {
    if (history_.empty()) {
        return 0.0;
    }
    const double total = std::accumulate(history_.begin(), history_.end(), 0.0, [](double sum, const KpiSnapshot& item) {
        return sum + item.p95LatencyMs;
    });
    return total / static_cast<double>(history_.size());
}

double AnomalyDetector::rollingPrb() const {
    if (history_.empty()) {
        return 0.0;
    }
    const double total = std::accumulate(history_.begin(), history_.end(), 0.0, [](double sum, const KpiSnapshot& item) {
        return sum + item.prbUtilization;
    });
    return total / static_cast<double>(history_.size());
}

std::vector<std::string> AnomalyDetector::inspect(const KpiSnapshot& snapshot) {
    std::vector<std::string> alerts;
    const double baselineLatency = rollingLatency();
    const double baselinePrb = rollingPrb();

    if (!history_.empty() && snapshot.p95LatencyMs > baselineLatency * 1.25 && snapshot.p95LatencyMs > 22.0) {
        alerts.push_back("latency-regression");
    }
    if (!history_.empty() && snapshot.prbUtilization > baselinePrb * 1.15 && snapshot.prbUtilization > 0.72) {
        alerts.push_back("air-interface-saturation");
    }
    if (snapshot.edgeCpuUtilization > 82.0) {
        alerts.push_back("edge-cpu-pressure");
    }
    if (snapshot.handoverFailures >= 2.0) {
        alerts.push_back("mobility-instability");
    }

    history_.push_back(snapshot);
    while (history_.size() > window_) {
        history_.pop_front();
    }

    return alerts;
}

} // namespace ranedge
