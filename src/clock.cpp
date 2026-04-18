#include <sys/time.h>
#include <cstdint>

// namespace ch = std::chrono;
// using namespace std::chrono_literals;

namespace cl {

int64_t time_since_epoch() {
    struct timeval tp;
    gettimeofday(&tp, nullptr);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}
    

}