#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    printf("Hello, world!\n");
    for (auto &s : args) {
        printf("#%s#\n", s.c_str());
    }
    while (true)
        ;
}
