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
        
	uint32_t GetLength(void);
        String SubString(uint32_t start, uint32_t length);
        
	void Reverse();
        bool IsEmpty();
        char GetAt(uint32_t pos);
        
	void ToUpper();
        void ToLower();


	String operator+(String &rhs);
	String &operator+=(String &rhs);
	bool operator==(String &rhs);
  
	int32_t Compare( String &rhs );
	const char *CStr(void) const; 

private: 
        char* m_pData;
        uint32_t m_size;
};

#endif // STRING_H
