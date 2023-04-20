// #include <NE10.h>
#include <cstdint>

typedef int32_t ne10_int32_t;
typedef struct ne10_fft_state_float32_t* ne10_fft_cfg_float32_t;
extern ne10_fft_cfg_float32_t (*ne10_fft_alloc_c2c_float32)(ne10_int32_t nfft);

int main() {
    auto plan = ne10_fft_alloc_c2c_float32(512);
    return 0;
}