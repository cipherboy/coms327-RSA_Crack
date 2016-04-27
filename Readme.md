# COMS 327 - RSA Public Key cracker

Need to know the relative time of the key's creation:

4:51PM on Friday (time from Jeremy--different from class)

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
