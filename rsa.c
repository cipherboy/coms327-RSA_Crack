#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
#include <sys/time.h>
#include <string.h>

// Copyright (C) Jeremy Sheaffer

/* Implementaation of the Extended Euclidean Algorithm (to find GCD) */
void mod_inv(mpz_t result, const mpz_t a, const mpz_t n)
{
    int i;
    mpz_t tmp;
    mpz_t num;
    mpz_t den;
    mpz_t quot[3];
    mpz_t p[2];

    mpz_init(tmp);
    mpz_init(num);
    mpz_init(den);
    mpz_init(quot[0]);
    mpz_init(quot[1]);
    mpz_init(quot[2]);
    mpz_init(p[0]);
    mpz_init(p[1]);

    mpz_set_ui(quot[0], 1);
    mpz_set_ui(quot[1], 1);
    mpz_set_ui(quot[2], 1);

    mpz_set_ui(p[0], 0);
    mpz_set_ui(p[1], 1);

    if (mpz_cmp(a, n) > 0) {
        mpz_set(num, a);
        mpz_set(den, n);
    } else {
        mpz_set(num, n);
        mpz_set(den, a);
    }

    for (i = 0; mpz_cmp_ui(den, 0); i++) {
        mpz_div(quot[i % 3], num, den);
        mpz_mod(tmp, num, den);
        mpz_set(num, den);
        mpz_set(den, tmp);
        if (i >= 2) {
            mpz_mul(tmp, quot[(i - 2) % 3], p[(i - 1) % 2]);
            mpz_sub(tmp, p[(i - 2) % 2], tmp);
            if (mpz_cmp_ui(tmp, 0) < 0) {
                mpz_add(tmp, tmp, n);
            }
            mpz_set(p[i % 2], tmp);
        }
    }

    mpz_mul(tmp, quot[(i - 2) % 3], p[(i - 1) % 2]);
    mpz_sub(tmp, p[(i - 2) % 2], tmp);
    if (mpz_cmp_ui(tmp, 0) < 0) {
        mpz_add(tmp, tmp, n);
    }
    mpz_set(result, tmp);

    mpz_clear(tmp);
    mpz_clear(num);
    mpz_clear(den);
    mpz_clear(quot[0]);
    mpz_clear(quot[1]);
    mpz_clear(quot[2]);
    mpz_clear(p[0]);
    mpz_clear(p[1]);
}

void generate_keys(unsigned bits)
{
    mpz_t p, q, n, pm1, qm1, t, e, d;
    gmp_randstate_t state;
    FILE *out;
    struct timeval tv;
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
    gettimeofday(&tv, NULL);
    /* Seed the random number generator.  Using time is generally a  *
     * bad idea, but mixing in microseconds like this improves it    *
     * somewhat (still not great).                                   */
    gmp_randseed_ui(state, (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff);
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

int usage(char *s)
{
    int n;

    fprintf(stderr, "Usage: %s %n-gen <bits>                   |\n", s, &n);
    fprintf(stderr,          "%*s-enc <plaintext> <cyphertext> |\n"
            "%*s-dec <cyphertext> <plaintext>\n", n, "", n, "");

    return 0;
}

int main(int argc, char *argv[])
{
    mpz_t val, n, e, d;
    FILE *pt, *ct;
    FILE *pub, *prv;
    char *string;
    int i, bytes, c;
    char ch;

    if (argc < 3) {
        usage(argv[0]);

        return -1;
    }

    if (!strcmp(argv[1], "-gen")) {
        if (argc != 3) {
            usage(argv[0]);

            return -1;
        }

        generate_keys(atoi(argv[2]));
    }

    mpz_init(val);
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);

    if (!strcmp(argv[1], "-enc")) {
        if (argc != 4) {
            usage(argv[0]);

            return -1;
        }

        pub = fopen("gmp.pub", "r");
        pt = fopen(argv[2], "r");
        ct = fopen(argv[3], "w");
        mpz_inp_raw(n, pub);
        mpz_inp_raw(e, pub);

        bytes = mpz_sizeinbase(n, 16) - 2;
        string = calloc(1, bytes + 1);

        while (!feof(pt)) {
            for (i = 0; (i < bytes) && ((c = fgetc(pt)) != EOF); i += 2)
                sprintf(string + i, "%02hhx", c);

            mpz_set_str(val, string, 16);
            if (mpz_cmp(val, n) > 0) {
                fprintf(stderr, "Value is too large!\n");
            }
            mpz_powm(val, val, e, n);
            mpz_out_raw(ct, val);
        }

        free(string);
    }

    if (!strcmp(argv[1], "-dec")) {
        if (argc != 4) {
            usage(argv[0]);

            return -1;
        }

        prv = fopen("gmp.prv", "r");
        ct = fopen(argv[2], "r");
        pt = fopen(argv[3], "w");

        mpz_inp_raw(n, prv);
        mpz_inp_raw(d, prv);

        bytes = mpz_sizeinbase(n, 16);
        /* hack: mpz_get_str() drops the leading zero (if present).  Allocate *
         * space for an extra byte, put a zero in the front, increment string *
         * so that the zero is "not part of it", then, when the length is     *
         * odd, start the work at index -1.                                   */
        string = calloc(1, bytes + 2);
        string[0] = '0';
        string++;

        while (mpz_inp_raw(val, ct)) {
            mpz_powm(val, val, d, n);
            mpz_get_str(string, 16, val);

            for (i = ((strlen(string) & 1) ? -1 : 0);
                 (i < bytes) && (1 == sscanf(string + i, "%02hhx", &ch));
                 i += 2) {
                fprintf(pt, "%c", ch);
            }
        }

        free(string - 1);
    }

    mpz_clear(val);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);

    return 0;
}
