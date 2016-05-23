#include "common.h"


/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bp -o 3 -a 7.9545454545e-02 1.0227272727e-01 -l */
double CDSPEngine::BandpassFilter1200( double inputSample )
{ 
	const uint32_t NZEROS = 6;
	const uint32_t NPOLES = 6;
	const double GAIN = 3.153274693e+03;

	static double xv[NZEROS+1]; // = { 0, 0, 0, 0, 0, 0, 0 };
	static double yv[NPOLES+1]; // = { 0, 0, 0, 0, 0, 0, 0 };

	xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = inputSample / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[6] - xv[0]) + 3 * (xv[2] - xv[4])
                     + ( -0.7513803887 * yv[0]) + (  3.9831348988 * yv[1])
                     + ( -9.5148931309 * yv[2]) + ( 12.9033387450 * yv[3])
                     + (-10.4665710440 * yv[4]) + (  4.8197406818 * yv[5]);
        
	return yv[6];
  }

/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bp -o 3 -a 1.5530303030e-01 1.7803030303e-01 -l */
double CDSPEngine::BandpassFilter2200( double inputSample  )
{
	const uint32_t NZEROS = 6;
	const uint32_t NPOLES = 6;
	const double GAIN = 3.153274737e+03;

	static double xv[NZEROS+1]; // = { 0, 0, 0, 0, 0, 0, 0 };
	static double yv[NPOLES+1]; // = { 0, 0, 0, 0, 0, 0, 0 };

	xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = inputSample / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[6] - xv[0]) + 3 * (xv[2] - xv[4])
                     + ( -0.7513803886 * yv[0]) + (  2.3673807854 * yv[1])
                     + ( -4.9576982251 * yv[2]) + (  6.0699177002 * yv[3])
                     + ( -5.4530346641 * yv[4]) + (  2.8646183902 * yv[5]);
        return yv[6];
  }



double CDSPEngine::EnvelopeDetector1( double inputSample )
{
	// square signal and low pass filter to remove high frequency copy
	// Low pass filter set to baud rate (600 baud)
	/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 2 -a 4.5454545455e-02 0.0000000000e+00 -l */

	const uint32_t NZEROS = 2;
	const uint32_t NPOLES = 2;
	const double GAIN = 5.921022146e+01;

	static double xv[NZEROS+1]; // = { 0, 0, 0 };
	static double yv[NPOLES+1]; // = { 0, 0, 0 };

	xv[0] = xv[1]; xv[1] = xv[2]; 
        xv[2] = ((double)inputSample * (double)inputSample) / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; 
        yv[2] =   (xv[0] + xv[2]) + 2 * xv[1]
                     + ( -0.6677576562 * yv[0]) + (  1.6002017528 * yv[1]);
        
	return yv[2];
}

double CDSPEngine::EnvelopeDetector2( double inputSample )
{
	// square signal and low pass filter to remove high frequency copy
	// Low pass filter set to baud rate (600 baud)
	/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 2 -a 4.5454545455e-02 0.0000000000e+00 -l */

	const uint32_t NZEROS = 2;
	const uint32_t NPOLES = 2;
	const double GAIN = 5.921022146e+01;

	static double xv[NZEROS+1]; // = { 0, 0, 0 };
	static double yv[NPOLES+1]; // = { 0, 0, 0 };

	xv[0] = xv[1]; xv[1] = xv[2]; 
        xv[2] = ((double)inputSample * (double)inputSample) / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; 
        yv[2] =   (xv[0] + xv[2]) + 2 * xv[1]
                     + ( -0.6677576562 * yv[0]) + (  1.6002017528 * yv[1]);
        
	return yv[2];
}
