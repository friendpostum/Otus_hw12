#include "map_reduce.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <future>

using MrVector = std::shared_ptr<std::vector<std::string>>;
using BuffStr = std::vector<std::string>;

void Map(const std::string &file_name, Partition str_border, mapper &func, size_t len, BuffStr &out) {
    std::ifstream ifs(file_name);

    ifs.seekg(str_border.begin);

    std::string line;
    while (ifs.tellg() < std::streamoff(str_border.end)) {
        ifs >> line;
        out.push_back(func(line, len));

        if (ifs.tellg() == -1) break;
    }

    std::sort(out.begin(), out.end());
}

bool Reduce(BuffStr &input, reducer &func) { return func(input); }

bool AllMapBuffersIsEmpty(const std::vector<BuffStr> &map_output) {
    if (std::any_of(map_output.cbegin(), map_output.cend(), [](auto i) { return !i.empty(); }))
        return false;

    return true;
}

void CopyAllDuplicates(const std::string duplicate, BuffStr &reduce_input, std::vector<BuffStr> &map_buffers) {
    for (auto &map_vecstr: map_buffers) {
        while (true) {
            auto it = std::find(map_vecstr.begin(), map_vecstr.end(), duplicate);
            if (it == map_vecstr.end()) break;

            reduce_input.push_back(duplicate);
            map_vecstr.erase(it);
        }
    }
}

void Shuffle(std::vector<BuffStr> &map_buffers, std::vector<BuffStr> &reduce_buffers) {
    for (size_t i = 0, j = 0;; ++i, ++j) {
        if (i == map_buffers.size()) i = 0;
        if (j == reduce_buffers.size()) j = 0;

        //if (AllMapBuffersIsEmpty(map_buffers)) break;
        if (std::all_of(map_buffers.cbegin(), map_buffers.cend(), [](auto i) { return i.empty(); }))
            break;

        if (!map_buffers[i].empty())
            CopyAllDuplicates(map_buffers[i].front(), reduce_buffers[j], map_buffers);
    }
}

bool MapReduce(const std::string &file_path, const std::vector<Partition> &borders,
               size_t prefix_len, int rnum, mapper map_func, reducer reduce_func) {

    // Map
    std::vector<BuffStr> map_buffers(borders.size());

    std::vector<std::thread> threads;
    threads.reserve(borders.size());
    for (int i = 0; i < borders.size(); ++i) {
        threads.emplace_back(Map, ref(file_path), borders[i], std::ref(map_func),
                             prefix_len, std::ref(map_buffers[i]));
    }

    for (auto &thread: threads)
        thread.join();

/*    for (int i = 0; i < borders.size(); ++i) {
        Map(file_path, borders[i], map_func, prefix_len, map_buffers[i]);
    }*/

    // Shuffle
    std::vector<BuffStr> reduce_buffers(rnum);
    Shuffle(map_buffers, reduce_buffers);

    // Reduce
    std::vector<std::future<bool>> futures;
    for (auto &buffer: reduce_buffers) {
        auto future = std::async(Reduce, std::ref(buffer), std::ref(reduce_func));
        futures.push_back(std::move(future));
    }

    for (auto &future: futures) {
        if (!future.get())
            return false;
    }

    return true;
}