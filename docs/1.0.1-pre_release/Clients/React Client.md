---
updated: 2026-05-02T15:26
created: 2026-05-01
---

The React client is the quickest way to embed DARTWIC telemetry and operations into a React app.

# Current Exports

- `DartwicContext`
- `DartwicProvider`
- `useDartwic`

# What The Provider Gives You

- `client`
- `channels`
- `connect`
- `disconnect`
- `operation`
- `bindTelemetryHandler`
- `unbindTelemetryHandler`
- `sendTelemetry`
- `addChannelToTelemetry`
- `addChannelsToTelemetry`
- `removeChannelFromTelemetry`
- `removeChannelsFromTelemetry`
- `queryChannelRange`
- `upsertChannel`

# Built-In Behavior

- connect-time runtime compatibility check
- telemetry subscription tracking
- stale-state calculation for channel data
- re-subscription after reconnect

# When To Use It

Use it when you want:

- live channel views
- lightweight operator dashboards
- a React-native control surface
- direct DARTWIC operations from UI code

# Minimal Shape

Wrap your app in `DartwicProvider`, then call `useDartwic()` inside child components.

# Reference

- [React Client Reference Overview](Reference/React/Overview.md)
- [DartwicProvider](Reference/React/DartwicProvider.md)
- [useDartwic](Reference/React/useDartwic.md)
