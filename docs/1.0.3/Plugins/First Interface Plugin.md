---
updated: 2026-05-02T15:27
created: 2026-05-01
---

This page is the fastest path to a small, working interface plugin.

# Recommended Layout

```text
my-plugin/
  plugin.json
  interface/
    src/
      index.ts
      runtime.ts
```

# Minimal Plugin Definition

Your first interface plugin should usually export:

- `id`
- `name`
- one small resource, task, or settings surface

Keep the first version narrow.

# Runtime Entry

Register the built plugin through the interface SDK runtime helpers so the host can create it with the current host API.

# What To Add First

Good first targets:

- a small resource page
- a focused task card
- a simple settings panel

# Host Surfaces You Will Use Most

- `definePlugin`
- host `React`
- host UI components like `Button`, `Input`, `Card`
- helper components like `ChannelComboBox`
- task/resource registration types

# Good First Target

Build one small operator workflow that answers a real question or performs one action clearly.
