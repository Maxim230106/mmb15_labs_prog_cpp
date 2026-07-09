#define IP_FILTER_NO_MAIN

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ip_filter.cpp"

namespace {

void fail(const std::string &message)
{
    throw std::runtime_error(message);
}

void expect_true(bool condition, const std::string &message)
{
    if (!condition) {
        fail(message);
    }
}

void test_parse_ip()
{
    const ip_t ip = parse_ip("46.70.1.2");
    expect_true(std::get<0>(ip) == 46, "parse_ip first octet");
    expect_true(std::get<1>(ip) == 70, "parse_ip second octet");
    expect_true(std::get<2>(ip) == 1, "parse_ip third octet");
    expect_true(std::get<3>(ip) == 2, "parse_ip fourth octet");
}

void test_read_ips()
{
    std::istringstream input("1.2.3.4\talpha\tbeta\n46.70.1.2\tfoo\tbar\n");
    const std::vector<ip_t> ips = read_ips(input);

    expect_true(ips.size() == 2u, "read_ips size");
    expect_true(ips[0] == ip_t(1, 2, 3, 4), "read_ips first ip");
    expect_true(ips[1] == ip_t(46, 70, 1, 2), "read_ips second ip");
}

void test_sort_and_filters()
{
    std::vector<ip_t> ips;
    ips.push_back(ip_t(1, 2, 3, 4));
    ips.push_back(ip_t(46, 70, 1, 2));
    ips.push_back(ip_t(46, 70, 5, 6));
    ips.push_back(ip_t(46, 1, 2, 3));
    ips.push_back(ip_t(1, 46, 2, 3));
    ips.push_back(ip_t(255, 0, 0, 1));

    sort_ips_desc(ips);

    expect_true(ips.front() == ip_t(255, 0, 0, 1), "sort_ips_desc first item");
    expect_true(ips.back() == ip_t(1, 2, 3, 4), "sort_ips_desc last item");

    const std::vector<ip_t> first_46 = filter_first_byte(ips, 46);
    expect_true(first_46.size() == 3u, "filter_first_byte size");
    expect_true(first_46[0] == ip_t(46, 70, 5, 6), "filter_first_byte order");

    const std::vector<ip_t> first_46_70 = filter_first_two_bytes(ips, 46, 70);
    expect_true(first_46_70.size() == 2u, "filter_first_two_bytes size");
    expect_true(first_46_70[0] == ip_t(46, 70, 5, 6), "filter_first_two_bytes first item");
    expect_true(first_46_70[1] == ip_t(46, 70, 1, 2), "filter_first_two_bytes second item");

    const std::vector<ip_t> any_46 = filter_any_byte(ips, 46);
    expect_true(any_46.size() == 4u, "filter_any_byte size");
}

void test_print_ips()
{
    std::vector<ip_t> ips;
    ips.push_back(ip_t(1, 2, 3, 4));
    ips.push_back(ip_t(46, 70, 1, 2));

    std::ostringstream out;
    print_ips(ips, out);

    expect_true(out.str() == "1.2.3.4\n46.70.1.2\n", "print_ips formatting");
}

void test_process_ips()
{
    std::istringstream input(
        "46.70.1.2\tfoo\tbar\n"
        "1.2.3.4\tfoo\tbar\n"
        "1.5.6.7\tfoo\tbar\n"
        "46.1.2.3\tfoo\tbar\n"
        "5.6.46.7\tfoo\tbar\n");
    std::ostringstream out;

    process_ips(input, out);

    expect_true(
        out.str() ==
            "46.70.1.2\n"
            "46.1.2.3\n"
            "5.6.46.7\n"
            "1.5.6.7\n"
            "1.2.3.4\n"
            "1.5.6.7\n"
            "1.2.3.4\n"
            "46.70.1.2\n"
            "46.70.1.2\n"
            "46.1.2.3\n"
            "5.6.46.7\n",
        "process_ips full output");
}

} // namespace

int main()
{
    try {
        test_parse_ip();
        test_read_ips();
        test_sort_and_filters();
        test_print_ips();
        test_process_ips();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
