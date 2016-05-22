#ifndef __DSP_ENGINE_H__
#define __DSP_ENGINE_H__

class CDSPEngine
{
public:
	static double BandpassFilter1200( double inputSample );
	static double BandpassFilter2200( double inputSample  );

	static double EnvelopeDetector1( double inputSample );
	static double EnvelopeDetector2( double inputSample );

private:
};

#endif // __DSP_ENGINE_H__
