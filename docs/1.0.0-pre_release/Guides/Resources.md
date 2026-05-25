---
updated: 2026-05-02T15:26
created: 2026-05-01
---

Resources are how DARTWIC surfaces plugin-provided pages, tools, and file-backed workflows without forcing everything into the same task model.

# What Resources Are For

- custom operator pages
- project-specific tooling
- file-backed workflows
- small utilities that do not belong in the main task flow

# Public Expectations

A good public resource should be easy to discover and focused on one job. Operators should not need to understand the whole plugin system to use a resource page safely.

# Resource Plugins

If you are building resources:

- keep labels explicit
- keep scope narrow
- prefer obvious create/open/delete behavior
- document file expectations when resources manage on-disk content

For implementation details, see [Resources](../Plugins/Reference/Interface/Resources.md).
