#include "ranedge/simulation.hpp"
#include "ranedge/telemetry.hpp"

#include <cassert>

int main() {
    ranedge::Simulation simulation;
    const auto snapshots = simulation.run({8});

    const auto metrics = ranedge::renderPrometheus(snapshots);
    assert(metrics.find("# TYPE ranedge_throughput_mbps gauge") != std::string::npos);
    assert(metrics.find("ranedge_alert_total") != std::string::npos);

    const auto logs = ranedge::renderOtelLogs(snapshots);
    assert(logs.find("\"service.name\":\"ranedge-sim\"") != std::string::npos);
    assert(logs.find("\"severity_text\":\"WARN\"") != std::string::npos);
    assert(logs.find("\"ran.p95_latency_ms\"") != std::string::npos);

    const auto traces = ranedge::renderOtelTraces(snapshots);
    assert(traces.find("\"name\":\"ran.scheduler.tick\"") != std::string::npos);
    assert(traces.find("\"trace_id\"") != std::string::npos);
    assert(traces.find("\"events\"") != std::string::npos);

    return 0;
}
