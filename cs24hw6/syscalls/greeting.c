#include <stdint.h>

int64_t output(void *data, uint64_t size);


/* INCOMING TRANSMISSION... */
int main() {
    /* Our alien alphabet only has 8 letters. */
    uint64_t P = 1,
             E = 2,
             T = 5,
             A = 5,
             L = 7,
             N = 124314007,
             R = 271173410,
             G = 1479296389;

    /* Say hello to the locals. */
    uint64_t msg[] = { G * R * E * E * T , P * L * A * N * E * T };
    output(msg, sizeof(msg));

    return 0;
}

