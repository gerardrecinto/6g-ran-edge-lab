#include "ranedge/simulation.hpp"
#include "ranedge/telemetry.hpp"

#include <cerrno>
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
            const std::string value = argv[++i];
            // strtoull silently accepts a leading '-' and wraps it into a huge
            // unsigned value (e.g. "-5" becomes ~1.8e19), which used to blow
            // past the ticks == 0 check and crash later on vector::reserve
            // with std::length_error. Reject non-digit input up front instead.
            if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos) {
                std::cerr << "--ticks expects a positive integer, got: " << value << "\n";
                return 2;
            }
            errno = 0;
            char* end = nullptr;
            const auto parsed = std::strtoull(value.c_str(), &end, 10);
            if (end == nullptr || *end != '\0' || parsed == 0 || errno == ERANGE) {
                std::cerr << "--ticks expects a positive integer, got: " << value << "\n";
                return 2;
            }
            options.ticks = static_cast<std::uint64_t>(parsed);
        } else if (arg == "--edge-cpu-budget" && i + 1 < argc) {
            const std::string value = argv[++i];
            char* end = nullptr;
            errno = 0;
            const double parsed = std::strtod(value.c_str(), &end);
            if (end == nullptr || *end != '\0' || value.empty() || parsed <= 0.0 || errno == ERANGE) {
                std::cerr << "--edge-cpu-budget expects a positive number, got: " << value << "\n";
                return 2;
            }
            options.edgeCpuBudget = parsed;
        } else if (arg == "--help") {
            std::cout << "Usage: ranedge-sim [--ticks N] [--edge-cpu-budget F] "
                         "[--json|--metrics|--otel-logs|--otel-traces]\n";
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
