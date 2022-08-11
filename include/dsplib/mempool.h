#pragma once

#include <cstddef>
#include <vector>

namespace dsplib {

//TODO: align ptr by type
void* pool_alloc(size_t size);

void pool_free(void* ptr);

//TODO: allocated/free
std::vector<int> pool_info();

}   // namespace dsplib