# COMS 327 - RSA Public Key cracker

NOTE: rsa.c is code from Jeremy Sheaffer, Iowa State University.

Need to know the relative time of the key's creation:

    4:51PM on Friday (time from Jeremy--different from class).

## Overview
To build all programs:

    for i in \*.sh; do chmod +x $i; bash $i; done

To attack a key (gmp2.pub), find the starting timestamp using
date, then use `rsa-seed <timestamp> 0` to determine the starting
and ending seeds. Use `rsa-crack <seed> <range>` to attack.
Can be (manually) parallelized over multiple machines.

Once the seed is found, use `rsa-gen <bits> <seed>` to recreate the
private key or `rsa-reverse <start time> <end time> <seed>` to find
the time the key was generated.

Further, due to the way the seeds are constructed, we know that, of the lower
20 bits, there are only 1 million possible combinations of arrangements;
that is, there are 48576k unused values in the lower 20 bits of the seed.
Thus, over the entire seed, 2^12 * (2^20 - 1000000) unused seeds, which is
198,967,296. Out of the entire 2^32 search space, this is ~4.6% of possible
seeds that are unused. Therefore, a strict brute force approach over the entire
key space as implemented in rsa-crack searches some unnecessary values, but is
relatively small in comparison to the entire search space.  

## Start
    date --date="Apr 22 16:50:00 CDT 2016" +"%s"
    1461361800.0
    Starting seed: 3363831808

## Seed
    date --date="Apr 22 16:51:15 CDT 2016" +"%s"
    1461361875.935470
    Found: 3443410478

## End
    date --date="Apr 22 16:51:59 CDT 2016" +"%s"
    1461361919.999999
    Ending seed: 3489612351
