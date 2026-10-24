#include "Finance.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>

namespace {
std::string ask(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Input closed");
    return line;
}
int askInt(const std::string& prompt) {
    const auto raw = ask(prompt);
    std::size_t used = 0;
    int number = std::stoi(raw, &used);
    if (used != raw.size()) throw std::invalid_argument("Enter a whole number");
    return number;
}
void list(const pfm::Ledger& ledger) {
    if (ledger.transactions().empty()) { std::cout << "No transactions yet.\n"; return; }
    for (const auto& t : ledger.transactions()) {
        std::cout << '#' << t.id << " | " << t.date << " | " << t.category
                  << " | " << pfm::formatMoney(t.amount) << " | " << t.note << '\n';
    }
}
void report(const pfm::Ledger& ledger, const std::string& month) {
    const auto s = ledger.summarize(month);
    std::cout << "\nReport: " << (month.empty() ? "all time" : month) << '\n';
    std::cout << "Income:   " << pfm::formatMoney(s.income) << '\n';
    std::cout << "Expenses: " << pfm::formatMoney(s.expenses) << '\n';
    std::cout << "Balance:  " << pfm::formatMoney(s.balance) << '\n';
    std::cout << "Expense categories:\n";
    for (const auto& [category, spent] : s.expensesByCategory)
        std::cout << "  " << category << ": " << pfm::formatMoney(spent) << '\n';
    if (!month.empty()) {
        std::cout << "Monthly budgets:\n";
        for (const auto& [category, limit] : ledger.budgets()) {
            const auto it = s.expensesByCategory.find(category);
            const auto spent = it == s.expensesByCategory.end() ? 0 : it->second;
            std::cout << "  " << category << ": " << pfm::formatMoney(spent) << " / "
                      << pfm::formatMoney(limit) << (spent > limit ? " (OVER BUDGET)" : " (on track)") << '\n';
        }
    }
}
} // namespace

int main(int argc, char** argv) {
    const std::string directory = argc > 1 ? argv[1] : "data";
    pfm::Ledger ledger;
    if (!ledger.load(directory)) {
        std::cerr << "Could not read data files in '" << directory << "'. Check CSV formatting and backups.\n";
        return 1;
    }
    std::cout << "Personal Finance Manager (amounts in CAD)\nData folder: " << directory << '\n';
    for (;;) {
        std::cout << "\n1 Add income  2 Add expense  3 View transactions  4 Delete transaction\n"
                  << "5 Monthly report  6 Overall report  7 Set monthly budget  8 Save and exit\n";
        try {
            const int choice = askInt("Choose: ");
            if (choice == 8) {
                if (!ledger.save(directory)) { std::cerr << "Save failed.\n"; return 1; }
                std::cout << "Saved. Goodbye!\n";
                return 0;
            }
            switch (choice) {
                case 1:
                case 2: {
                    const auto date = ask("Date (YYYY-MM-DD): ");
                    const auto category = ask("Category: ");
                    const auto note = ask("Note: ");
                    const auto amount = pfm::parseMoney(ask("Amount (e.g. 19.99): "));
                    if (amount <= 0) throw std::invalid_argument("Enter an amount greater than zero");
                    int id = ledger.add(date, category, note, choice == 1 ? amount : -amount);
                    std::cout << "Added transaction #" << id << '\n';
                    break;
                }
                case 3: list(ledger); break;
                case 4: {
                    int id = askInt("Transaction ID to delete: ");
                    std::cout << (ledger.remove(id) ? "Deleted.\n" : "ID not found.\n");
                    break;
                }
                case 5: report(ledger, ask("Month (YYYY-MM): ")); break;
                case 6: report(ledger, ""); break;
                case 7: {
                    auto category = ask("Budget category: ");
                    auto amount = pfm::parseMoney(ask("Monthly limit: "));
                    ledger.setBudget(category, amount);
                    std::cout << "Budget updated.\n";
                    break;
                }
                default: std::cout << "Choose an option from 1 to 8.\n";
            }
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Input closed") {
                if (!ledger.save(directory)) { std::cerr << "Save failed.\n"; return 1; }
                return 0;
            }
            std::cout << "Error: " << e.what() << '\n';
        } catch (const std::exception& e) { std::cout << "Error: " << e.what() << '\n'; }
    }
}
