#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

std::tuple<int,int,int,int> parse_ip(const std::string &str)
{
    auto parts = split(str, '.');
    return std::make_tuple(std::stoi(parts[0]), std::stoi(parts[1]), std::stoi(parts[2]), std::stoi(parts[3]));
}

int main(int, char **)
{
    try
    {
        std::vector<std::tuple<int,int,int,int>> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            ip_pool.push_back(parse_ip(split(line, '\t').at(0)));
        }

        // TODO reverse lexicographically sort

        // сортировка в обратном лексикографическом порядке по числам/байтам

        std::sort(ip_pool.begin(), ip_pool.end(), std::greater<std::tuple<int,int,int,int>>());

        //функция для вывода кортежа
        auto print_ip = [](const std::tuple<int,int,int,int> &t){
            std::cout << std::get<0>(t) << "." << std::get<1>(t) << "."
                      << std::get<2>(t) << "." << std::get<3>(t) << "\n";
        };

        // 1) Вывод всего отсортированного списка
        for (const auto &ip : ip_pool) print_ip(ip);

        // 2) Вывод IP, у которых первые байты равны 46 и 70
        for (const auto &ip : ip_pool)
        {
            if (std::get<0>(ip) == 46 && std::get<1>(ip) == 70)
                print_ip(ip);
        }

        // 3) Вывод IP, где любой байт равен 46
        for (const auto &ip : ip_pool)
        {
            if (std::get<0>(ip) == 46 || std::get<1>(ip) == 46 ||
                std::get<2>(ip) == 46 || std::get<3>(ip) == 46)
            {
                print_ip(ip);
            }
        }

    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
