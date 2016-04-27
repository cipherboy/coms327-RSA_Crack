#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
  FILE *pt, *ct;
  FILE *pub, *prv;
  char *string;
  int i, bytes, c;
  int bits = 4096;
  char ch;

  mpz_t n, pm1, e, p, q, pm2, pm3;

  mpz_init(n);
  mpz_init(e);
  mpz_init(pm2);
  mpz_init(pm3);

  pub = fopen("gmp2.pub", "r");
  mpz_inp_raw(n, pub);
  mpz_inp_raw(e, pub);

  gmp_randstate_t state;
  FILE *out;
  struct timeval tv;

  mpz_set_ui(e, 65537);

  gmp_randinit_mt(state);
  gettimeofday(&tv, NULL);

  unsigned int seed = atoi(argv[1]);
  unsigned int scount = atoi(argv[2]);

  unsigned int loops = 0;

  int j = 0;

  mpz_init(p);
  mpz_init(q);
  mpz_init(pm1);

  for (; loops < scount; loops++) {
    gmp_randseed_ui(state,  seed+loops);

    mpz_urandomb(p, state, bits);
    mpz_urandomb(q, state, bits);

    mpz_mul(pm1, p, q);
    mpz_set(pm2, pm1);
    mpz_mul_ui(pm3, p, 66000);
    mpz_add(pm2, pm2, pm3);
    mpz_mul_ui(pm3, q, 66000);
    mpz_add(pm2, pm2, pm3);
    mpz_mul_ui(pm3, e, 66000);
    mpz_add(pm2, pm2, pm3);

    if (mpz_cmp(n, pm1) >= 0) {
      if (mpz_cmp(pm2, n) >= 0) {
        printf("Found seed candidate: %u\n", seed+loops);
        gmp_printf("%Zd\n", p);
      }
    }
  }
  mpz_clear(p);
  mpz_clear(q);
  mpz_clear(pm1);
  mpz_clear(pm2);
  mpz_clear(pm3);
  gmp_randclear(state);
  mpz_clear(n);
  mpz_clear(e);

  return 2;
}
