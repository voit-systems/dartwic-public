---
updated: 2026-05-02T15:27
created: 2026-05-01
---

This page is the high-signal public guide to the engine plugin surface.

# Manifest Fields

```json
{
  "id": "example_device_plugin",
  "name": "Example Device Plugin",
  "version": "0.1.0",
  "minEngineVersion": "1.0.0-pre_release",
  "contains_engine_plugin": true,
  "contains_interface_plugin": false
}
```

# Native Entry Point

The engine loads the plugin binary, resolves the expected entrypoint, and creates a `BasePlugin` implementation from it.

# BasePlugin

`BasePlugin` is the root contract for plugin identity, module type registration, and plugin-owned runtime lifecycle.

# PluginModuleType

This metadata tells the engine what module types the plugin provides and how they should be surfaced.

# BaseModule

`BaseModule` is the common base for module instances created by a plugin.

# Packaging Notes

- ship the built binary in the plugin package layout expected by the engine
- match your build output to the release/debug host you plan to use

# Detailed Reference

- [BasePlugin](Reference/Engine/BasePlugin.md)
- [BaseModule](Reference/Engine/BaseModule.md)
- [PluginModuleType](Reference/Engine/PluginModuleType.md)
- [SDK API](Reference/Engine/SDK%20API.md)
