# Personal Finance Management System | C++

## Project Overview

The Personal Finance Management System is a console-based application developed in C++17 that helps users manage their personal finances by tracking income, expenses, and monthly budgets. The program provides an organized way to record financial transactions, monitor spending habits, and generate financial summaries.

This project demonstrates practical applications of object-oriented programming, data structures, file handling, and algorithmic problem-solving in C++.

## Key Features

- **Income and Expense Tracking:** Add financial transactions with dates, categories, amounts, and descriptions.
- **Transaction Management:** View recorded transactions and delete entries when needed.
- **Monthly Budgeting:** Create spending limits for different categories, such as groceries, transportation, and entertainment.
- **Financial Reports:** Generate monthly or overall summaries showing total income, expenses, and remaining balance.
- **Spending Analysis:** View expenses organized by category to better understand spending habits.
- **Data Persistence:** Automatically load saved records when the application starts and save financial information to CSV files when exiting.
- **Input Validation:** Prevent invalid dates, incorrect financial amounts, and incomplete transaction information.

## Technologies Used

- **Programming Language:** C++17
- **Build System:** CMake
- **Libraries:** C++ Standard Library (STL)
- **Data Storage:** CSV files
- **Testing:** Automated C++ tests using CTest
- **Programming Concepts:** Object-Oriented Programming, Encapsulation, Data Structures, File I/O, and Input Validation

## How the Program Works

When the application starts, users interact with a menu-driven interface that provides several financial management options.

Users can:

1. Add an income transaction.
2. Add an expense transaction.
3. View transaction history.
4. Delete existing transactions.
5. Generate monthly financial reports.
6. View overall financial summaries.
7. Set monthly spending budgets.
8. Save their information and exit.

The program processes transactions, calculates financial totals, and stores information locally for future use.

## Project Structure

- `main.cpp` – Handles the main application interface, user input, and menu navigation.
- `Finance.cpp` – Implements financial calculations, transaction management, budgeting, and file storage.
- `Finance.hpp` – Defines financial data structures and the Ledger class.
- `test_finance.cpp` – Contains automated tests for financial calculations and application functionality.
- `CMakeLists.txt` – Configures the C++ build process.

## Technical Implementation

The application uses a modular design that separates user interaction from financial processing.

Financial records are managed through a Ledger class, which handles adding and deleting transactions, calculating financial summaries, and maintaining category budgets.

The program uses standard C++ containers, including vectors and maps, to organize transaction records and spending categories.

To improve accuracy, financial amounts are represented using integer cents instead of floating-point values, reducing the risk of rounding errors during calculations.

CSV file handling allows transaction and budget information to remain available between application sessions.

## How to Run the Project

### Requirements

- C++17-compatible compiler
- CMake 3.16 or newer
- Visual Studio Code or another C++ development environment

### Build Instructions

Open a terminal in the project directory and run:

```bash
cmake -S . -B build
cmake --build build
```

On Linux or macOS, run:

```bash
./build/finance_app
```

On Windows, using a Visual Studio CMake generator, run:

```powershell
.\build\Debug\finance_app.exe
```

### Running Tests

```bash
ctest --test-dir build --output-on-failure
```

## Skills Demonstrated

Through this project, I applied several important software development concepts:

- Object-oriented programming and encapsulation
- C++ class design and modular programming
- Data structures using vectors and maps
- File handling and CSV data persistence
- Error handling and input validation
- Financial calculations and data aggregation
- Automated software testing
- Software organization using CMake

## Future Improvements

Potential enhancements include:

- Developing a graphical user interface using Qt
- Adding financial charts and spending visualizations
- Implementing recurring transactions
- Providing advanced transaction filtering and searching
- Supporting multiple user profiles

## Author

**Arber Aliu**  
Computer Science Student  
Western University

*Developed to demonstrate C++ programming, object-oriented software design, and practical problem-solving skills.*
