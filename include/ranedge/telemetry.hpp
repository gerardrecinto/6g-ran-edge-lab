#pragma once

#include "ranedge/types.hpp"

#include <string>
#include <vector>

namespace ranedge {

std::string renderPrometheus(const std::vector<KpiSnapshot>& snapshots);
std::string renderOtelLogs(const std::vector<KpiSnapshot>& snapshots);
std::string renderOtelTraces(const std::vector<KpiSnapshot>& snapshots);

} // namespace ranedge
