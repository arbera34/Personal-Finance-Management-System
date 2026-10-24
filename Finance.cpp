#include "Finance.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace pfm {
namespace {
std::string trim(std::string s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    return s.substr(first, s.find_last_not_of(" \t\r\n") - first + 1);
}
std::string csvEscape(const std::string& s) {
    std::string out = "\"";
    for (char c : s) { if (c == '"') out += '"'; out += c; }
    return out + '"';
}
bool parseCsv(const std::string& line, std::vector<std::string>& fields) {
    fields.clear();
    std::string current;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') { current += '"'; ++i; }
            else quoted = !quoted;
        } else if (c == ',' && !quoted) { fields.push_back(current); current.clear(); }
        else current += c;
    }
    if (quoted) return false;
    fields.push_back(current);
    return true;
}
bool validMonth(const std::string& month) {
    return month.size() == 7 && validDate(month + "-01");
}
} // namespace

bool validDate(const std::string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
    for (std::size_t i = 0; i < s.size(); ++i)
        if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    int y = std::stoi(s.substr(0, 4)), m = std::stoi(s.substr(5, 2)), d = std::stoi(s.substr(8, 2));
    if (y < 1900 || y > 9999 || m < 1 || m > 12 || d < 1) return false;
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0))) days[1] = 29;
    return d <= days[m - 1];
}

Cents parseMoney(const std::string& input) {
    const std::string s = trim(input);
    if (s.empty()) throw std::invalid_argument("Amount is empty");
    std::size_t i = 0;
    bool neg = false;
    if (s[i] == '-' || s[i] == '+') { neg = s[i] == '-'; ++i; }
    if (i == s.size()) throw std::invalid_argument("Invalid amount");
    Cents whole = 0, fraction = 0;
    int digits = 0;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
        int digit = s[i++] - '0';
        if (whole > (std::numeric_limits<Cents>::max() / 100 - 10) / 10)
            throw std::out_of_range("Amount too large");
        whole = whole * 10 + digit;
        ++digits;
    }
    if (!digits) throw std::invalid_argument("Invalid amount");
    if (i < s.size() && s[i] == '.') {
        ++i;
        int places = 0;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])) && places < 2) {
            fraction = fraction * 10 + (s[i++] - '0');
            ++places;
        }
        if (places == 1) fraction *= 10;
        if (places == 0) throw std::invalid_argument("Missing cents");
    }
    if (i != s.size()) throw std::invalid_argument("Use at most two decimal places");
    if (whole > (std::numeric_limits<Cents>::max() - fraction) / 100)
        throw std::out_of_range("Amount too large");
    Cents value = whole * 100 + fraction;
    return neg ? -value : value;
}

std::string formatMoney(Cents amount) {
    const bool neg = amount < 0;
    const std::uint64_t absValue = neg ? static_cast<std::uint64_t>(-(amount + 1)) + 1 : static_cast<std::uint64_t>(amount);
    std::ostringstream out;
    if (neg) out << '-';
    out << '$' << (absValue / 100) << '.' << std::setw(2) << std::setfill('0') << (absValue % 100);
    return out.str();
}

int Ledger::add(const std::string& date, std::string category, std::string note, Cents amount) {
    category = trim(category);
    if (!validDate(date)) throw std::invalid_argument("Date must be a real YYYY-MM-DD date");
    if (category.empty()) throw std::invalid_argument("Category is required");
    if (amount == 0) throw std::invalid_argument("Amount cannot be zero");
    if (nextId_ == std::numeric_limits<int>::max()) throw std::overflow_error("Too many transactions");
    const int id = nextId_++;
    entries_.push_back({id, date, std::move(category), std::move(note), amount});
    return id;
}

bool Ledger::remove(int id) {
    auto it = std::find_if(entries_.begin(), entries_.end(), [id](const Transaction& t) { return t.id == id; });
    if (it == entries_.end()) return false;
    entries_.erase(it);
    return true;
}

Summary Ledger::summarize(const std::string& month) const {
    if (!month.empty() && !validMonth(month)) throw std::invalid_argument("Month must be YYYY-MM");
    Summary s;
    for (const auto& t : entries_) {
        if (!month.empty() && t.date.substr(0, 7) != month) continue;
        if (t.amount > 0) s.income += t.amount;
        else { s.expenses += -t.amount; s.expensesByCategory[t.category] += -t.amount; }
    }
    s.balance = s.income - s.expenses;
    return s;
}

void Ledger::setBudget(std::string category, Cents limit) {
    category = trim(category);
    if (category.empty() || limit < 0) throw std::invalid_argument("Invalid category or budget");
    budgets_[category] = limit;
}

bool Ledger::save(const std::string& directory) const {
    try {
        std::filesystem::create_directories(directory);
        std::ofstream tx(std::filesystem::path(directory) / "transactions.csv", std::ios::trunc);
        std::ofstream bg(std::filesystem::path(directory) / "budgets.csv", std::ios::trunc);
        if (!tx || !bg) return false;
        tx << "id,date,category,note,amount_cents\n";
        for (const auto& t : entries_)
            tx << t.id << ',' << csvEscape(t.date) << ',' << csvEscape(t.category) << ','
               << csvEscape(t.note) << ',' << t.amount << '\n';
        bg << "category,limit_cents\n";
        for (const auto& [category, limit] : budgets_)
            bg << csvEscape(category) << ',' << limit << '\n';
        tx.flush(); bg.flush();
        return tx.good() && bg.good();
    } catch (const std::filesystem::filesystem_error&) { return false; }
}

bool Ledger::load(const std::string& directory) {
    const auto txPath = std::filesystem::path(directory) / "transactions.csv";
    const auto bgPath = std::filesystem::path(directory) / "budgets.csv";
    if (!std::filesystem::exists(txPath) && !std::filesystem::exists(bgPath)) return true;
    if (!std::filesystem::exists(txPath) || !std::filesystem::exists(bgPath)) return false;
    std::ifstream tx(txPath), bg(bgPath);
    if (!tx || !bg) return false;
    Ledger loaded;
    std::string line;
    std::vector<std::string> fields;
    if (!std::getline(tx, line) || line != "id,date,category,note,amount_cents") return false;
    try {
        while (std::getline(tx, line)) {
            if (!parseCsv(line, fields) || fields.size() != 5) return false;
            std::size_t consumed = 0;
            int id = std::stoi(fields[0], &consumed);
            if (consumed != fields[0].size() || id < 1 || !validDate(fields[1]) || trim(fields[2]).empty()) return false;
            Cents amount = std::stoll(fields[4], &consumed);
            if (consumed != fields[4].size() || amount == 0 || id == std::numeric_limits<int>::max()) return false;
            if (std::any_of(loaded.entries_.begin(), loaded.entries_.end(), [id](const Transaction& t){ return t.id == id; })) return false;
            loaded.entries_.push_back({id, fields[1], fields[2], fields[3], amount});
            loaded.nextId_ = std::max(loaded.nextId_, id + 1);
        }
        if (!std::getline(bg, line) || line != "category,limit_cents") return false;
        while (std::getline(bg, line)) {
            if (!parseCsv(line, fields) || fields.size() != 2 || trim(fields[0]).empty()) return false;
            std::size_t consumed = 0;
            Cents amount = std::stoll(fields[1], &consumed);
            if (consumed != fields[1].size() || amount < 0) return false;
            loaded.budgets_[fields[0]] = amount;
        }
    } catch (const std::exception&) { return false; }
    if (tx.bad() || bg.bad()) return false;
    *this = std::move(loaded);
    return true;
}
} // namespace pfm
