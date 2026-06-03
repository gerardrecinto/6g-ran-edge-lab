#include "ranedge/simulation.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    ranedge::SimulationOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--json") {
            options.json = true;
        } else if (arg == "--ticks" && i + 1 < argc) {
            options.ticks = static_cast<std::uint64_t>(std::strtoull(argv[++i], nullptr, 10));
        } else if (arg == "--help") {
            std::cout << "Usage: ranedge-sim [--ticks N] [--json]\n";
            return 0;
        } else {
            std::cerr << "unknown argument: " << arg << "\n";
            return 2;
        }
    }

    ranedge::Simulation simulation;
    const auto snapshots = simulation.run(options);
    std::cout << (options.json ? ranedge::renderJsonLines(snapshots) : ranedge::renderTable(snapshots));
    return 0;
}
