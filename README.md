# Personal Finance Manager | C++17

A terminal-based personal finance application that tracks income and expenses, manages monthly category budgets, and generates financial summaries. Built with modern C++ and designed to demonstrate object-oriented programming, data validation, file persistence, and automated testing.

## Features

- Add income and expense transactions with a date, category, and description
- List transactions and delete entries by ID
- View monthly or all-time income, spending, balance, and category breakdowns
- Set monthly budgets and flag categories that exceed their limits
- Automatically load saved data on startup and save data upon exit
- Store data in human-readable CSV files, with support for commas and quotes in notes
- Validate dates, positive input amounts, and monetary precision
- Track money in integer cents to avoid floating-point rounding errors

## Technologies and concepts

- **Language:** C++17
- **Build:** CMake 3.16+
- **Libraries:** C++ Standard Library (`vector`, `map`, `filesystem`, streams)
- **Testing:** Self-contained automated tests via CTest (no third-party dependency)
- **Concepts:** encapsulation, separation of concerns, CSV serialization, input validation, aggregation

## Project structure

```text
PersonalFinanceManager/
├── CMakeLists.txt
├── include/
│   └── Finance.hpp         # Public data types and Ledger interface
├── src/
│   ├── Finance.cpp         # Business logic, summaries, CSV storage
│   └── main.cpp            # Interactive terminal menu
├── tests/
│   └── test_finance.cpp    # Automated regression tests
└── data/                   # Local CSV files (ignored by Git)
```

## Getting started

You need a C++17 compiler (GCC, Clang, or MSVC) and CMake 3.16 or newer.

From the project root:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

**Run on Linux/macOS:**

```bash
./build/finance_app
```

**Run on Windows (PowerShell, common Visual Studio generator):**

```powershell
.\build\Debug\finance_app.exe
```

With a single-configuration Windows generator such as MinGW Makefiles, the executable is typically `build\finance_app.exe`.

The optional first command-line argument changes the data directory. For example:

```bash
./build/finance_app my-private-finance-data
```

## Example interaction

```text
Personal Finance Manager (amounts in CAD)
1 Add income  2 Add expense  3 View transactions  4 Delete transaction
5 Monthly report  6 Overall report  7 Set monthly budget  8 Save and exit
Choose: 2
Date (YYYY-MM-DD): 2026-10-05
Category: Food
Note: Groceries
Amount (e.g. 19.99): 42.50
Added transaction #1
```

Monthly reports compare spending against your budget for each category. For example, spending $125.00 against a $100.00 Food budget displays `OVER BUDGET`.

## Storage and privacy

The program saves `transactions.csv` and `budgets.csv` inside the selected data directory. **Do not commit personal financial records.** The default `data/*.csv` files are excluded by `.gitignore`. There are no bank connections, credentials, cloud uploads, or external APIs.

This is an educational project, not a banking or financial-advice application. Currency is displayed as CAD; it does not convert exchange rates. Back up CSV files before experimenting with manual edits.

## Possible next improvements

- Recurring payments and scheduled transactions
- CSV import/export with duplicate detection
- Search, sort, and filter transactions
- Unit test coverage for file corruption and larger datasets
- A Qt desktop interface with charts

## Resume description

**Personal Finance Management System | C++17, CMake, STL**

- Developed a console-based finance tracker with transaction management, monthly budgets, and category-based reporting.
- Implemented CSV persistence, date and money validation, and integer-cent accounting to prevent floating-point rounding issues.
- Structured the program into reusable C++ components and integrated automated regression tests through CMake/CTest.

## Author

Arber Aliu — Computer Science, Western University
