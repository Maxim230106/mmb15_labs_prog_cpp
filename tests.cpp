#define BOOST_TEST_MODULE ip_filter_tests
#define IP_FILTER_NO_MAIN
#include <boost/test/included/unit_test.hpp>

#include <sstream>
#include <string>
#include <vector>

#include "ip_filter.cpp"

BOOST_AUTO_TEST_CASE(parse_ip_to_tuple)
{
    const auto ip = parse_ip("46.70.1.2");
    BOOST_TEST(std::get<0>(ip) == 46);
    BOOST_TEST(std::get<1>(ip) == 70);
    BOOST_TEST(std::get<2>(ip) == 1);
    BOOST_TEST(std::get<3>(ip) == 2);
}

BOOST_AUTO_TEST_CASE(read_ips_ignores_other_fields)
{
    std::istringstream input(
        "1.2.3.4\talpha\tbeta\n"
        "46.70.1.2\tfoo\tbar\n"
    );

    const auto ips = read_ips(input);
    BOOST_TEST(ips.size() == 2u);
    BOOST_TEST((ips[0] == ip_t{1, 2, 3, 4}));
    BOOST_TEST((ips[1] == ip_t{46, 70, 1, 2}));
}

BOOST_AUTO_TEST_CASE(sort_and_filters)
{
    std::vector<ip_t> ips{
        {1, 2, 3, 4},
        {46, 70, 1, 2},
        {46, 70, 5, 6},
        {46, 1, 2, 3},
        {1, 46, 2, 3},
        {255, 0, 0, 1}
    };

    sort_ips_desc(ips);
    BOOST_TEST((ips.front() == ip_t{255, 0, 0, 1}));
    BOOST_TEST((ips.back() == ip_t{1, 2, 3, 4}));

    const auto first_46 = filter_first_byte(ips, 46);
    BOOST_TEST(first_46.size() == 3u);
    BOOST_TEST((first_46[0] == ip_t{46, 70, 5, 6}));

    const auto first_46_70 = filter_first_two_bytes(ips, 46, 70);
    BOOST_TEST(first_46_70.size() == 2u);
    BOOST_TEST((first_46_70[0] == ip_t{46, 70, 5, 6}));
    BOOST_TEST((first_46_70[1] == ip_t{46, 70, 1, 2}));

    const auto any_46 = filter_any_byte(ips, 46);
    BOOST_TEST(any_46.size() == 4u);
}

BOOST_AUTO_TEST_CASE(print_ips_formats_correctly)
{
    std::ostringstream out;
    print_ips({ip_t{1, 2, 3, 4}, ip_t{46, 70, 1, 2}}, out);
    BOOST_TEST(out.str() == std::string("1.2.3.4\n46.70.1.2\n"));
}
