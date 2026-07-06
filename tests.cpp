#define BOOST_TEST_MODULE ip_filter_tests
#include <boost/test/included/unit_test.hpp>

#include <sstream>
#include <tuple>
#include <vector>
#include <string>

using ip_t = std::tuple<int, int, int, int>;

std::vector<std::string> split(const std::string &str, char d);
ip_t parse_ip(const std::string &str);
std::vector<ip_t> read_ips(std::istream &in);
void sort_ips_desc(std::vector<ip_t> &ips);
std::vector<ip_t> filter_first_byte(const std::vector<ip_t> &ips, int value);
std::vector<ip_t> filter_first_two_bytes(const std::vector<ip_t> &ips, int first, int second);
std::vector<ip_t> filter_any_byte(const std::vector<ip_t> &ips, int value);

BOOST_AUTO_TEST_CASE(split_by_tab)
{
    const auto parts = split("46.70.1.2\tfoo\tbar", '\t');
    BOOST_REQUIRE_EQUAL(parts.size(), 3u);
    BOOST_TEST(parts[0] == "46.70.1.2");
    BOOST_TEST(parts[1] == "foo");
    BOOST_TEST(parts[2] == "bar");
}

BOOST_AUTO_TEST_CASE(parse_ip_to_tuple)
{
    const auto ip = parse_ip("1.2.3.4");
    BOOST_TEST(std::get<0>(ip) == 1);
    BOOST_TEST(std::get<1>(ip) == 2);
    BOOST_TEST(std::get<2>(ip) == 3);
    BOOST_TEST(std::get<3>(ip) == 4);
}

BOOST_AUTO_TEST_CASE(sort_and_filters)
{
    std::vector<ip_t> ips{
        {1, 2, 3, 4},
        {46, 70, 1, 2},
        {46, 1, 2, 3},
        {1, 10, 1, 1},
        {1, 2, 3, 3}
    };

    sort_ips_desc(ips);
    BOOST_TEST(std::get<0>(ips.front()) == 46);
    BOOST_TEST(std::get<1>(ips.front()) == 70);
    BOOST_TEST(std::get<2>(ips.front()) == 1);
    BOOST_TEST(std::get<3>(ips.front()) == 2);

    const auto first1 = filter_first_byte(ips, 1);
    BOOST_REQUIRE_EQUAL(first1.size(), 3u);

    const auto first46_70 = filter_first_two_bytes(ips, 46, 70);
    BOOST_REQUIRE_EQUAL(first46_70.size(), 1u);
    BOOST_TEST(std::get<0>(first46_70.front()) == 46);
    BOOST_TEST(std::get<1>(first46_70.front()) == 70);

    const auto any46 = filter_any_byte(ips, 46);
    BOOST_REQUIRE_EQUAL(any46.size(), 2u);
}

BOOST_AUTO_TEST_CASE(read_ips_ignores_other_fields)
{
    std::istringstream input(
        "1.2.3.4\ttext2\ttext3\n"
        "46.70.1.2\tignore\tme\n"
    );

    const auto ips = read_ips(input);
    BOOST_REQUIRE_EQUAL(ips.size(), 2u);
    BOOST_TEST(std::get<0>(ips[0]) == 1);
    BOOST_TEST(std::get<1>(ips[1]) == 70);
}
