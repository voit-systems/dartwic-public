---
created: 2026-03-31T23:14
updated: 2026-04-13T01:02
---
# Interface Plugin API

This document is for developers who want to create DARTWIC interface plugins.

Interface plugins add UI behavior to the DARTWIC interface. They can provide settings pages, task/module UI, helper panels, resource UI, schematic UI, and other interface-side features.

## What Interface Plugins Are

An interface plugin is JavaScript that runs inside the DARTWIC interface runtime.

It is installed as files and loaded by the interface when needed.

Interface plugins are different from engine plugins:

- Interface plugins run in the GUI.
- Engine plugins run in the engine process.
- A plugin package can contain one side or both sides.

## Minimum Plugin Files

An installed interface plugin must contain:

```text
plugin.json
ui/
  index.js
```

When packaged in `plugin.zip`, interface files should be located at:

```text
plugin/
  interface/
    <pluginId>/
      plugin.json
      ui/
        index.js
```

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
  "contains_engine_plugin": false,
  "contains_interface_plugin": true
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

For interface-only plugins:

```json
{
  "contains_engine_plugin": false,
  "contains_interface_plugin": true
}
```

For plugins that also include engine files:

```json
{
  "contains_engine_plugin": true,
  "contains_interface_plugin": true
}
```

## Runtime Entry File

The interface loads:

```text
ui/index.js
```

This file should register your plugin on:

```text
window.__dartwicPluginRegistry__
```

Example:

```js
(function () {
  const pluginId = "example-plugin";

  window.__dartwicPluginRegistry__ = window.__dartwicPluginRegistry__ || {};
  window.__dartwicPluginRegistry__[pluginId] = {
    createPlugin(hostApi) {
      const { React, components } = hostApi;
      const { Button } = components;

      function PluginSettingsPage() {
        return React.createElement(Button, null, "Hello from plugin");
      }

      return {
        id: pluginId,
        name: "Example Plugin",
        PluginSettingsPage,
      };
    },
  };
})();
```

## Recommended Source Exports

If you are developing with the interface plugin SDK, your source can expose:

```js
export const pluginMeta = {
  id: "example-plugin",
  name: "Example Plugin",
};

export function createPlugin(hostApi) {
  return {
    id: pluginMeta.id,
    name: pluginMeta.name,
  };
}
```

Your build step should produce a browser script at:

```text
ui/index.js
```

that registers the plugin globally.

## Host API

`createPlugin(hostApi)` receives a controlled API from the DARTWIC interface.

The host API may include:

- `React`
- `useDartwic`
- Shared UI components
- Shared helper components
- Channel helper utilities

Common components include:

- `Button`
- `Input`
- `Label`
- `Select`
- `Separator`
- Dialog title/header/footer/description components

Common helpers include:

- `ChannelComboBox`
- `convertChannelValuePathToChannelName`

Prefer using `hostApi` instead of importing internal DARTWIC interface files directly.

## Plugin Object

Your plugin factory returns a plugin object.

Common fields:

```js
{
  id: "example-plugin",
  name: "Example Plugin",
  taskTypes: ["example-task"],
  PluginSettingsPage,
  SettingsPage,
  ModuleUiComponent,
  TaskUiComponent,
}
```

Not every plugin needs every field.

Use only the fields required by the UI extension point you are targeting.

## Settings Pages

To add a plugin settings page, return:

```js
{
  PluginSettingsPage
}
```

or:

```js
{
  SettingsPage
}
```

The settings page should be a React component.

## Task Or Module UI

To provide UI for a task or module type, return the relevant UI component and, if needed, declare supported task types.

Example:

```js
return {
  id: "example-plugin",
  name: "Example Plugin",
  taskTypes: ["example-task"],
  TaskUiComponent,
};
```

If `taskTypes` is present, DARTWIC only uses the plugin for matching task types.

## Packaging

Your release should include:

```text
plugin.json
plugin.zip
```

`plugin.zip` should contain:

```text
plugin/interface/<pluginId>/plugin.json
plugin/interface/<pluginId>/ui/index.js
```

If the package also includes engine files, it may also contain:

```text
plugin/engine/<pluginId>/...
```

## Compatibility

Interface plugins must match:

```text
interface_plugins_api_version
```

If the installed DARTWIC interface expects a different interface plugin API version, the plugin may not be installable.

## Development SDK

The interface plugin SDK lives at:

```text
sdk/interface_plugin_sdk
```

The example package lives at:

```text
sdk/example_package
```

The SDK contains DARTWIC-specific files and conventions. It does not force a specific bundler or package manager.

## Debug Checklist

If your plugin does not install:

- Confirm `plugin.json` is valid.
- Confirm `contains_interface_plugin` is `true`.
- Confirm `interface_plugins_api_version` matches DARTWIC.
- Confirm `plugin.zip` contains `plugin/interface/<pluginId>/`.

If your plugin installs but does not load:

- Confirm `ui/index.js` exists.
- Confirm `ui/index.js` registers on `window.__dartwicPluginRegistry__`.
- Confirm the registry key exactly matches your plugin id.
- Check the browser console.

If your plugin loads but UI does not appear:

- Confirm the expected component field is returned.
- Confirm `taskTypes` matches the requested task type, if used.
- Confirm your React component does not throw during render.
