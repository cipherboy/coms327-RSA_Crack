#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

int main(int argc, char *argv[])
{
    FILE *pub;
    int i;
    int bits = 4096;

    mpz_t n, p, q, pm1, pm2, pm3;

    unsigned int seed = atoi(argv[1]);
    unsigned int scount = atoi(argv[2]);
    unsigned int loops = 0;

    mpz_init(n);
    mpz_init(p);
    mpz_init(q);
    mpz_init(pm1);
    mpz_init(pm2);
    mpz_init(pm3);

    pub = fopen("gmp2.pub", "r");
    mpz_inp_raw(n, pub);

    gmp_randstate_t state;

    gmp_randinit_mt(state);

    int j = 0;

    // Loop over all seeds. 
    for (; loops < scount; loops++) {
        gmp_randseed_ui(state,  seed+loops);

        mpz_urandomb(p, state, bits);
        mpz_urandomb(q, state, bits);

        mpz_mul(pm1, p, q);
        mpz_set(pm2, pm1);
        // a = 1,600,000,000
        mpz_mul_ui(pm3, p, 1600000000);
        mpz_add(pm2, pm2, pm3);

        // p' = p + c
        // q' = q + d
        //
        // pq <= n <= pq + ap
        //
        // Where a is a nice large constant such that
        // pq <= n <= pq + pd + qc + cd <=  pq + ap
        if (mpz_cmp(n, pm1) >= 0) {
            if (mpz_cmp(pm2, n) >= 0) {
                printf("Found seed candidate: %u\n", seed+loops);
                gmp_printf("%Zd\n", p);

                // Not sure if only candidate so don't return
            }
        }
    }
    mpz_clear(n);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(pm1);
    mpz_clear(pm2);
    mpz_clear(pm3);
    gmp_randclear(state);

    return 0;
}
