#include <map>
#include <string>
#include <cstdio>

int main()
{
    auto strings = std::map<int, std::string> {};
    auto it = end(strings);
    auto last = it;

    auto add = [&](int x, std::string s) {
        strings.emplace(x, std::move(s));
    };

    auto print_next = [&]() {
        if (last != end(strings)) {
            it = next(last);
        } else {
            it = begin(strings);
        }
        if (it != end(strings)) {
            last = it;
            printf("%d: %s\n", it->first, it->second.c_str());
        }
    };

    add(3, "hallo");
    add(5, "welt");
    add(2, "erstes");
    print_next(); // 2
    add(9, "neun");
    add(1, "ignoriert");
    print_next(); // 3
    print_next(); // 5
    print_next(); // 9
    print_next(); // -
    add(9, "nochmal ignoriert");
    add(7, "sieben ignoriert");
    print_next(); // -
    add(10, "weiter");
    print_next(); // 10
    print_next(); // -
    print_next(); // -
}
