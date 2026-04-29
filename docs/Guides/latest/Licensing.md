---
created: 2026-04-27T21:46
updated: 2026-04-28T17:57
---
DARTWIC Engine 1.0 uses an offline-first signed license file.

# How it works

- The engine loads a local `license.json` file at startup.
- The license payload is signed with a private key that is not stored in the engine.
- The engine embeds the matching public key and verifies the signature locally.
- Any edit to the payload invalidates the signature and prevents the engine from starting in release mode.

# Deployment binding

Licenses are bound to a deployment ID, not just an organization name. The engine compares the deployment ID in the signed payload against the configured local deployment ID.

# Offline behavior

Normal license validation is fully local. The engine does not need a constant online connection to remain functional.

# Typical payload fields

- `licenseId`
- `customer`
- `tier`
- `deploymentId`
- `issuedAt`
- `expiresAt`
- `entitlements`

# Operator-facing failures

- Invalid signature
- Expired license
- Deployment mismatch
- Missing license
- Over-limit module or channel configuration
