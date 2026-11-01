# Integration notes

Branch: `feature/integration` (started from `Builder+COR1`, then `main` merged in).
Each pattern was merged one at a time and the whole program was built after every merge.
This file records **every change made while integrating**, so each owner knows what
changed in their code and why.

Build: `make` at the repo root builds every `.cpp` except unit tests (`*/tests/*`)
and CMake build folders. Each pattern's unit tests are built with its own CMakeLists.

---

## Merge order

1. Builder + CHOR1 (base)
2. State
3. Adapter
4. Command
5. Observer
6. Factory
7. *(still to do)* Proxy, CHOR2, Façade

---

## Changes per owner

### CHOR1: WorkflowNode / WorkflowInstance (Dirk, Corné)

**WorkflowNode** (needed for State to compile)
- Removed the empty `class NodeState` in `WorkflowNode.h`; it now includes the State pattern's `NodeState.h`.
- `setState()` is **public**, because the concrete states call `node.setState(...)`.
- The constructor starts every node in `WaitingState`.
- The event functions (`start`, `pass`, `reject`, …) now forward to the state: `state->reset(*this)`.
- `getState()` / `getStateName()` return the real state (they returned `nullptr`, which crashes as a `std::string`).
- `needsApproval()` called `approve()`; it now calls `needsApproval()`.
- `retry()` removed (dropped from the design).
- `resetChain()` no longer crashes on the last node (`next` is null there).

**WorkflowInstance**
- Added `getId()`; the commands and the observers need it.
- `status` and `current` are initialised in the constructor (they held garbage before).
- `WorkflowStatus` is now an `enum class`, defined **only here**. Observer had a second copy, which was removed.
- `WorkflowInstance` **is the Subject** of the Observer pattern (`: public WorkflowSubject`).
  `complete()`, `reject(node)` and `cancel()` now notify the observers.
- `reject(node)` records the node, and `getRejectedAt()` returns it (it returned the workflow ID before).
- The history stores the status as text ("Rejected"). Assigning the enum to a string silently stored one random character.

**Nodes**
- **Added `ExternalCheckNode.cpp`** (the header had no implementation). It runs an adapter through
  the `ExternalCheck` interface: passed → PASSED, check said no → REJECTED, adapter threw or no
  document → FAILED.
- `RangeCheckNode` returns **REJECTED** when a rule isn't met. FAILED is for technical errors;
  a rule not being met is a business "no".

### State (Mandla)
- `PendingState::cancel` was written as `WaitingState::cancel`; renamed, and `CancelledState.h` included.
- Added `PendingState::reset()` → Waiting (agreed design: a node waiting for a decision can be reset), and included `WaitingState.h`.

### Adapter (Hafani)
- The test-only mock `Document.h` moved from `Adapter/inc` to `Adapter/tests/support`, so the main
  build uses the real `Document`. The adapters work unchanged with the team's `Document`
  (`getField` / `getNumber`).
- `Adapter/CMakeLists.txt` points at `inc` (it still said `src` after the header split).

### Command (Hafani)
- `Command/CMakeLists.txt` finds the team's code in the integration layout
  (`WorkflowInstance/inc` + `src`, any State file names) and also builds `WorkflowSubject`.
- `AppealCommand` removed (appeal dropped from the design).

### Observer (Takunda)
- Includes changed to file names only (`#include "WorkflowInstance.h"`), since
  `../../WorkflowInstance/WorkflowInstance.h` doesn't exist in this layout.
- `AccountCreationObserver`: `registry->add(factory->create(wi).release())`. **Ownership:** the
  factory returns a `unique_ptr<Account>` (the caller owns the new account); the registry stores
  `unique_ptr<Account>` but its `add()` takes a raw `Account*` and wraps it. `.release()` hands
  ownership from the factory's pointer to the registry, so the account is owned exactly once
  and never leaks.
  *Suggested clean-up:* make `AccountRegistry::add` take `std::unique_ptr<Account>`, so the
  ownership transfer shows in the type and `.release()` isn't needed.

### Factory (Motlatso)
- Includes changed to file names only (`"WorkflowInstance.h"`, `"Document.h"`).
- Removed the stray `SequenceDiagrams/GeneralAccountFactory.h`. Because the Makefile searches
  every header folder, the compiler picked up that broken copy instead of `inc/GeneralAccountFactory.h`.

### Repository
- Root `Makefile`: skips `tests/` and `build/` folders, adds every header folder to the include
  path, and rebuilds when a header changes (`-MMD -MP`).
- One root `.gitignore`: `build/`, `*.o`, `*.d`, `tividy`, `.idea/`, `.vscode/`, `.DS_Store`.
- Build output that had been committed (`Command/build/`, `.o`, `.d`, `tividy`) was untracked.

---

## Rules for everyone (to avoid the problems above)
- **Include by file name only**: `#include "Document.h"`, never `../../Folder/Document.h`.
  The Makefile finds every header folder.
- **No headers outside your pattern's folder** (e.g. in `SequenceDiagrams/`). A copy there can shadow the real one.
- **One definition per type.** Before adding an enum or struct, check that it doesn't already exist.
- **Never commit build output** (`build/`, `.o`, `tividy`) or generated Doxygen HTML.

---

## Still open
- `DocCompletionCheckNode` returns FAILED for an empty field. Should an incomplete form be REJECTED? (CHOR1 owners to decide)
- `handle()` doesn't move along the chain or change node states yet; `main.cpp` steps through the nodes by hand.
- Proxy branch commits generated `docs/html/`; remove it before merging.
- Factory and Proxy both use root `inc/` and `src/`; consider moving them into their own folders.
- Still to merge: Proxy, CHOR2, Façade. Then the demo `main.cpp` (admission and loan scenarios).

---

## Update: Proxy merged (Markus)
- Merged cleanly; `AccountProxy`, `RealAccount`, `AccessLog` build with Factory's `Account` classes (shared root `inc/` and `src/`).
- Removed the committed Doxygen output (`docs/html/`) from Git and added `docs/html/` and `docs/latex/` to `.gitignore`; it is regenerated by running `doxygen`. The test-evidence files in `docs/` were kept.
- Still to merge: CHOR2, Façade.
