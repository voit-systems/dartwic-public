---
created: 2026-04-27T00:19
updated: 2026-04-28T17:10
---
# Creating a Plugin

This is the shortest path to a working DARTWIC plugin package.

## Pick the plugin shape

hello
Use one of these:
- engine only
- interface only
- both engine and interface

If you are building custom UI, you want an interface plugin.

## Minimum package layout

```text
plugin.json
plugin/
  interface/
    <pluginId>/
      plugin.json
      ui/
        index.js
```

If you also ship engine code:

```text
plugin/
  engine/
    <pluginId>/
      plugin.json
      bin/
        <plugin binary>
```

## `plugin.json`

Current interface plugin API:

```json
{
  "id": "example-plugin",
  "name": "Example Plugin",
  "version": "0.1.0",
  "engine_plugins_api_version": 2,
  "interface_plugins_api_version": 2,
  "contains_engine_plugin": false,
  "contains_interface_plugin": true
}
```

## Interface plugin source

Recommended source shape:

```jsx
export const pluginMeta = {
  id: "example-plugin",
  interfacePluginApiVersion: 2,
};

export function createPlugin(hostApi) {
  const React = hostApi.React;
  const { definePlugin } = hostApi.sdk.pluginReact;

  function SettingsPage() {
    return (
      <div className="rounded-lg border border-border bg-card p-4 text-sm text-foreground">
        Example plugin settings.
      </div>
    );
  }

  return definePlugin({
    id: pluginMeta.id,
    SettingsPage,
  });
}
```

Runtime registration:

```jsx
import { createPlugin, pluginMeta } from "./index.jsx";

(function registerPlugin() {
  const registry = (window.__dartwicPluginRegistry__ = window.__dartwicPluginRegistry__ || {});
  registry[pluginMeta.id] = { createPlugin };
})();
```

## Styling

Write plain Tailwind classes directly.

Use the same semantic classes the app uses:

- `bg-card`
- `text-foreground`
- `border-border`
- `bg-muted`
- `text-muted-foreground`

You do not need a separate styling DSL.

## What an interface plugin can register

- settings pages
- module config pages
- task editors
- task metrics and secondary panels
- resource pages
- schematic nodes

## Task editors in v2

Task editors now use a host-owned header.

The plugin renders the body.

The host renders:

- `Cancel`
- `Save`
- close button

Inside `createPlugin(hostApi)`, task editors should usually use the shared hook:

```jsx
export function createPlugin(hostApi) {
  const { useTaskEditorBridge } = hostApi.sdk.pluginReact;

  function ExampleTaskEditor({ taskEditor }) {
    useTaskEditorBridge(taskEditor, {
      isDirty: true,
      canSave: true,
      saveLabel: "SAVE",
      cancelLabel: "CANCEL",
      onSave: async () => {},
      onCancel: () => {},
    });

    return <div className="p-4">Task body</div>;
  }
}
```

## Build output

Your build should produce:

```text
ui/index.js
```

That file is what the DARTWIC interface loads at runtime.

## Good next reads

- `Interface Plugin API`
- `Engine Plugin API`
