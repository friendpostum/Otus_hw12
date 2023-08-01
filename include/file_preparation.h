#pragma once
#include <filesystem>
#include <vector>

using pos = std::streampos;

struct Partition {
    Partition(pos begin, pos end) noexcept:
            begin(begin),
            end(end)
            {}

    pos begin;
    pos end;
};

std::vector<Partition> split_file(const std::filesystem::path& path, unsigned int blocks_nmb);
