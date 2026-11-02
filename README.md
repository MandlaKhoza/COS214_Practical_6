# TIVIDY: Workflow Management System

COS 214 Final Project (2026) · C++11 · University of Pretoria

TIVIDY is a **generic workflow engine**. A workflow _definition_ describes the steps
of a process; every case that enters the system becomes an independent workflow
_instance_ with its own data, state and history. The engine itself knows nothing
about any industry. Each industry only supplies configuration: form fields,
business rules, external-system adapters and approvers.

Two industries run on the same engine:

| Industry      | Workflow                                      | Program                       |
| ------------- | --------------------------------------------- | ----------------------------- |
| **Education** | University application (BSc Computer Science) | `tividy` (`main.cpp`)         |
| **Finance**   | Personal loan application                     | `loan_demo` (`loan_demo.cpp`) |

Repository: <https://github.com/MandlaKhoza/COS214_Practical_6>

---

## Contents

1. [Dependencies](#1-dependencies)
2. [Build and run](#2-build-and-run)
3. [The two industries](#3-the-two-industries)
4. [Unit tests](#4-unit-tests)
5. [Design patterns](#5-design-patterns)
6. [Repository layout](#6-repository-layout)
7. [Documentation (Doxygen)](#7-documentation-doxygen)
8. [Development process](#8-development-process)
9. [Team](#9-team)

---

## 1. Dependencies

| Tool                              | Version                               | Needed for                              |
| --------------------------------- | ------------------------------------- | --------------------------------------- |
| C++ compiler (`g++` or `clang++`) | C++11 support                         | everything                              |
| `make`                            | any                                   | building the programs                   |
| CMake                             | ≥ 3.14                                | unit tests                              |
| Git                               | any                                   | unit tests (CMake downloads GoogleTest) |
| GoogleTest                        | **1.12.1** (downloaded automatically) | unit tests                              |
| Doxygen                           | ≥ 1.9 (optional)                      | generating the documentation            |

GoogleTest 1.12.1 is pinned on purpose: it is the last release that supports C++11
(1.13 and later require C++14). No manual install is needed, because each test
`CMakeLists.txt` fetches it.

**macOS:** `xcode-select --install` (compiler and make), `brew install cmake doxygen`
**Ubuntu:** `sudo apt install build-essential cmake git doxygen`

## 2. Build and run

From the repository root:

```bash
make            # builds both programs: tividy and loan_demo
make run        # integration checks + university application scenario
make run-loan   # personal loan scenario
make clean      # removes all build output
```

or run the programs directly with `./tividy` and `./loan_demo`.

The root `Makefile` compiles every engine `.cpp` in the project (skipping `tests/`
and `build/` folders) and adds every header folder to the include path, so
sources include headers by file name only (`#include "Document.h"`).

## 3. The two industries

Both scenarios are built from the **same engine classes**, unchanged:
`WorkflowDefinition`, `WorkflowInstance`, `WorkflowNode` and its node types, the
node states, `ApprovalChain`, the commands and `WorkflowController`, the observers
and `Document`. Only the configuration below differs.

### 3.1 Education: university application (`./tividy`)

`main.cpp` first runs 56 integration checks across the engine (chain execution,
rejection, validation, pending approval, cancellation, reset, failure handling,
observers, history, definitions and cloning), then runs the admission scenario.

```
ApplicationCompletion → MinimumAPS → VerifiedQualifications → FacultyApproval
 all fields filled?      APS ≥ 30     exam board: Maths ≥ 60,   faculty: APS ≥ 35
                                      English ≥ 50 (adapter)
```

- **Participants:** applicant, admissions office (rule checks), exam board
  (external system via `QualificationAdapter`), Science Faculty admissions committee.
- **Input:** name, surname, email, programme, exam candidate number, ID copy,
  certified results, APS.
- **Outcome:** approved, or rejected at the step that failed, with the reason in the history.

### 3.2 Finance: personal loan application (`./loan_demo`)

```
ApplicationCompletion → AffordabilityRules → IdentityCheck → CreditCheck → FraudScreening → LoanApproval
                        income ≥ R8 000       Home Affairs    score ≥ 620    risk ≤ 0.5        Officer ≤ R100k →
                        term ≤ 72 months      (adapter)       (adapter)      (adapter)         Manager ≤ R500k →
                        principal ≤ R1m                                                        Credit Committee
```

The demo runs four acts:

1. **Rejection and recovery:** Thabo (R350 000) is rejected at the credit check,
   his credit record is updated, staff reset the workflow (command), it runs
   again, escalates to the Branch Manager and is **approved**. He is notified,
   and a loan account is opened automatically.
2. **Cancellation:** Lerato withdraws. The cancel is logged, and a later reset or
   run of the cancelled workflow is **refused**.
3. **Escalation:** Sipho (R900 000) escalates Officer → Manager → Credit
   Committee and is **rejected** by the committee.
4. **Isolation and access control:** the other instances are unaffected by
   Thabo's reset; his loan account is viewed through the access-control proxy by
   the owner (full), another client (denied), staff (full and notes) and an
   auditor (summary), followed by the access log and the command audit log.

External systems (Home Affairs, credit bureau, fraud engine, exam board) are
**local test stubs** behind the Adapter pattern; no real service is contacted.

## 4. Unit tests

Unit tests use **GoogleTest** and are built per pattern with CMake. In each
pattern folder:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

| Folder     | Test programs                                                                    | What is tested                                                                                                          |
| ---------- | -------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| `Adapter/` | `test_adapter`                                                                   | the five external-system stubs and five adapters: pass, reject, missing field, unknown person, boundaries, polymorphism |
| `Command/` | `test_command`, `test_reset_node`, `test_cancel_workflow`, `test_reset_workflow` | invoker and audit log, refusals, reset and cancel against the real node states, all-or-nothing reset, null receivers    |
| `Factory/` | `proxy_tests`                                                                    | access log, access levels per role, proxy behaviour                                                                     |

<!-- TODO (owners): add the rows below once their CMakeLists build these tests
| `Factory/` | factory tests | FactoryTests.cpp, FinanceFactoryTests.cpp |
| `CoR2/` | approval tests | ApproverTest, ThresholdApproverTest, ApprovalChainTest, ApprovalNodeTest |
| `observer/` | `test_observer` | attach, detach, notify order |
-->

`Command/` builds its concrete-command tests only when the `WorkflowInstance/`
and `State-pattern/` folders are present, and otherwise builds just `test_command`.
Test evidence (failing, then passing) is kept in `*/test_evidence/` and `docs/`.

## 5. Design patterns

| Category    | Pattern                     | Problem it solves in TIVIDY                                                                                           | Main classes (folder)                                                                                                                  |
| ----------- | --------------------------- | --------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------- |
| Creational  | **Builder**                 | assembling a case `Document` from a form definition                                                                   | `Director`, `Builder`, `DocumentBuilder` (`DocumentBuilder/`)                                                                          |
| Creational  | **Factory Method**          | opening the right kind of account when a workflow is approved                                                         | `AccountFactory`, `GeneralAccountFactory`, `FinanceAccountFactory` (`Factory/`)                                                        |
| Creational  | **Prototype**               | every instance gets its own copy of the definition's step chain (instance isolation)                                  | `WorkflowNode::clone/cloneChain`, `WorkflowDefinition::createChain` (`WorkflowInstance/`)                                              |
| Structural  | **Adapter**                 | external systems with incompatible interfaces used through one `ExternalCheck` interface                              | `CreditCheckAdapter`, `IdentityVerificationAdapter`, `QualificationAdapter`, `FraudCheckAdapter`, `CriminalRecordAdapter` (`Adapter/`) |
| Structural  | **Proxy**                   | role-based access to account details, with every access logged                                                        | `AccountProxy`, `RealAccount`, `AccessLog` (`Factory/`)                                                                                |
| Structural  | **Facade**                  | one entry point for the user interface                                                                                | `TividyFacade`                                                                                                                         |
| Behavioural | **Chain of Responsibility** | (1) a case moves step by step through the workflow; (2) approvals escalate up the authority hierarchy                 | `WorkflowNode` and node types (`WorkflowInstance/`); `Approver`, `ThresholdApprover`, `RuleBasedApprover`, `ApprovalChain` (`CoR2/`)   |
| Behavioural | **Template Method**         | every step runs the same skeleton (`handle()`), and only `process()` differs                                          | `WorkflowNode::handle` / `process`                                                                                                     |
| Behavioural | **State**                   | what a step may do depends on where it is (Waiting, InProgress, Pending, Passed, Rejected, Failed, Cancelled)         | `NodeState` + 7 states (`State-pattern/`)                                                                                              |
| Behavioural | **Command**                 | user actions (cancel, reset) as objects that can be refused and audited; reset of a whole workflow as a macro command | `Command`, `WorkflowController`, `CancelWorkflowCommand`, `ResetNodeCommand`, `ResetWorkflowCommand` (`Command/`)                      |
| Behavioural | **Observer**                | reacting when a workflow finishes (notify the applicant, open an account) without the engine knowing who listens      | `WorkflowSubject`, `NotificationObserver`, `AccountCreationObserver` (`observer/`)                                                     |

`WorkflowInstance` is the Observer's subject; `ExternalCheckNode` connects the
Adapter pattern to the workflow chain; `ApprovalNode` connects the approval chain
to it.

## 6. Repository layout

````
Adapter/            Adapter pattern: external-system stubs, adapters, tests
Command/            Command pattern: commands, WorkflowController, tests
CoR2/               approval chain (Chain of Responsibility 2), tests
DocumentBuilder/    Builder pattern and Document
Factory/            Factory Method (accounts) and Proxy (account access), tests
State-pattern/      node states (State pattern)
WorkflowInstance/   engine core: WorkflowDefinition, WorkflowInstance, WorkflowNode and node types
observer/           Observer pattern
Facade/             combines all teh subsystems
Builder patter/     Uses the information give by the user to build a document that must be traversed by teh user
main.cpp            integration checks + university scenario
other Diagrams PNG/ stored images of all teh diagrams
other Diagrams VPP/ stores vpp files of all the diagrams
Docs/               Test results for teh design patterns
TUI/                All teh fiels related to teh TUI
CMakeList.txt/      builds tividy and loan_demo
Doxyfile/           Doxygen configuration
makefile/           used for running teh code on WSL
INTEGRATION_NOTES.md  every change made while integrating the patterns, and why
Each pattern folder uses `inc/` (or `include/`) for headers and `src/` for sources.

## 7. Documentation (Doxygen)

```bash
doxygen Doxyfile
open docs/html/index.html      # Linux: xdg-open docs/html/index.html
````

The generated `docs/html/` is not committed (see `.gitignore`); run the command above to rebuild it.

## 8. Development process

- **Branches:** each member works on `feature/<pattern>_<name>`, branched from `main`.
- **Pull requests:** every change reaches `main` through a pull request that needs
  **three approving reviews**.
- **Integration:** patterns were combined one at a time on `feature/integration`,
  building the whole program after each merge. Every fix made to a pattern during
  integration is recorded in `INTEGRATION_NOTES.md`. The integration branch is
  merged into `main` with a merge commit, so every member's history is preserved.
- **Never committed:** build output (`build/`, `*.o`, `tividy`), IDE folders and generated documentation.

## 9. Team

| Member                        | Main responsibilities                        |
| ----------------------------- | -------------------------------------------- |
| Hafani Ramagoma u23605091     | Adapter, Command, integration, loan scenario |
| Amelton Khoza u19201062       | State, Chain of Responsibility (approvals)   |
| Dirk Kunz u24689999           | Chain of Responsibility (workflow), TUI      |
| Corné Myburgh u25505204       | Chain of Responsibility (workflow)           |
| Markus Zeelie u25098544       | Proxy, Chain of Responsibility (approvals)   |
| Takunda Mugwagwa u24667341    | Observer                                     |
| Motlatso Makgamatho u25605021 | Factory Method                               |
