#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
  unsigned int seconds = atoi(argv[1]);
  unsigned int microseconds = atoi(argv[2]);
  unsigned int seed = (microseconds ^ (seconds << 20)) & 0xffffffff;
  printf("Seed: %u\n", seed);
}
