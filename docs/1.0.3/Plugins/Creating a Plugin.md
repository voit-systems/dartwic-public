---
updated: 2026-05-02T15:27
created: 2026-05-01
---

This is the shortest public path to a working plugin package.

# Choose The Shape

- engine only
- interface only
- both

If you are building operator UI, start with an interface plugin. If you are integrating native runtime behavior or devices, start with an engine plugin.

# Start From The Example

Canonical starter:

- `sdk/dartwic-example-plugin`

Important pieces:

- `plugin.json`
- `engine/`
- `interface/`
- `plugin/`
- `deployment-settings.json`

# Top-Level Manifest

```json
{
  "id": "example_device_plugin",
  "name": "Example Device Plugin",
  "version": "0.1.0",
  "minEngineVersion": "1.0.0-pre_release",
  "minInterfaceVersion": "1.0.0-pre_release",
  "contains_engine_plugin": true,
  "contains_interface_plugin": true
}
```

# Build Outputs

- engine release output: `plugin/engine/<plugin-id>/`
- engine debug output: `plugin/engine-debug/<plugin-id>/`
- interface output: `plugin/interface/<plugin-id>/ui/index.js`

# Next Reads

- [First Interface Plugin](First%20Interface%20Plugin.md)
- [First Engine Plugin](First%20Engine%20Plugin.md)
- [Interface Plugin API](Interface%20Plugin%20API.md)
- [Engine Plugin API](Engine%20Plugin%20API.md)
- [Engine Reference Overview](Reference/Engine/Overview.md)
- [Interface Reference Overview](Reference/Interface/Overview.md)
