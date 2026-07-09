#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<int, int, int, int> ip_t;

namespace {

std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> parts;
    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(delim);

    while (stop != std::string::npos) {
        parts.push_back(str.substr(start, stop - start));
        start = stop + 1;
        stop = str.find_first_of(delim, start);
    }

    parts.push_back(str.substr(start));
    return parts;
}

std::string first_field(const std::string &line)
{
    const std::string::size_type tab_pos = line.find('\t');
    return tab_pos == std::string::npos ? line : line.substr(0, tab_pos);
}

int parse_octet(const std::string &text)
{
    if (text.empty()) {
        throw std::invalid_argument("empty octet");
    }

    std::size_t pos = 0;
    const int value = std::stoi(text, &pos);
    if (pos != text.size()) {
        throw std::invalid_argument("invalid octet: " + text);
    }
    if (value < 0 || value > 255) {
        throw std::out_of_range("octet out of range: " + text);
    }

    return value;
}

bool has_any_byte(const ip_t &ip, int value)
{
    return std::get<0>(ip) == value || std::get<1>(ip) == value || std::get<2>(ip) == value || std::get<3>(ip) == value;
}

} // namespace

ip_t parse_ip(const std::string &text)
{
    const std::vector<std::string> parts = split(text, '.');
    if (parts.size() != 4) {
        throw std::invalid_argument("invalid ip address: " + text);
    }

    return ip_t(parse_octet(parts[0]), parse_octet(parts[1]), parse_octet(parts[2]), parse_octet(parts[3]));
}

std::vector<ip_t> read_ips(std::istream &in)
{
    std::vector<ip_t> ips;
    std::string line;

    while (std::getline(in, line)) {
        if (!line.empty()) {
            ips.push_back(parse_ip(first_field(line)));
        }
    }

    return ips;
}

void sort_ips_desc(std::vector<ip_t> &ips)
{
    std::sort(ips.begin(), ips.end(), std::greater<ip_t>());
}

std::string ip_to_string(const ip_t &ip)
{
    std::ostringstream out;
    out << std::get<0>(ip) << '.'
        << std::get<1>(ip) << '.'
        << std::get<2>(ip) << '.'
        << std::get<3>(ip);
    return out.str();
}

std::vector<ip_t> filter_first_byte(const std::vector<ip_t> &ips, int value)
{
    std::vector<ip_t> result;
    std::vector<ip_t>::const_iterator it = ips.begin();

    for (; it != ips.end(); ++it) {
        if (std::get<0>(*it) == value) {
            result.push_back(*it);
        }
    }

    return result;
}

std::vector<ip_t> filter_first_two_bytes(const std::vector<ip_t> &ips, int first, int second)
{
    std::vector<ip_t> result;
    std::vector<ip_t>::const_iterator it = ips.begin();

    for (; it != ips.end(); ++it) {
        if (std::get<0>(*it) == first && std::get<1>(*it) == second) {
            result.push_back(*it);
        }
    }

    return result;
}

std::vector<ip_t> filter_any_byte(const std::vector<ip_t> &ips, int value)
{
    std::vector<ip_t> result;
    std::vector<ip_t>::const_iterator it = ips.begin();

    for (; it != ips.end(); ++it) {
        if (has_any_byte(*it, value)) {
            result.push_back(*it);
        }
    }

    return result;
}

void print_ips(const std::vector<ip_t> &ips, std::ostream &out)
{
    std::vector<ip_t>::const_iterator it = ips.begin();
    for (; it != ips.end(); ++it) {
        out << ip_to_string(*it) << '\n';
    }
}

void process_ips(std::istream &in, std::ostream &out)
{
    std::vector<ip_t> ips = read_ips(in);
    sort_ips_desc(ips);

    print_ips(ips, out);
    print_ips(filter_first_byte(ips, 1), out);
    print_ips(filter_first_two_bytes(ips, 46, 70), out);
    print_ips(filter_any_byte(ips, 46), out);
}

#ifndef IP_FILTER_NO_MAIN
int main()
{
    try {
        process_ips(std::cin, std::cout);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
#endif
