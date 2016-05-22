#include "common.h"

CComplex::CComplex( )
	: m_real( 0 ), m_imag( 0 )
{

}

CComplex::CComplex( CComplex &rhs )
	: m_real( rhs.m_real ), m_imag( rhs.m_imag )
{

}

CComplex::CComplex( double real, double imag )
	: m_real( real ), m_imag( imag )
{

}

CComplex::~CComplex( )
{
	// Do nothing
}

CComplex CComplex::operator+ ( CComplex &rhs )
{
	return CComplex( m_real + rhs.m_real, m_imag + rhs.m_imag );
}

CComplex CComplex::operator- ( CComplex &rhs )
{
	return CComplex( m_real - rhs.m_real, m_imag - rhs.m_imag );
}

CComplex CComplex::operator* ( CComplex &rhs )
{
	return CComplex( ((m_real * rhs.m_real) - (m_imag * rhs.m_imag)), ((m_real * rhs.m_imag) + (m_imag * rhs.m_real)) );
}


// To be completed in next class
CComplex CComplex::operator/ ( CComplex &rhs )
{
    CComplex conj = rhs.Conjugate();
    double norm = rhs.Normal();
    
	return ((*this) * conj) / (norm * norm);
}

CComplex CComplex::operator/ ( double scale )
{
    return CComplex( m_real / scale, m_imag / scale );
}

CComplex CComplex::Conjugate()
{
	// Conjugate vector
    return CComplex( m_real, -m_imag );
}

double CComplex::Normal()
{
	// Normal vector
    return sqrt( (m_real * m_real) + (m_imag * m_imag) );
}

uint32_t CComplex::Print( char *szDest, uint32_t maxLen )
{
	if ( szDest == NULL )
		return (0);

    return snprintf( szDest, maxLen, "(%f,%f)", m_real, m_imag );
}