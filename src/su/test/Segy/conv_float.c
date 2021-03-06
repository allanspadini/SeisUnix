/* CONV_FLOAT: $Revision: 1.3 $ ; $Date: 92/06/15 13:03:40 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@keller.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"

/* conv_float - convert between 32 bit IBM and IEEE floating numbers
 *
 * Credits:
 *	CWP: Brian
 *
 * Parameters:
 *    from	- input vector
 *    to	- output vector, can be same as input vector
 *    len	- number of floats in vectors
 *    type	- conversion type
 *
 * Notes:
 *	Up to 3 bits lost on IEEE -> IBM
 *
 *	IBM -> IEEE may overflow or underflow, taken care of by 
 *	substituting large number or zero
 *
 *	Only integer shifting and masking are used.
 *
 *	These routines assume a big-endian machine.  If yours is little
 *	endian you will need to reverse the bytes in ibm_to_float
 *	with something like
 *
 *	fconv = from[i];
 *	fconv = (fconv<<24) | ((fconv>>24)&0xff) |
 *		((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);
 *
 *	and the operation in float_to_ibm.
 *
 */


/* Assumes sizeof(int) == 4 */
void ibm_to_float(int from[], int to[], int n)
{
    register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
	fconv = from[i];
	if (fconv) {
            fmant = 0x00ffffff & fconv;
            t = (int) ((0x7f000000 & fconv) >> 22) - 130;
            while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }
            if (t > 254) fconv = (0x80000000 & fconv) | 0x7f7fffff;
            else if (t <= 0) fconv = 0;
            else fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
        }
	to[i] = fconv;
    }
    return;
}

void float_to_ibm(int from[], int to[], int n)
{
    register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
	fconv = from[i];
	if (fconv) {
            fmant = (0x007fffff & fconv) | 0x00800000;
            t = (int) ((0x7f800000 & fconv) >> 23) - 126;
            while (t & 0x3) { ++t; fmant >>= 1; }
            fconv = (0x80000000 & fconv) | (((t>>2) + 64) << 24) | fmant;
        }
	to[i] = fconv;
    }
    return;
}

#if defined(TEST)
main()
{
    float f;
    int i;

    f = 1.0; i = 0;
    float_to_ibm(&f,&i,1);
    printf("f = %f -> 0x%x\n",f,i);

    i = 0x41100000; f = 0.0;
    ibm_to_float(&i,&f,1);
    printf("i = 0x%x -> f = %f\n",i,f);
    exit(0);
}
#endif


