/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

#include <stdint.h>

#include "pcg_basic.h"

/*
 * This code shows how you can cope if you're on a 32-bit platform (or a
 * 64-bit platform with a mediocre compiler) that doesn't support 128-bit math,
 * or if you're using the basic version of the library which only provides
 * 32-bit generation.
 *
 * Here we build a 64-bit generator by tying together two 32-bit generators.
 * Note that we can do this because we set up the generators so that each
 * 32-bit generator has a *totally different* different output sequence
 * -- if you tied together two identical generators, that wouldn't be nearly
 * as good.
 *
 * For simplicity, we keep the period fixed at 2^64.  The state space is
 * approximately 2^254 (actually  2^64 * 2^64 * 2^63 * (2^63 - 1)), which
 * is huge.
 */

typedef struct {
    pcg32_random_t gen[2];
} pcg32x2_random_t;

void pcg32x2_srandom_r(pcg32x2_random_t* rng, uint64_t seed1, uint64_t seed2,
                       uint64_t seq1,  uint64_t seq2)
{
    uint64_t mask = ~0ull >> 1;
    // The stream for each of the two generators *must* be distinct
    if ((seq1 & mask) == (seq2 & mask)) 
        seq2 = ~seq2;
    pcg32_srandom_r(rng->gen,   seed1, seq1);
    pcg32_srandom_r(rng->gen+1, seed2, seq2);
}

uint64_t pcg32x2_random_r(pcg32x2_random_t* rng)
{
    return ((uint64_t)(pcg32_random_r(rng->gen)) << 32)
           | pcg32_random_r(rng->gen+1);
}

/* See other definitons of ..._boundedrand_r for an explanation of this code. */

uint64_t pcg32x2_boundedrand_r(pcg32x2_random_t* rng, uint64_t bound)
{
    uint64_t threshold = -bound % bound;
    for (;;) {
        uint64_t r = pcg32x2_random_r(rng);
        if (r >= threshold)
            return r % bound;
    }
}
