/*
 * Code for computing the Entropy of 1s of a binary string.
 *
 * See:
 *
 * "Generating high-quality random numbers in parallel by
 * cellular automata", Tomassini et.al., 1999.
 *
 */

#ifndef __CA__ENTROPY__METER__
#define __CA__ENTROPY__METER__

typedef struct EntropyMeter EntropyMeter;

/* Create an entropy meter and call reset on it */
EntropyMeter *    entropy_meter_new               (int h);

/* Reset the entropy meter */
void              entropy_meter_reset             (EntropyMeter *);

/* Free the entropy meter */
void              entropy_meter_destroy           (EntropyMeter *);

/* Take a value into account */
void              entropy_meter_count             (EntropyMeter *, int value);

/* Get an entropy measure */
long double       entropy_meter_get_measure       (EntropyMeter *);

#endif
