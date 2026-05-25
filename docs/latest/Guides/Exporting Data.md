---
updated: 2026-05-02T15:26
created: 2026-05-01
---

Export is most useful when you already know the channels and time range you want. Start focused and widen only if the result is still too small.

# Finding Data

1. find the relevant dataframe or grouping
2. search for the channel keys you actually need
3. query a small time window first
4. widen only after confirming the data shape is right

# Range Queries

Range queries are the normal way to pull history for analysis. Query one or a few channels first, use explicit start and end times, and keep the first request narrow enough to inspect quickly.

# Bucketing and Decimation

Use decimation when:

- you are plotting long spans
- you need a quick trend rather than exact replay
- you want a smaller export for external tools

Keep raw, high-density exports for cases where exact timing matters.

# CSV Export

CSV is the simplest handoff format when another team wants the data or you are moving into Python, spreadsheets, or a BI tool.

# Practical Advice

- do not export the whole world by default
- verify one sample query before launching a larger job
- prefer decimated summaries for dashboards and quick review
- prefer denser/raw exports for engineering analysis
