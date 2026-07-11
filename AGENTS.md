# Project Florence Desktop — Agent Instructions

## Build

- **CMake + Qt 6.10+** (`qt_standard_project_setup(REQUIRES 6.10)`)
- Build directory already exists at `build/Desktop_Qt_6_11_1_MinGW_64_bit_Debug/`
- Existing CMake build tree uses **MinGW 64-bit**; do not mix with MSVC or other toolchains
- Entrypoint: `main.cpp` → loads QML module `project_florence_desktop` from `Main.qml`

## Project structure

| Path | Role |
|---|---|
| `main.cpp` | C++ entrypoint |
| `Main.qml` | Main QML UI (dark/light mode toggle, two-panel grid) |
| `importedcontent/` | Figma-to-Qt design files; auto-included via `add_subdirectory` if `CMakeLists.txt` exists |
| `talimatlar/` | Backend API docs (FastAPI at `http://localhost:7055`), OpenAPI spec |

## Backend API

- Base URL: `http://localhost:7055`
- Endpoints documented in `talimatlar/API.md` and `talimatlar/openapi.json`
- Covers: BIST companies, price history (yfinance), news (GDELT), Monte Carlo simulations, macro economy, IPO, Scout stock picker, auth (mock), favorites (mock)

## State

- **No commits yet** (empty git history on `master`)
- **No tests, no CI, no lint/formatter/typecheck config** — skipping these steps is expected
- **No `package.json`** — this is not an npm project

## OpenCode config

- `opencode.json.txt` (note: `.txt` suffix, not `.json`) — enables `lsp: true`
