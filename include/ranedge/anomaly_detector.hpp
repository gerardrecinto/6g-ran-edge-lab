#pragma once

#include "ranedge/types.hpp"

#include <deque>
#include <string>
#include <vector>

namespace ranedge {

class AnomalyDetector {
public:
    explicit AnomalyDetector(std::size_t window = 5);

    std::vector<std::string> inspect(const KpiSnapshot& snapshot);

private:
    std::size_t window_;
    std::deque<KpiSnapshot> history_;

    double rollingLatency() const;
    double rollingPrb() const;
};

} // namespace ranedge
