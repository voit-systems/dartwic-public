---
updated: 2026-05-02T15:26
created: 2026-05-01
---

The interface is meant to help operators stay oriented, act deliberately, and move between live telemetry, tasks, and resources without losing context.

# What The Interface Is Good At

- watching a focused set of live channels
- moving from telemetry into action
- surfacing plugin-provided workflows
- giving operators the few diagnostics that matter most

# Main Concepts

- `Channels` are the live values and states you monitor.
- `Tasks` are operator workflows or action surfaces.
- `Resources` are plugin-provided pages, files, or utilities.
- `Diagnostics` help you decide whether the data is current and trustworthy.

# What To Check Early

- connection status
- whether timestamps are advancing
- whether the values you care about are stale
- whether control authority is available before sending commands

# Good Habits

- pin or search into the smallest useful channel set
- treat stale data as a workflow problem first, not just a visual problem
- keep exports and wide queries intentional so the interface stays responsive
