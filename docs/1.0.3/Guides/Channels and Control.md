---
updated: 2026-05-02T15:26
created: 2026-05-01
---

Operators rarely need every RAPID field. In normal use, a much smaller set is enough to judge freshness, writeability, and safety.

# The Fields That Matter Most

- `value`
- `timestamp`
- `stale_timeout`
- `record_mode`
- `control_policy`

# Record Modes

Use record state to understand whether a channel is live-only or part of persisted history. If you plan to export or trend data later, make sure the channels you care about are actually being recorded.

# Control Policies

Control policy tells you whether a write is allowed and helps explain why a control is visible but not currently actionable.

# Manual Override

Manual override behavior depends on deployment, but operators should always confirm:

- override is intentionally enabled
- the target is correct
- downstream automation will not immediately undo the change

# Operator Guidance

- never treat a visible control as automatically safe to use
- confirm freshness before acting on a displayed value
- confirm authority before sending commands
- if control state and live telemetry disagree, stop and resolve that mismatch first
