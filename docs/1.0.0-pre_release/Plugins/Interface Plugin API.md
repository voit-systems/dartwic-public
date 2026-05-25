---
updated: 2026-05-02T15:27
created: 2026-05-01
---

This page is the high-signal public guide to the interface plugin surface.

# Manifest Fields

```json
{
  "id": "example_device_plugin",
  "name": "Example Device Plugin",
  "version": "0.1.0",
  "minInterfaceVersion": "1.0.0-pre_release",
  "contains_engine_plugin": false,
  "contains_interface_plugin": true
}
```

# Runtime Contract

- the built interface bundle is loaded from `ui/index.js`
- runtime registration uses the interface SDK
- a plugin can expose a concrete definition or a host-created factory

# Host API

`createPlugin(hostApi)` gives access to the host React runtime, common UI components, helper utilities, and the public plugin SDK surface.

# Plugin Definition Shape

The public shape usually centers on:

- `id`
- `name`
- `taskCards`
- `taskConfigs`
- `resources`
- `pluginSettings`
- `moduleConfigs`
- `schematicNodes`

# Task Surfaces

Task definitions usually separate compact task cards from richer task configuration surfaces.

# Resource Definitions

Resources are best for plugin-provided pages, file-backed tools, and project-specific utilities.

# What Interface Plugins Are Best At

- operator workflows
- focused project pages
- configuration UI
- schematic node UX

# Detailed Reference

- [definePlugin](Reference/Interface/definePlugin.md)
- [Host API](Reference/Interface/Host%20API.md)
- [Plugin Definition](Reference/Interface/Plugin%20Definition.md)
- [Tasks](Reference/Interface/Tasks.md)
- [Resources](Reference/Interface/Resources.md)
- [Module Configs](Reference/Interface/Module%20Configs.md)
- [Plugin Settings](Reference/Interface/Plugin%20Settings.md)
- [Schematic Nodes](Reference/Interface/Schematic%20Nodes.md)
- [Hooks and Utilities](Reference/Interface/Hooks%20and%20Utilities.md)
- [Runtime Registration](Reference/Interface/Runtime%20Registration.md)
