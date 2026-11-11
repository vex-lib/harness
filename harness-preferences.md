# harness — Repo-Local Living Preferences

## Constitution Link

gist.md (`../../metadata/gists/preferences/gist.md`) is the compressed law list; all universal laws bind this repository. Read one on demand: `./tools/prefs law "<title>"`. Universal laws are inherited, never restated.

Tier: core (R4 agent interface). Owns: agent policy, history and interface state. Borrows: Relational Engine memory/IO, Vexspoke CPU/behavior, api-haven connectors as-is, darling-framework for interface.

## Exclusive Laws

| Law Title | Scope | Enforcement |
| **Harness Host Boundary Law** | R4 agent interface | Mandatory for `harness` |
| **Agent Integration Law** | R4 agent execution | Mandatory for `harness` |

### Harness Host Boundary Law

* `harness` is the user's own agent, an R4 interface integrating this ecosystem, not a third-party agent host.
* Borrows Relational Engine memory/IO and Vexspoke CPU/behavior.
* Consumes `api-haven` connectors plus `harness_run`/`harness_poll` as-is through opaque handles and fn-tables.
* Composes `darling-framework`; drives R5 applications through registered seams.
* Owns no connector, transport, session, GPU or OS implementation.
* Never include R5 engine or supervisor headers; R1/R2/R3 never include harness.
* Invoke personal tools `func` and `b` as bounded child processes; never `#include` them.

### Agent Integration Law

* The agent operates the user's own projects, external tools and R5 applications through registered ecosystem contracts.
* Orchestration uses `func` compile-time opcodes and compiled native artifacts, never a shipped interpreted runtime.
* Each external action is a bounded child: 100 ms reap slices, `SIGTERM` cancel; no unbounded wait or `system()`.
* Validate agent input once at the cold seam; failures reject loudly (`THROW`).
* Reach projects, tools and R5 apps through declared seams (opaque handle + fn-table), never another tier's internals.

## Repo-Local Extensions

* None — no managed exceptions to universal laws.

## Readiness Cross-Reference

* Feature readiness: harness readiness gist.
* Open blockers: ecosystem blockers gist.
* Proof lawbook: `tests/test-preferences.md` — `./tools/prefs test "<title>"`.
* Evidence ledger: `metadata/gists/test-checklist.md` — `python3 tools/test_checklist.py check`.
