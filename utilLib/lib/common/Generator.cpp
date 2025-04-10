#include "common/Generator.h"

namespace generators {
thread_local generator<std::mt19937> gen_mt19937 = generator<std::mt19937>();
}