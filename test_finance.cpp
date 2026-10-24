#include "Finance.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
int failures = 0;
void check(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
template <typename F> void rejects(F f, const std::string& message) {
    try { f(); check(false, message); } catch (const std::invalid_argument&) {} 
}
}

int main() {
    using namespace pfm;
    check(validDate("2024-02-29"), "leap day allowed");
    check(!validDate("2025-02-29"), "invalid leap day rejected");
    check(!validDate("2026-13-01"), "invalid month rejected");
    check(parseMoney("12.3") == 1230, "one decimal digit");
    check(parseMoney("-0.05") == -5, "negative cents");
    check(formatMoney(-1234) == "-$12.34", "format negative amount");
    rejects([] { parseMoney("1.999"); }, "reject excess precision");
    rejects([] { parseMoney("abc"); }, "reject nonnumeric input");
    Ledger ledger;
    int incomeId = ledger.add("2026-10-01", "Salary", "Paycheck", 150000);
    ledger.add("2026-10-02", "Food", "Lunch, with a \"friend\"", -1250);
    ledger.add("2026-09-30", "Food", "September", -800);
    ledger.setBudget("Food", 1000);
    auto october = ledger.summarize("2026-10");
    check(october.income == 150000 && october.expenses == 1250 && october.balance == 148750, "monthly totals");
    check(october.expensesByCategory.at("Food") == 1250, "category total");
    check(ledger.summarize().expenses == 2050, "all time totals");
    rejects([&] { ledger.add("2026-02-30", "Test", "", 100); }, "reject bad date");
    rejects([&] { ledger.add("2026-01-01", "", "", 100); }, "reject empty category");
    const auto folder = std::filesystem::temp_directory_path() / "pfm_tests_roundtrip";
    check(ledger.save(folder.string()), "save CSV");
    Ledger restored;
    check(restored.load(folder.string()), "load CSV");
    check(restored.transactions().size() == 3, "all entries restored");
    check(restored.transactions()[1].note == "Lunch, with a \"friend\"", "CSV quotes and commas round-trip");
    check(restored.budgets().at("Food") == 1000, "budget restored");
    check(restored.remove(incomeId), "delete existing transaction");
    check(!restored.remove(incomeId), "cannot delete twice");
    int newId = restored.add("2026-10-04", "Gift", "", -100);
    check(newId > incomeId, "ID not reused after delete");
    std::filesystem::remove_all(folder);
    if (failures) { std::cerr << failures << " test(s) failed\n"; return 1; }
    std::cout << "All finance tests passed.\n";
    return 0;
}
