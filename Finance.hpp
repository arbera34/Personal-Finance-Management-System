#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace pfm {
using Cents = std::int64_t;

struct Transaction {
    int id;
    std::string date;       // YYYY-MM-DD
    std::string category;
    std::string note;
    Cents amount;           // positive = income, negative = expense
};

struct Summary {
    Cents income = 0;
    Cents expenses = 0;
    Cents balance = 0;
    std::map<std::string, Cents> expensesByCategory;
};

bool validDate(const std::string& date);
Cents parseMoney(const std::string& input);
std::string formatMoney(Cents amount);

class Ledger {
public:
    int add(const std::string& date, std::string category, std::string note, Cents amount);
    bool remove(int id);
    const std::vector<Transaction>& transactions() const { return entries_; }
    Summary summarize(const std::string& month = "") const;
    void setBudget(std::string category, Cents monthlyLimit);
    const std::map<std::string, Cents>& budgets() const { return budgets_; }
    bool save(const std::string& directory) const;
    bool load(const std::string& directory);

private:
    std::vector<Transaction> entries_;
    std::map<std::string, Cents> budgets_;
    int nextId_ = 1;
};
} // namespace pfm
