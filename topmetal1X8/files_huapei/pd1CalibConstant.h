#ifndef _PD1_CALIB_CONSTANT_H_
#define _PD1_CALIB_CONSTANT_H_

const int CalibOK = 1;
const int CalibFAIL = -1;

const int MAX_NCHIPS = 4096; // This is the number of ADC channels (chips)

const int PD1_DIMX = 72;
const int PD1_DIMY = 72;

const double time_per_frame = 1.0; // this means the time (in micro-second) of each frame. When we fit time \tau of each pixel, we shall average all ADC values of that pixel in that frame, and only count the timing difference between neighbouring frames.
 

#endif
