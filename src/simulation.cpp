#include "ranedge/simulation.hpp"

#include <iomanip>
#include <sstream>

namespace ranedge {

Simulation::Simulation()
    : cell_(CellConfig{
          "lab-ran-01",
          RadioTech::SixGResearch,
          120.0,
          1.0,
          {
              {"urllc-control", 3.0, 80.0, 6.0},
              {"embb-video", 1.7, 220.0, 18.0},
              {"iot-sensing", 1.1, 18.0, 32.0},
              {"wifi-offload", 1.4, 90.0, 24.0},
          }
      }),
      detector_(5) {}

std::vector<UserEquipment> Simulation::usersForTick(std::uint64_t tick) const {
    const bool busy = tick >= 5 && tick <= 8;
    const bool mobility = tick == 7 || tick == 8;
    return {
        {"ue-drone-ctrl", RadioTech::SixGResearch, "urllc-control", busy ? 150.0 : 84.0, mobility ? 0.58 : 0.91, mobility},
        {"ue-xr-01", RadioTech::Nr5g, "embb-video", busy ? 340.0 : 210.0, busy ? 0.71 : 0.86, false},
        {"ue-xr-02", RadioTech::Nr5g, "embb-video", busy ? 320.0 : 180.0, busy ? 0.67 : 0.88, false},
        {"ue-sensor-a", RadioTech::Lte, "iot-sensing", 16.0, 0.74, false},
        {"ue-sensor-b", RadioTech::Lte, "iot-sensing", 12.0, 0.79, false},
        {"ue-lab-ap", RadioTech::Wifi7, "wifi-offload", busy ? 145.0 : 72.0, busy ? 0.62 : 0.81, mobility},
    };
}

std::vector<KpiSnapshot> Simulation::run(const SimulationOptions& options) {
    cell_.setEdgeCpuBudget(options.edgeCpuBudget);

    std::vector<KpiSnapshot> snapshots;
    snapshots.reserve(options.ticks);

    for (std::uint64_t tick = 1; tick <= options.ticks; ++tick) {
        auto allocations = cell_.schedule(usersForTick(tick));
        auto snapshot = cell_.collectKpis(tick, allocations);
        snapshot.alerts = detector_.inspect(snapshot);
        snapshots.push_back(snapshot);
    }

    return snapshots;
}

std::string renderTable(const std::vector<KpiSnapshot>& snapshots) {
    std::ostringstream out;
    out << "tick  cell        mbps     p95_ms  prb    edge_cpu  alerts\n";
    out << "----  ----------  -------  ------  -----  --------  -----------------------------\n";
    for (const auto& item : snapshots) {
        std::ostringstream alerts;
        for (std::size_t i = 0; i < item.alerts.size(); ++i) {
            if (i > 0) {
                alerts << ",";
            }
            alerts << item.alerts[i];
        }
        out << std::setw(4) << item.tick << "  "
            << std::left << std::setw(10) << item.cellId << std::right << "  "
            << std::setw(7) << std::fixed << std::setprecision(1) << item.throughputMbps << "  "
            << std::setw(6) << std::fixed << std::setprecision(1) << item.p95LatencyMs << "  "
            << std::setw(5) << std::fixed << std::setprecision(2) << item.prbUtilization << "  "
            << std::setw(8) << std::fixed << std::setprecision(1) << item.edgeCpuUtilization << "  "
            << (alerts.str().empty() ? "ok" : alerts.str()) << '\n';
    }
    return out.str();
}

std::string renderJsonLines(const std::vector<KpiSnapshot>& snapshots) {
    std::ostringstream out;
    for (const auto& item : snapshots) {
        out << "{\"tick\":" << item.tick
            << ",\"cell\":\"" << item.cellId
            << "\",\"throughput_mbps\":" << std::fixed << std::setprecision(1) << item.throughputMbps
            << ",\"p95_latency_ms\":" << std::fixed << std::setprecision(1) << item.p95LatencyMs
            << ",\"prb_utilization\":" << std::fixed << std::setprecision(2) << item.prbUtilization
            << ",\"edge_cpu_utilization\":" << std::fixed << std::setprecision(1) << item.edgeCpuUtilization
            << ",\"alerts\":[";
        for (std::size_t i = 0; i < item.alerts.size(); ++i) {
            if (i > 0) {
                out << ',';
            }
            out << "\"" << item.alerts[i] << "\"";
        }
        out << "]}\n";
    }
    return out.str();
}

} // namespace ranedge
