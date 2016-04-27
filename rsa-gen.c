#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
#include <string.h>

void generate_keys(unsigned bits, unsigned seed)
{
    mpz_t p, q, n, pm1, qm1, t, e, d;
    gmp_randstate_t state;
    FILE *out;
    int i;

    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(pm1);
    mpz_init(qm1);
    mpz_init(t);
    mpz_init(e);
    mpz_init(d);

    /* GMP allows you to choose your random number generator.  mt is *
     * Mersenne Twister, which is generally considered a secure      *
     * algorithm.                                                    */
    gmp_randinit_mt(state);
    /* Seed the random number generator.  Using time is generally a  *
     * bad idea, but mixing in microseconds like this improves it    *
     * somewhat (still not great).                                   */
    gmp_randseed_ui(state, seed);
    do {
        /* Choose two large, random primes, p and q.. */
        mpz_urandomb(p, state, bits);
        mpz_nextprime(p, p);
        mpz_urandomb(q, state, bits);
        mpz_nextprime(q, q);

        /* Multiply them together to get the modulus, n, which will be *
         * part of both the public and private keys.                   */
        mpz_mul(n, p, q);

        mpz_sub_ui(pm1, p, 1);
        mpz_sub_ui(qm1, q, 1);
        /* t, also called Phi(n) is Euler's Totient of n.  Euler's     *
         * Totient is the number of integers relatively prime to n.    *
         * This is useful because Euler proved that                    *
         * m^Phi(n) = 1 (mod n) for all m.  This implies that:         *
         *   m^(k*Phi(n)) = 1 (mod n)                                  *
         *   m*m^(k*Phi(n) = m (mod n)                                 *
         *   m^(k*Phi(n)+1) = m (mod n)                                *
         *   m^(e*d) = m (mod n)                                       *
         *   d = (k*Phi(n)+1) / e                                      *
         * This makes t useful as the modulus used to calculate d,     *
         * which is the other half of the private key.                 */
        mpz_mul(t, pm1, qm1);

        /* People like to use 65537 for e, because the choice of e     *
         * doesn't matter much, and small values make encryption fast. *
         * (n,e) is the public key.                                    */
        mpz_set_ui(e, 65537);

        /* d is the multiplicative inverse of e mod t.  In other       *
         * words, d * e = 1 (mod t).  (n,d) is the private key.        */
        mpz_invert(d, e, t);

        /* If d is negative, we need to add t to get it positive. */
        if ((i = mpz_cmp_ui(d, 0)) < 0) {
            mpz_add(d, d, t);
        }

        /* It's possible that e is not invertable, in which case, we   *
         * need to start over.                                         */
    } while (!i);

    out = fopen("gmp.pub", "w");
    mpz_out_raw(out, n);
    mpz_out_raw(out, e);
    fclose(out);

    out = fopen("gmp.prv", "w");
    mpz_out_raw(out, n);
    mpz_out_raw(out, d);
    fclose(out);

    gmp_randclear(state);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(pm1);
    mpz_clear(qm1);
    mpz_clear(t);
    mpz_clear(e);
    mpz_clear(d);
}

int main(int argc, char *argv[])
{
    generate_keys(atoi(argv[1]), atoi(argv[2]));
    return 0;
}
