# harness

R4 agent interface for Vexgraph: the user's own coding-agent host integrating `api-haven` connectors, `func` tools, and `darling` UI.

```bash
python3 tests/harness/run.py
./tools/b test <unit>
```

## About

`harness` is the R4 agent layer designed to operate projects, external tools, and R5 applications through registered seams (opaque handles and function tables) instead of relying on third-party agent hosts. It consumes `api-haven` as-is for AI providers and MCP transport, invokes `func` and `b` as bounded child processes (100 ms reap slices, `SIGTERM` cancellation), and delegates UI presentation to `darling`.

## Features

- **Implemented (`src/space`)**: Offline draft value classes `ModelUser`, `Channel`, `Message`, and `Task` covering persona metadata, channel identity, borrowed message spans, and task state transitions/delegation.
- **Planned core objects**: `Model`, `Prompt`, `Conversation`, `Answer`, `Question`, `Tool`, `ToolRegistry`, `HarnessMcp` (agent-level registration over `api-haven`'s MCP contracts), and the top-level `Harness` host.

## Build and test

From the `worktree/` root:

```bash
python3 tests/harness/run.py    # strict C23 (-Wall -Wextra -Werror) + ASan/UBSan + arity tests
./tools/b test <unit>
```

Mirrored owner tests live in `../../../tests/harness/space/`.

## Future

- Implement `Model`, `Prompt`, `Conversation`, `ToolRegistry`, and the `Harness` execution loop.
- Wire `api-haven` provider/MCP execution and bounded child-process tool supervision.
- Connect the `darling` UI host and registered R5 application seams.

## Scope and Limitations

- **Draft only**: No runnable agent binary, `HarnessServer`, mention router, live provider execution, or UI exists yet.
- **Offline tests only**: `tests/harness/run.py` proves construction, rejection, null-safe getters, and bounded projections for `src/space` values; no runtime platform is proven.
- **Borrowed text**: `Message` text is borrowed rather than durable history.
- **Layer boundaries**: Owns no connector, session, GPU, or OS implementation; never includes R5 headers.
