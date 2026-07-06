#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using ip_t = std::tuple<int, int, int, int>;

namespace {

std::string first_field(const std::string &line)
{
    const auto tab_pos = line.find('\t');
    return tab_pos == std::string::npos ? line : line.substr(0, tab_pos);
}

std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> parts;
    std::string::size_type start = 0;
    while (true) {
        const auto pos = str.find(delim, start);
        if (pos == std::string::npos) {
            parts.push_back(str.substr(start));
            break;
        }
        parts.push_back(str.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

int parse_octet(const std::string &s)
{
    if (s.empty()) {
        throw std::invalid_argument("empty octet");
    }
    std::size_t idx = 0;
    const int value = std::stoi(s, &idx);
    if (idx != s.size()) {
        throw std::invalid_argument("invalid octet: " + s);
    }
    if (value < 0 || value > 255) {
        throw std::out_of_range("octet out of range: " + s);
    }
    return value;
}

} // namespace

ip_t parse_ip(const std::string &text)
{
    const auto parts = split(text, '.');
    if (parts.size() != 4) {
        throw std::invalid_argument("invalid ip address: " + text);
    }
    return {parse_octet(parts[0]), parse_octet(parts[1]), parse_octet(parts[2]), parse_octet(parts[3])};
}

std::vector<ip_t> read_ips(std::istream &in)
{
    std::vector<ip_t> ips;
    for (std::string line; std::getline(in, line);) {
        if (line.empty()) {
            continue;
        }
        ips.push_back(parse_ip(first_field(line)));
    }
    return ips;
}

void sort_ips_desc(std::vector<ip_t> &ips)
{
    std::sort(ips.begin(), ips.end(), [](const auto &lhs, const auto &rhs) {
        return lhs > rhs;
    });
}

std::string ip_to_string(const ip_t &ip)
{
    std::ostringstream out;
    out << std::get<0>(ip) << '.' << std::get<1>(ip) << '.' << std::get<2>(ip) << '.' << std::get<3>(ip);
    return out.str();
}

std::vector<ip_t> filter_first_byte(const std::vector<ip_t> &ips, int value)
{
    std::vector<ip_t> result;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(result), [value](const auto &ip) {
        return std::get<0>(ip) == value;
    });
    return result;
}

std::vector<ip_t> filter_first_two_bytes(const std::vector<ip_t> &ips, int first, int second)
{
    std::vector<ip_t> result;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(result), [first, second](const auto &ip) {
        return std::get<0>(ip) == first && std::get<1>(ip) == second;
    });
    return result;
}

std::vector<ip_t> filter_any_byte(const std::vector<ip_t> &ips, int value)
{
    std::vector<ip_t> result;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(result), [value](const auto &ip) {
        return std::get<0>(ip) == value || std::get<1>(ip) == value || std::get<2>(ip) == value || std::get<3>(ip) == value;
    });
    return result;
}

void print_ips(const std::vector<ip_t> &ips, std::ostream &out = std::cout)
{
    for (const auto &ip : ips) {
        out << ip_to_string(ip) << '\n';
    }
}

#ifndef IP_FILTER_NO_MAIN
int main()
{
    try {
        auto ips = read_ips(std::cin);
        sort_ips_desc(ips);

        print_ips(ips);
        print_ips(filter_first_byte(ips, 1));
        print_ips(filter_first_two_bytes(ips, 46, 70));
        print_ips(filter_any_byte(ips, 46));
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
#endif
