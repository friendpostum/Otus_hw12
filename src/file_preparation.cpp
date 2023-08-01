#include "file_preparation.h"

#include <fstream>
#include <iostream>
#include <cmath>

std::vector<Partition> split_file(const std::filesystem::path &path, unsigned int part_nmb) {
    std::vector<Partition> result;
    result.reserve(part_nmb);

    pos file_sz = std::filesystem::file_size(path);
    pos partSzAsDouble = std::ceil(static_cast<double>(file_sz) / part_nmb);

    auto part_sz = static_cast<pos>(partSzAsDouble);

    std::ifstream file;
    file.exceptions(std::ifstream::failbit);
    file.open(path.c_str(), std::ios::binary);

    pos part_begin = 0;
    pos part_end = 0;

    while (part_end < file_sz) {
        part_end += part_sz;
        if (part_end > file_sz) {
            part_end = file_sz;
        } else {
            file.seekg(part_end);

            file.ignore(file_sz - part_end, '\n');
            if (file.eof()) {
                part_end = file_sz;
            } else {
                part_end = file.tellg();
                if (part_end == -1) {
                    throw std::runtime_error("Error reading the file.");
                }
            }
        }

        result.emplace_back(part_begin, part_end);
        part_begin = part_end + pos(1);
    }

    return result;
}
