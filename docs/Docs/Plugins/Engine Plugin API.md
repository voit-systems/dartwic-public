---
created: 2026-03-31T23:14
updated: 2026-04-13T01:02
---
# Engine Plugin API

This document is for developers who want to create DARTWIC engine plugins.

Engine plugins add native engine-side behavior to DARTWIC. They can expose repeatable module types, device integrations, drivers, and runtime logic used by the engine.

## What Engine Plugins Are

An engine plugin is a native library loaded by the DARTWIC engine.

Engine plugins are different from interface plugins:

- Engine plugins run in the engine process.
- Interface plugins run in the GUI.
- A plugin package can contain one side or both sides.

## Minimum Plugin Files

An installed engine plugin usually contains:

```text
plugin.json
bin/
  <plugin binary>
module_config.json
default_parameters.json
```

When packaged in `plugin.zip`, engine files should be located at:

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

> [!warning] WARNING
> Ensure you build your Engine Plugin in CMake Release profile, or it will not be loaded by the engine properly.


## Manifest

The plugin manifest is `plugin.json`.

Example:

```json
{
  "id": "example-plugin",
  "name": "Example Plugin",
  "version": "1.0.0",
  "engine_plugins_api_version": 1,
  "interface_plugins_api_version": 1,
  "contains_engine_plugin": true,
  "contains_interface_plugin": false
}
```

Required fields:

- `id`
- `name`
- `version`
- `engine_plugins_api_version`
- `interface_plugins_api_version`
- `contains_engine_plugin`
- `contains_interface_plugin`

For engine-only plugins:

```json
{
  "contains_engine_plugin": true,
  "contains_interface_plugin": false
}
```

For plugins that also include interface files:

```json
{
  "contains_engine_plugin": true,
  "contains_interface_plugin": true
}
```

## Binary Naming

Preferred binary names:

Windows:

```text
bin/<pluginId>.dll
```

macOS:

```text
bin/lib<pluginId>.dylib
```

Linux:

```text
bin/lib<pluginId>.so
```

The binary must export:

```cpp
createPlugin
```

## BasePlugin

Engine plugins inherit from:

```cpp
DARTWIC::Plugins::BasePlugin
```

Common methods:

```cpp
virtual void onPluginLoaded();

virtual std::vector<PluginModuleType> getModuleTypes() const;

virtual DARTWIC::Modules::BaseModule* createModule(
    const std::string& module_type_id,
    nlohmann::json cfg,
    DARTWIC::API::SDK_API* drtw
);
```

Use `onPluginLoaded` for startup behavior.

Use `getModuleTypes` to declare repeatable module types.

Use `createModule` to create module instances.

## Module Types

A plugin can expose one or more module types.

Module type shape:

```cpp
struct PluginModuleType {
    std::string id;
    std::string config_path = "module_config.json";
    std::string default_parameters_path = "default_parameters.json";
};
```

Example:

```cpp
std::vector<DARTWIC::Plugins::PluginModuleType> getModuleTypes() const override {
    return {{
        "example-module",
        "module_config.json",
        "default_parameters.json"
    }};
}
```

## BaseModule

Modules inherit from:

```cpp
DARTWIC::Modules::BaseModule
```

Useful helpers:

```cpp
getConfig<T>(key, defaultValue)
getParameter<T>(key, defaultValue)
```

Use modules for repeatable runtime objects created from your plugin's module types.

## SDK API

Plugins and modules receive:

```cpp
DARTWIC::API::SDK_API* dartwic
```

This is the host API pointer used to interact with DARTWIC.

Prefer using the SDK headers shipped in:

```text
sdk/engine_plugin_sdk
```

## Packaging

Your release should include:

```text
plugin.json
plugin.zip
```

`plugin.zip` should contain:

```text
plugin/engine/<pluginId>/plugin.json
plugin/engine/<pluginId>/bin/<plugin binary>
```

If the package also includes interface files, it may also contain:

```text
plugin/interface/<pluginId>/...
```

## Compatibility

Engine plugins must match:

```text
engine_plugins_api_version
```

If the installed DARTWIC engine expects a different engine plugin API version, the plugin may not install or load.

## Restart Requirement

Engine plugins are native libraries.

After installing or uninstalling engine plugin files, the engine may need to restart before changes fully take effect.

This is normal because native libraries may remain loaded in the running process.

## Development SDK

The engine plugin SDK lives at:

```text
sdk/engine_plugin_sdk
```

The example package lives at:

```text
sdk/example_package
```

The SDK contains DARTWIC-specific headers and starter files. It does not force a specific CMake or native build setup.

## Debug Checklist

If your plugin does not install:

- Confirm `plugin.json` is valid.
- Confirm `contains_engine_plugin` is `true`.
- Confirm `engine_plugins_api_version` matches DARTWIC.
- Confirm `plugin.zip` contains `plugin/engine/<pluginId>/`.

If your plugin installs but does not load:

- Confirm the binary is under `bin/`.
- Confirm the binary name matches the platform convention.
- Confirm the binary exports `createPlugin`.
- Confirm dependencies can be found by the engine.
- Restart the engine.

If your module type does not appear:

- Confirm `getModuleTypes` returns it.
- Confirm the plugin loaded successfully.
- Confirm `module_config.json` is valid.

If module creation fails:

- Confirm `createModule` handles the requested module type id.
- Confirm your module constructor accepts the provided config and SDK API pointer.
- Check engine logs for plugin errors.
