# Bare-Metal Runbook

This is the checklist I would use before trusting this kind of wireless lab binary on a real Linux host.

## Host checks

```bash
lscpu
numactl --hardware
ip -br link
ethtool -S eth0 | head
sysctl kernel.sched_rt_runtime_us
```

## Build

```bash
make
make test
```

## Run

```bash
./build/ranedge-sim --ticks 20
./build/ranedge-sim --ticks 20 --json
./build/ranedge-sim --ticks 20 --metrics
./build/ranedge-sim --ticks 20 --otel-logs
```

## Operational notes

- Pinning, NUMA placement, and RT scheduling are intentionally not hardcoded. Those should be set by the host profile or systemd unit.
- JSONL, metrics, and OTel-shaped records are easier to ship into Fluent Bit, Vector, Loki, Prometheus, or an OpenTelemetry Collector.
- Keep radio logs, kernel counters, and simulator output separated until correlation is needed. It makes public examples safer and avoids leaking real lab identifiers.
