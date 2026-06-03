#include "ranedge/telemetry.hpp"

#include <iomanip>
#include <sstream>

namespace ranedge {

namespace {

std::string severityFor(const KpiSnapshot& snapshot) {
    return snapshot.alerts.empty() ? "INFO" : "WARN";
}

std::string alertList(const KpiSnapshot& snapshot) {
    std::ostringstream out;
    for (std::size_t i = 0; i < snapshot.alerts.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        out << snapshot.alerts[i];
    }
    return out.str();
}

std::string hexId(std::uint64_t seed, std::size_t width) {
    std::ostringstream out;
    out << std::hex << std::setfill('0') << std::setw(static_cast<int>(width)) << seed;
    return out.str();
}

} // namespace

std::string renderPrometheus(const std::vector<KpiSnapshot>& snapshots) {
    std::ostringstream out;
    out << "# HELP ranedge_throughput_mbps Synthetic cell throughput in Mbps.\n";
    out << "# TYPE ranedge_throughput_mbps gauge\n";
    out << "# HELP ranedge_p95_latency_ms Synthetic p95 scheduler latency in milliseconds.\n";
    out << "# TYPE ranedge_p95_latency_ms gauge\n";
    out << "# HELP ranedge_prb_utilization Synthetic physical resource block utilization ratio.\n";
    out << "# TYPE ranedge_prb_utilization gauge\n";
    out << "# HELP ranedge_edge_cpu_utilization Synthetic edge CPU utilization percentage.\n";
    out << "# TYPE ranedge_edge_cpu_utilization gauge\n";
    out << "# HELP ranedge_alert_total Synthetic alert count by alert name.\n";
    out << "# TYPE ranedge_alert_total counter\n";

    for (const auto& item : snapshots) {
        out << "ranedge_throughput_mbps{cell=\"" << item.cellId << "\",tick=\"" << item.tick << "\"} "
            << std::fixed << std::setprecision(1) << item.throughputMbps << '\n';
        out << "ranedge_p95_latency_ms{cell=\"" << item.cellId << "\",tick=\"" << item.tick << "\"} "
            << std::fixed << std::setprecision(1) << item.p95LatencyMs << '\n';
        out << "ranedge_prb_utilization{cell=\"" << item.cellId << "\",tick=\"" << item.tick << "\"} "
            << std::fixed << std::setprecision(2) << item.prbUtilization << '\n';
        out << "ranedge_edge_cpu_utilization{cell=\"" << item.cellId << "\",tick=\"" << item.tick << "\"} "
            << std::fixed << std::setprecision(1) << item.edgeCpuUtilization << '\n';
        for (const auto& alert : item.alerts) {
            out << "ranedge_alert_total{cell=\"" << item.cellId << "\",alert=\"" << alert << "\"} 1\n";
        }
    }
    return out.str();
}

std::string renderOtelLogs(const std::vector<KpiSnapshot>& snapshots) {
    std::ostringstream out;
    for (const auto& item : snapshots) {
        out << "{\"resource\":{\"service.name\":\"ranedge-sim\",\"service.version\":\"0.1.0\"},"
            << "\"scope\":\"ranedge.scheduler\","
            << "\"severity_text\":\"" << severityFor(item) << "\","
            << "\"body\":\"cell kpi snapshot\","
            << "\"attributes\":{"
            << "\"cell.id\":\"" << item.cellId << "\","
            << "\"radio.access\":\"6g-research\","
            << "\"tick\":" << item.tick << ','
            << "\"ran.throughput_mbps\":" << std::fixed << std::setprecision(1) << item.throughputMbps << ','
            << "\"ran.p95_latency_ms\":" << std::fixed << std::setprecision(1) << item.p95LatencyMs << ','
            << "\"ran.prb_utilization\":" << std::fixed << std::setprecision(2) << item.prbUtilization << ','
            << "\"edge.cpu_utilization\":" << std::fixed << std::setprecision(1) << item.edgeCpuUtilization << ','
            << "\"alerts\":\"" << alertList(item) << "\""
            << "}}\n";
    }
    return out.str();
}

std::string renderOtelTraces(const std::vector<KpiSnapshot>& snapshots) {
    std::ostringstream out;
    for (const auto& item : snapshots) {
        const auto traceSeed = 0x6a00000000000000ULL + item.tick;
        const auto spanSeed = 0x5100000000000000ULL + item.tick;
        out << "{\"resource\":{\"service.name\":\"ranedge-sim\"},"
            << "\"trace_id\":\"" << hexId(traceSeed, 16) << hexId(traceSeed + 17, 16) << "\","
            << "\"span_id\":\"" << hexId(spanSeed, 16) << "\","
            << "\"parent_span_id\":\"0000000000000000\","
            << "\"name\":\"ran.scheduler.tick\","
            << "\"kind\":\"SPAN_KIND_INTERNAL\","
            << "\"status\":\"" << (item.alerts.empty() ? "OK" : "ERROR") << "\","
            << "\"attributes\":{"
            << "\"cell.id\":\"" << item.cellId << "\","
            << "\"tick\":" << item.tick << ','
            << "\"ran.prb_utilization\":" << std::fixed << std::setprecision(2) << item.prbUtilization << ','
            << "\"ran.p95_latency_ms\":" << std::fixed << std::setprecision(1) << item.p95LatencyMs << ','
            << "\"alert.count\":" << item.alerts.size()
            << "},\"events\":[";
        for (std::size_t i = 0; i < item.alerts.size(); ++i) {
            if (i > 0) {
                out << ',';
            }
            out << "{\"name\":\"" << item.alerts[i] << "\",\"attributes\":{\"cell.id\":\"" << item.cellId << "\"}}";
        }
        out << "]}\n";
    }
    return out.str();
}

} // namespace ranedge
