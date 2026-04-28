---
created: 2026-04-21T18:51
updated: 2026-04-27T16:39
---
# React Client

The DARTWIC React client is versioned independently from the engine.

## Compatibility

- Declares a minimum supported DARTWIC Engine version with `minEngineVersion`
- Declares a minimum supported protocol version with `minProtocolVersion` when needed

At connect time, the client asks the engine for runtime metadata and validates compatibility before continuing.
