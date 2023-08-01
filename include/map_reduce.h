#pragma once
#include "file_preparation.h"

#include <vector>
#include <string>
#include <functional>

using mapper = std::function<std::string(std::string, size_t)>;
using reducer = std::function<bool(std::vector<std::string>)>;

bool MapReduce(const std::string& file_path, const std::vector<Partition>& borders,
               size_t prefix_len, int rnum, mapper map_func, reducer reduce_func);