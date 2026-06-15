---
updated: 2026-05-02T15:27
created: 2026-05-01
---

DARTWIC plugins extend either the engine, the interface, or both.

# Two Plugin Surfaces

- `Engine plugins` add native runtime logic, module types, and hardware-facing behavior.
- `Interface plugins` add React UI, resources, task editors, settings, and schematic nodes.

# Public Starter Path

1. Read [Creating a Plugin](Creating%20a%20Plugin.md).
2. Start from `sdk/dartwic-example-plugin`.
3. Pick engine, interface, or both.
4. Build one small feature first.

# Reference

- [Engine Reference Overview](Reference/Engine/Overview.md)
- [Interface Reference Overview](Reference/Interface/Overview.md)
- [BasePlugin](Reference/Engine/BasePlugin.md)
- [Host API](Reference/Interface/Host%20API.md)

# What The Example Plugin Gives You

- combined manifest structure
- an engine entrypoint
- an interface entrypoint
- example configuration surfaces
- packaging and deployment scaffolding
