

#ifndef __DSP_LIB__     
#define __DSP_LIB__

#include <stdlib.h>
#include <math.h>


#define Q15(X) \
   ((X < 0.0) ? (int)(32768*(X) - 0.5) : (int)(32767*(X) + 0.5))

typedef int             fractional;

extern fractional Float2Fract (float aVal);
extern float Fract2Float (fractional aVal);

typedef struct {
        fractional* abcCoefficients;    // A,B,C in X-space
        fractional* controlHistory;     // control history d-line in Y-space
        fractional controlOutput;       // PID Controller Output
        fractional measuredOutput;      // Measured Output sample
        fractional controlReference;    // Reference Input sample
} tPID;

extern void PIDCoeffCalc(               /* Kp, Ki, Kd->A,B,C */
        fractional* kCoeffs,
        tPID* controller               
);


extern void PIDInit (tPID* controller);
extern fractional* PID (tPID* controller);

#endif 
