#include "Customer.h"
#include <sstream>
#include <vector>
#include <cctype>

int Customer::nextId = 1;

namespace {
    std::string escapeJson(const std::string& input) {
        std::string out;
        out.reserve(input.size());
        for (char c : input) {
            switch (c) {
                case '\"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\b': out += "\\b"; break;
                case '\f': out += "\\f"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default:   out += c; break;
            }
        }
        return out;
    }

    std::string unescapeJson(const std::string& input) {
        std::string out;
        out.reserve(input.size());
        for (size_t i = 0; i < input.size(); ++i) {
            char c = input[i];
            if (c == '\\' && i + 1 < input.size()) {
                char n = input[++i];
                switch (n) {
                    case '\"': out += '\"'; break;
                    case '\\': out += '\\'; break;
                    case 'b':  out += '\b'; break;
                    case 'f':  out += '\f'; break;
                    case 'n':  out += '\n'; break;
                    case 'r':  out += '\r'; break;
                    case 't':  out += '\t'; break;
                    default:   out += n; break;
                }
            } else {
                out += c;
            }
        }
        return out;
    }

    bool extractJsonString(const std::string& json, const std::string& key, std::string& out) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return false;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return false;
        pos = json.find('\"', pos);
        if (pos == std::string::npos) return false;
        ++pos; // move past opening quote
        size_t end = pos;
        bool escaped = false;
        while (end < json.size()) {
            char c = json[end];
            if (c == '\\' && !escaped) {
                escaped = true;
            } else if (c == '\"' && !escaped) {
                break;
            } else {
                escaped = false;
            }
            ++end;
        }
        if (end >= json.size()) return false;
        out = unescapeJson(json.substr(pos, end - pos));
        return true;
    }

    bool extractJsonNumber(const std::string& json, const std::string& key, long long& out) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return false;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return false;
        ++pos;
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) ++pos;
        size_t end = pos;
        while (end < json.size() &&
               (std::isdigit(static_cast<unsigned char>(json[end])) ||
                json[end] == '-' || json[end] == '+' ||
                json[end] == '.' || json[end] == 'e' || json[end] == 'E')) {
            ++end;
        }
        if (end == pos) return false;
        try {
            out = std::stoll(json.substr(pos, end - pos));
        } catch (...) {
            return false;
        }
        return true;
    }
}

Customer::Customer() : id(0), name(""), email(""), phone("") {}

Customer::Customer(const std::string& name, const std::string& email, const std::string& phone)
    : id(nextId++), name(name), email(email), phone(phone) {}

Customer::Customer(int id, const std::string& name, const std::string& email, const std::string& phone)
    : id(id), name(name), email(email), phone(phone) {
    if (id >= nextId) nextId = id + 1;
}

void Customer::display() const {
    std::cout << "Customer ID: " << id << "\n"
              << "Name: " << name << "\n"
              << "Email: " << email << "\n"
              << "Phone: " << phone << "\n";
}

std::string Customer::serialize() const {
    std::ostringstream ss;
    ss << "{";
    ss << "\"id\":" << id << ",";
    ss << "\"name\":\"" << escapeJson(name) << "\",";
    ss << "\"email\":\"" << escapeJson(email) << "\",";
    ss << "\"phone\":\"" << escapeJson(phone) << "\"";
    ss << "}";
    return ss.str();
}

Customer Customer::deserialize(const std::string& data) {
    long long idVal = 0;
    std::string nameVal, emailVal, phoneVal;

    if (!extractJsonNumber(data, "id", idVal)) {
        return Customer();
    }
    extractJsonString(data, "name", nameVal);
    extractJsonString(data, "email", emailVal);
    extractJsonString(data, "phone", phoneVal);

    return Customer(static_cast<int>(idVal), nameVal, emailVal, phoneVal);
}
