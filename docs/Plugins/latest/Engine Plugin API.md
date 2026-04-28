---
created: 2026-03-31T23:14
updated: 2026-04-26T00:00
---
# Engine Plugin API

This page describes the engine-side plugin contract.

Current version:

- `engine_plugins_api_version: 2`

## What engine plugins do

Engine plugins run in the DARTWIC engine process.

Use them for:

- device drivers
- module types
- runtime logic
- native integrations

## Minimum engine package layout

```text
plugin/
  engine/
    <pluginId>/
      plugin.json
      bin/
        <plugin binary>
      module_config.json
      default_parameters.json
```

## Manifest

```json
{
  "id": "example-plugin",
  "name": "Example Plugin",
  "version": "0.1.0",
  "engine_plugins_api_version": 2,
  "interface_plugins_api_version": 2,
  "contains_engine_plugin": true,
  "contains_interface_plugin": false
}
```

## Binary contract

The engine loads a native binary and expects an exported:

```cpp
createPlugin
```

Preferred names:

- Windows: `bin/<pluginId>.dll`
- macOS: `bin/lib<pluginId>.dylib`
- Linux: `bin/lib<pluginId>.so`

## Main C++ integration points

Engine plugins inherit from:

```cpp
DARTWIC::Plugins::BasePlugin
```

Common hooks:

```cpp
virtual void onPluginLoaded();

virtual std::vector<PluginModuleType> getModuleTypes() const;

virtual DARTWIC::Modules::BaseModule* createModule(
    const std::string& module_type_id,
    nlohmann::json cfg,
    DARTWIC::API::SDK_API* drtw
);
```

## Module types

Use `getModuleTypes()` to expose one or more module types.

Typical metadata:

- `id`
- `config_path`
- `default_parameters_path`

## Compatibility

Engine plugins must match:

- `engine_plugins_api_version`

Current value:

- `2`

## Practical notes

- build Release binaries, not Debug binaries
- keep interface plugin files separate from engine files even when shipping both
- treat `plugin.json` as the compatibility contract
- use the engine SDK in `sdk/engine_plugin_sdk` as the reference starting point
