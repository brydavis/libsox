/*
 * July 5, 1991
 * Copyright 1991 Lance Norskog And Sundry Contributors
 * This source code is freely redistributable and may be used for
 * any purpose.  This copyright notice must be maintained. 
 * Lance Norskog And Sundry Contributors are not responsible for 
 * the consequences of using this software.
 */

/*
 * Sound Tools masking noise effect file.
 */

#include <stdlib.h>
#include <math.h>
#include "st_i.h"

static st_effect_t st_mask_effect;

#define HALFABIT 1.44                   /* square root of 2 */

/*
 * Problems:
 *      1) doesn't allow specification of noise depth
 *      2) does triangular noise, could do local shaping
 *      3) can run over 32 bits.
 */

/*
 * Process options
 */
static int st_mask_getopts(eff_t effp UNUSED, int n, char **argv UNUSED)
{
        if (n)
        {
                st_fail(st_mask_effect.usage);
                return (ST_EOF);
        }
        /* should take # of bits */

        st_initrand();
        return (ST_SUCCESS);
}

/*
 * Processed signed long samples from ibuf to obuf.
 * Return number of samples processed.
 */
static int st_mask_flow(eff_t effp, const st_sample_t *ibuf, st_sample_t *obuf, 
                 st_size_t *isamp, st_size_t *osamp)
{
        int len, done;
        
        st_sample_t l;
        st_sample_t tri16;      /* 16 signed bits of triangular noise */

        len = ((*isamp > *osamp) ? *osamp : *isamp);
        switch (effp->outinfo.encoding) {
                case ST_ENCODING_ULAW:
                case ST_ENCODING_ALAW:
                        for(done = 0; done < len; done++) {
                                tri16 = 
                                  ((rand()%32768L) + (rand()%32768L)) - 32767;

                                l = *ibuf++ + tri16*16*HALFABIT;  /* 2^4.5 */
                                *obuf++ = l;
                        }
                        break;
                default:
                switch (effp->outinfo.size) {
                        case ST_SIZE_BYTE:
                        for(done = 0; done < len; done++) {
                                tri16 = 
                                  ((rand()%32768L) + (rand()%32768L)) - 32767;

                                l = *ibuf++ + tri16*256*HALFABIT;  /* 2^8.5 */
                                *obuf++ = l;
                        }
                        break;
                        case ST_SIZE_WORD:
                        for(done = 0; done < len; done++) {
                                tri16 = 
                                  ((rand()%32768L) + (rand()%32768L)) - 32767;

                                l = *ibuf++ + tri16*HALFABIT;  /* 2^.5 */
                                *obuf++ = l;
                        }
                        break;
                        default: /* Mask (dither) not needed at >= 24 bits */
                        for(done = 0; done < len; done++) {
                                *obuf++ = *ibuf++;
                        }
                        break;
                }
        }

        *isamp = done;
        *osamp = done;
        return (ST_SUCCESS);
}

static st_effect_t st_mask_effect = {
  "mask",
  "Usage: Mask effect takes no options",
  ST_EFF_MCHAN,
  st_mask_getopts,
  st_effect_nothing,
  st_mask_flow,
  st_effect_nothing_drain,
  st_effect_nothing
};

const st_effect_t *st_mask_effect_fn(void)
{
    return &st_mask_effect;
}
