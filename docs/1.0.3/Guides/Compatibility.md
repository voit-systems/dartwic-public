---
updated: 2026-05-02T15:26
created: 2026-05-01
---

DARTWIC compatibility is primarily version-floor based. Public clients and plugins declare the minimum host versions they expect.

# Main Rules

- clients can reject engines that are too old
- plugins declare minimum engine and/or interface versions
- version mismatches should fail early rather than degrade silently

# What This Means In Practice

If something connects or loads but behaves strangely, check version compatibility before spending time on deeper debugging.

# Runtime Checks

- the Python client checks runtime metadata after connect
- interface surfaces can gate plugin loading by minimum interface version
- plugin packages should treat minimum versions as public compatibility contracts
