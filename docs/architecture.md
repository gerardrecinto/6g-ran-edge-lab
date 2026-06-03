# Architecture

`6g-ran-edge-lab` is a small C++ lab for wireless platform observability. It does not claim to implement a real 3GPP stack. The goal is to model the kinds of tradeoffs a platform engineer sees around radio capacity, edge CPU pressure, mobility events, and service-level slices.

```mermaid
flowchart LR
    UE["UE demand model<br/>LTE, 5G NR, Wi-Fi 7, 6G research"]
    Scheduler["Weighted scheduler<br/>slice priority + CQI"]
    Cell["Cell KPI collector<br/>throughput, PRB, p95 latency"]
    Detector["Rolling anomaly detector<br/>latency, PRB, CPU, mobility"]
    Outputs["CLI table / JSONL<br/>Prometheus-ready labels"]

    UE --> Scheduler --> Cell --> Detector --> Outputs
```

## Why this shape

- C++ keeps the hot path close to the systems work: deterministic scheduling, allocation math, and low-overhead telemetry formatting.
- The simulator is intentionally dependency-light so it can run on a laptop, CI runner, edge node, or bare-metal lab host.
- DevOps artifacts live beside the code because wireless platform work usually fails at the boundary between radio software, host performance, and observability.

## Domain assumptions

- `SixGResearch` is a research-mode enum, not a production 6G implementation.
- Slice priorities approximate how control traffic, eMBB, IoT sensing, and Wi-Fi offload may compete for capacity.
- Alert names are operational signals, not vendor-specific alarms.
