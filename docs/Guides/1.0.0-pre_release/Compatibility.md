# Compatibility

DARTWIC compatibility is intentionally simple at the public level:

- Engine plugins declare `minEngineVersion`.
- Interface plugins declare `minInterfaceVersion`.
- Clients declare `minEngineVersion`.
- Clients may also declare `minProtocolVersion` when a transport-level breaking change happens.

That means a plugin or client should keep working on newer DARTWIC releases unless a real breaking contract change occurs.

## Examples

- `minEngineVersion: 1.0.0` means the plugin or client works with DARTWIC Engine `1.0.0` and newer.
- `minInterfaceVersion: 1.1.0` means the interface plugin needs DARTWIC Interface `1.1.0` or newer.
- `minProtocolVersion: 2` is only needed when the engine/client message contract breaks.

## Internal compatibility

DARTWIC still tracks protocol and plugin API versions internally. Those values are mainly for runtime safety and release engineering, not the main public compatibility story.
