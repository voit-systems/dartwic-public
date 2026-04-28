---
created: 2026-04-21T18:51
updated: 2026-04-27T16:39
---
# Python Client

The DARTWIC Python client is versioned independently from the engine.

## Compatibility

- Declares a minimum supported DARTWIC Engine version with `MIN_ENGINE_VERSION`
- Uses the engine runtime metadata endpoint to verify compatibility after connecting

This keeps the public model simple while still protecting against true protocol mismatches.
