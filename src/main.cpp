#include <iostream>

#include "file_preparation.h"
#include "map_reduce.h"

constexpr int max_prefix_lenght = 100;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Need format <path> <mnum> <rnum>" << std::endl;
        return -1;
    }
    int mnum = std::stoi(argv[2]);
    int rnum = std::stoi(argv[3]);
    if (mnum < 1 || rnum < 1) {
        std::cout << "mnum and rnum must be > 0" << std::endl;
        return -1;
    }

    std::vector<Partition> partitions = split_file(argv[1], mnum);

    auto mapper = [](const std::string& s, size_t len) {
        return s.substr(0, len);
    };

    auto reducer = [](std::vector<std::string> buffer){
        std::string last_string;
        for (size_t i = 0; i < buffer.size(); ++i) {
            if (i && buffer[i] == last_string)
                return false;
            last_string = buffer[i];
        }

        return true;
    };

    int prefix_len = 1;
    while (prefix_len++ < max_prefix_lenght) {
        bool res = MapReduce(argv[1], partitions, prefix_len, rnum, mapper, reducer);
        if (res) break;
    }

    if (prefix_len == max_prefix_lenght)
        std::cout << "Map-reduce error\n";
    else
        std::cout << "Minimal prefix lenght " << prefix_len << std::endl;
    
}