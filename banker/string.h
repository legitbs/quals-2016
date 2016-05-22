#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdio.h>

class String
{
public:
        String();
        String(const String& rhs);
        String(const char* rhs);
        ~String();
        
	uint32_t GetLength(void) const;
        String SubString(uint32_t start, uint32_t length);
        
	void Reverse();
        bool IsEmpty();
        char GetAt(uint32_t pos);
        
	void ToUpper();
        void ToLower();

	int32_t ToInt( void ) const;


	String &operator=(const String &rhs); // Copy assignment
	//String &operator=(String &rhs); // Move assignment (take values)
	String operator+(String &rhs);
	String &operator+=(String &rhs);
	bool operator==(String &rhs) const;
	bool operator==(const String &rhs) const;
  
	int32_t Compare( const String &rhs ) const;
	const char *CStr(void) const; 

private: 
        char* m_pData;
        uint32_t m_size;
};

#endif // STRING_H
