---
updated: 2026-04-28T17:57
created: 2026-03-15T23:16
---

# Example Summary
"""Simple Python dataframe query example for DARTWIC.
This saves a PNG so the example works even when a GUI plot window is awkward.

# Flow
- Connect to the local DARTWIC node.
- Search for a dataframe matching `tank_sim`.
- Query a time-bounded range for selected channel paths.
- Plot the reshaped channel values and save a PNG.
