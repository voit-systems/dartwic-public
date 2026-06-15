---
updated: 2026-05-02T15:26
created: 2026-05-01
---

This page is the shortest path from first launch to a useful session.

# What You Need

- a running DARTWIC engine
- a reachable interface
- access to the channels or modules you care about
- permission to control hardware if you plan to write commands

# First Session

1. Connect to the target system.
2. Open the channels you care about first.
3. Confirm timestamps are moving and values are fresh.
4. Check control state before attempting any write.
5. Save or export only the data slice you actually need.

# First Things To Learn

- how live channels are named
- which channels are read-only versus commandable
- what stale data looks like
- where tasks and resources live in your interface build
- what level of command authority your session has

# Recommended Mental Model

DARTWIC is best used as a live operations and analysis workspace, not as a giant raw-data browser. Start narrow, confirm freshness, then expand into tasks, exports, or plugin surfaces as needed.

# If You Are Building Against DARTWIC

- [Python Client](../Clients/Python%20Client.md)
- [React Client](../Clients/React%20Client.md)
- [Plugins Overview](../Plugins/Overview.md)
