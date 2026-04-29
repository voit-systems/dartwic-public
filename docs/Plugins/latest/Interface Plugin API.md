---
created: 2026-03-31T23:14
updated: 2026-04-28T17:57
---
This page describes the current DARTWIC interface plugin API.

Current version:

- `interface_plugins_api_version: 2`

# What v2 plugins can register

- `SettingsPage`
- `PluginSettingsPage`
- `ModuleConfigPage`
- `taskEditors`
- `TaskMetricsGui`
- `TaskSecondaryGui`
- `resources`
- `schematicNodes`

# Runtime contract

The built plugin must register on:

```js
window.__dartwicPluginRegistry__
```

The runtime entry should register a `createPlugin(hostApi)` function under your plugin id.

# Minimum example

```jsx
export const pluginMeta = {
  id: "example-plugin",
  interfacePluginApiVersion: 2,
};

export function createPlugin(hostApi) {
  const React = hostApi.React;
  const {
    definePlugin,
    defineTaskEditor,
    useTaskEditorBridge,
  } = hostApi.sdk.pluginReact;

  function SettingsPage() {
    return (
      <div className="rounded-lg border border-border bg-card p-4 text-sm text-foreground">
        Example plugin settings.
      </div>
    );
  }

  function ExampleTaskEditor({ taskEditor }) {
    useTaskEditorBridge(taskEditor, {
      isDirty: false,
      canSave: false,
      saveLabel: "SAVE",
      cancelLabel: "CLOSE",
      onCancel: () => {},
    });

    return (
      <div className="rounded-lg border border-border bg-card p-4 text-sm text-muted-foreground">
        Task editor body.
      </div>
    );
  }

  return definePlugin({
    id: pluginMeta.id,
    SettingsPage,
    taskEditors: {
      "example.task": defineTaskEditor(ExampleTaskEditor),
    },
  });
}
```

# Host API

`createPlugin(hostApi)` receives:

- `React`
- `useDartwic`
- `components`
- `helpers`
- `sdk`

Common UI pieces exposed by the host include:

- `Button`
- `Input`
- `Label`
- `Select`
- `Separator`
- `Card`
- `Tabs`
- `Tooltip`
- `Checkbox`
- `Switch`

Common helpers include:

- `ChannelComboBox`
- `ConfigurableInput`
- `ManualRefreshButton`
- `convertChannelValuePathToChannelName`

# Styling

Use plain Tailwind classes directly.

Recommended semantic classes:

- `bg-card`
- `text-foreground`
- `border-border`
- `bg-muted`
- `text-muted-foreground`
- `bg-sidebar`

Use `hostApi.sdk.styling.cn` for conditional classes.

Plugins can also define additional theme variables or semantic colors, but normal class strings are still the main authoring path.

# Task editor API

Task editors are registered under `taskEditors`.

Example:

```jsx
return {
  taskEditors: {
    "modbus.write": defineTaskEditor(ModbusWriteTaskEditor),
  },
};
```

Your task editor component receives:

- `task`
- `operation`
- `liveChannels`
- `openSource`
- `onSaved`
- `onClose`
- `taskEditor`

The host owns the top header actions.

Use `useTaskEditorBridge(taskEditor, {...})` to provide:

- `onSave`
- `onCancel`
- `isDirty`
- `isSaving`
- `canSave`
- `errorMessage`
- `saveLabel`
- `cancelLabel`

# Resource registration

Plugins can register first-class resources.

Example shape:

```jsx
resources: [
  {
    name: "my_resource",
    label: "My Resource",
    type: "component",
    component: MyResourcePage,
  },
]
```

Directory-style resources can also define:

- `file_extension`
- `file_extensions`
- `excluded_file_names`
- `excluded_paths`
- `resource_on_create_file_data`
- `resource_on_create_file_dialog`
- `resource_on_delete_function`
- `context_config`

# Schematic node registration

Plugins can register schematic nodes with:

- `type`
- `component`
- `configComponent`
- `selectionIcon`
- `propDefaults`
- `dataDefaults`
- `resizable`
- `connectable`
- `useMeasuredSize`

Example:

```jsx
schematicNodes: [
  {
    type: "example_node",
    component: ExampleNode,
    configComponent: ExampleNodeConfig,
    selectionIcon: ExampleNodeIcon,
    resizable: true,
    connectable: true,
  },
]
```

# Compatibility

Interface plugins must match the interface runtime’s `interface_plugins_api_version`.

Current value:

- `2`

# Migration notes from older interface plugins

If you have older plugin code:

- replace `moduleUiPluginMeta` with `pluginMeta`
- replace `createModuleUiPlugin` with `createPlugin`
- move task Save/Cancel buttons out of the plugin footer
- register task save/cancel behavior through `taskEditor`
- keep using Tailwind classes directly
