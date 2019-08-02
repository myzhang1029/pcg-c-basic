/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 * Copyright 2019 Zhang Maiyun
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
 *       http://www.pcg-random.org
 */

/*
 * This code is derived from the full C implementation, which is in turn
 * derived from the canonical C++ PCG implementation. The C++ version
 * has many additional features and is preferable if you can use C++ in
 * your project.
 */

#include "pcg_easy.h"

// pcg32_srandom_r(rng, initstate, initseq):
//     Seed the rng.  Specified in two parts, state initializer and a
//     sequence selection constant (a.k.a. stream id)

void pcg32_srand(pcg32_random_t *rng, uint64_t initstate)
{
    rng->state = 0U;
    rng->inc = ((uint64_t)rng << 1u) | 1u;
    pcg32_rand(rng);
    rng->state += initstate;
    pcg32_rand(rng);
}

void pcg32x2_srand(pcg32x2_random_t *rng, uint64_t seed1, uint64_t seed2)
{
    pcg32_srand(rng->gen, seed1);
    pcg32_srand(rng->gen + 1, seed2);
}

// pcg32_random_r(rng)
//     Generate a uniformly distributed 32-bit random number

uint32_t pcg32_rand(pcg32_random_t *rng)
{
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint64_t pcg32x2_rand(pcg32x2_random_t *rng)
{
    return ((uint64_t)(pcg32_rand(rng->gen)) << 32) | pcg32_rand(rng->gen + 1);
}

uint64_t pcg32x2_uniform(pcg32x2_random_t *rng, uint64_t bound)
{
    uint64_t threshold = -bound % bound;
    for (;;)
    {
        uint64_t r = pcg32x2_rand(rng);
        if (r >= threshold)
            return r % bound;
    }
}
