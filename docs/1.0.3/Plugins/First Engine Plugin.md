---
updated: 2026-05-02T15:27
created: 2026-05-01
---

This page is the fastest path to a small, working engine plugin.

# Recommended Layout

```text
my-plugin/
  plugin.json
  engine/
    src/
      plugin.cpp
```

# Required Export

An engine plugin must expose the native entrypoint expected by the engine host and return a `BasePlugin` implementation the host can load.

# Minimal Shape

Your first engine plugin should usually define:

- one plugin class derived from `BasePlugin`
- one small module type if needed
- minimal metadata needed for discovery and loading

# First Success

Aim for the smallest real milestone:

- the plugin loads
- the engine sees the plugin metadata
- one module type or behavior is available
