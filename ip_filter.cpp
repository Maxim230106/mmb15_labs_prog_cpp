#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using ip_t = std::tuple<int, int, int, int>;
using byte_t = std::remove_cvref_t<decltype(std::get<0>(std::declval<ip_t>()))>;
static_assert(std::is_same_v<byte_t, int>);

std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> parts;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        parts.push_back(str.substr(start, stop - start));
        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    parts.push_back(str.substr(start));
    return parts;
}

ip_t parse_ip(const std::string &str)
{
    const auto parts = split(str, '.');
    if (parts.size() != 4)
    {
        throw std::runtime_error("Invalid IPv4 address: " + str);
    }

    return std::make_tuple(
        std::stoi(parts[0]),
        std::stoi(parts[1]),
        std::stoi(parts[2]),
        std::stoi(parts[3])
    );
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

std::vector<ip_t> read_ips(std::istream &in)
{
    std::vector<ip_t> ips;

    for (std::string line; std::getline(in, line);)
    {
        if (line.empty())
        {
            continue;
        }

        const auto fields = split(line, '\t');
        if (!fields.empty())
        {
            ips.push_back(parse_ip(fields.front()));
        }
    }

    return ips;
}

void sort_ips_desc(std::vector<ip_t> &ips)
{
    std::sort(ips.begin(), ips.end(), [](const auto &lhs, const auto &rhs)
    {
        return lhs > rhs;
    });
}

std::vector<ip_t> filter_first_byte(const std::vector<ip_t> &ips, int value)
{
    std::vector<ip_t> result;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(result), [value](const auto &ip)
    {
        return std::get<0>(ip) == value;
    });
    return result;
}

std::vector<ip_t> filter_first_two_bytes(const std::vector<ip_t> &ips, int first, int second)
{
    std::vector<ip_t> result;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(result), [first, second](const auto &ip)
    {
        return std::get<0>(ip) == first && std::get<1>(ip) == second;
    });
    return result;
}

std::vector<ip_t> filter_any_byte(const std::vector<ip_t> &ips, int value)
{
    std::vector<ip_t> result;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(result), [value](const auto &ip)
    {
        return std::get<0>(ip) == value ||
               std::get<1>(ip) == value ||
               std::get<2>(ip) == value ||
               std::get<3>(ip) == value;
    });
    return result;
}

void print_ips(const std::vector<ip_t> &ips)
{
    for (const auto &ip : ips)
    {
        std::cout << ip_to_string(ip) << '\n';
    }
}

#ifndef IP_FILTER_NO_MAIN
int main(int, char **)
{
    try
    {
        auto ips = read_ips(std::cin);
        sort_ips_desc(ips);

        print_ips(ips);
        print_ips(filter_first_byte(ips, 1));
        print_ips(filter_first_two_bytes(ips, 46, 70));
        print_ips(filter_any_byte(ips, 46));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#endif
