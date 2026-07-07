#include "ranedge/simulation.hpp"
#include "ranedge/telemetry.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

enum class OutputMode {
    Table,
    Json,
    Prometheus,
    OtelLogs,
    OtelTraces
};

} // namespace

int main(int argc, char** argv) {
    ranedge::SimulationOptions options;
    OutputMode mode = OutputMode::Table;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--json") {
            mode = OutputMode::Json;
        } else if (arg == "--metrics") {
            mode = OutputMode::Prometheus;
        } else if (arg == "--otel-logs") {
            mode = OutputMode::OtelLogs;
        } else if (arg == "--otel-traces") {
            mode = OutputMode::OtelTraces;
        } else if (arg == "--ticks" && i + 1 < argc) {
            char* end = nullptr;
            const auto parsed = std::strtoull(argv[++i], &end, 10);
            if (end == nullptr || *end != '\0' || parsed == 0) {
                std::cerr << "--ticks expects a positive integer, got: " << argv[i] << "\n";
                return 2;
            }
            options.ticks = static_cast<std::uint64_t>(parsed);
        } else if (arg == "--help") {
            std::cout << "Usage: ranedge-sim [--ticks N] [--json|--metrics|--otel-logs|--otel-traces]\n";
            return 0;
        } else {
            std::cerr << "unknown argument: " << arg << "\n";
            return 2;
        }
    }

    ranedge::Simulation simulation;
    const auto snapshots = simulation.run(options);
    switch (mode) {
        case OutputMode::Json:
            std::cout << ranedge::renderJsonLines(snapshots);
            break;
        case OutputMode::Prometheus:
            std::cout << ranedge::renderPrometheus(snapshots);
            break;
        case OutputMode::OtelLogs:
            std::cout << ranedge::renderOtelLogs(snapshots);
            break;
        case OutputMode::OtelTraces:
            std::cout << ranedge::renderOtelTraces(snapshots);
            break;
        case OutputMode::Table:
            std::cout << ranedge::renderTable(snapshots);
            break;
    }
    return 0;
}
