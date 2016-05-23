#ifndef __COMPLEX_H__
#define __COMPLEX_H__

// A complex class
class CComplex
{
public:
	CComplex();
	CComplex( CComplex &rhs );
	CComplex( double real, double imag );
	~CComplex();

    CComplex operator+ ( CComplex& );
    CComplex operator- ( CComplex& );
    CComplex operator* ( CComplex& );
    CComplex operator/ ( CComplex& );
    CComplex operator/ ( double scale );

    CComplex Conjugate();
    double Normal();
    uint32_t Print( char *szDest, uint32_t maxLen );

private:
	double m_real;
	double m_imag;
};

#endif // __COMPLEX_H__