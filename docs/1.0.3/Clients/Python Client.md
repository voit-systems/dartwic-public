---
updated: 2026-05-02T15:26
created: 2026-05-01
---

The Python client is the simplest way to script against a running DARTWIC engine.

# What It Wraps

It builds on the TEMPEST Python client and adds DARTWIC-shaped helpers for:

- runtime compatibility checks
- dataframe search
- channel-key search
- range queries

# Main Class

`DartwicClient`

Useful methods:

- `connect(ip="127.0.0.1", port=7000, password="")`
- `disconnect()`
- `operation(name, payload=None, timeout_ms=5000)`
- `search_channel_dataframes(query="", limit=100)`
- `get_channel_keys(query="", limit=1000)`
- `query_channel_range(series, from_timestamp=None, to_timestamp=None, data_frame="", bucket_count=None, timeout_ms=10000)`

# Compatibility

- the package exports `MIN_ENGINE_VERSION`
- the client checks runtime metadata after connect
- version mismatch should fail early

# Why Use The Helpers

The helpers normalize common DARTWIC query flows so you can script quickly without rebuilding the same payload handling each time.

# Typical Workflow

1. Connect.
2. Find the relevant dataframe.
3. Search for the channel keys.
4. Query a bounded range.
5. Drop to `operation(...)` when you need lower-level access.

# Example

See [Python Query Example](../Guides/Examples/Python%20Query%20Example.md).

# Reference

- [Python Client Reference Overview](Reference/Python/Overview.md)
- [DartwicClient](Reference/Python/DartwicClient.md)
