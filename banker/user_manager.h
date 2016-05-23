#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__

#define ACCESS_USER	0
#define ACCESS_ADMIN	1

#include "string.h"
#include "doublelist.h"

class User : public CDoubleLink
{
public:
	User( String sUsername, String sPassword, uint8_t accessType )
		: m_sUsername( sUsername ), m_sPassword( sPassword ), m_accessType( accessType )
	{

	}

	const String &GetUsername( void ) const { return m_sUsername; };
	const String &GetPassword( void ) const { return m_sPassword; };

	uint8_t GetAccessType( void ) const { return m_accessType; };

	bool IsAdmin( void ) const { return m_accessType == ACCESS_ADMIN; };

private:
	String m_sUsername;
	String m_sPassword;
	uint8_t m_accessType;
};

class UserManager
{
public:
	UserManager();
	~UserManager();

	void LoadUserTable( void );
	void SaveUserTable( void );
	
	void AddUser( String sUsername, String sPassword, uint8_t accessType );
	bool DeleteUser( String sUsername );
	User *GetUser( String sUsername );

	CDoubleList *GetUserList( void );

private:
	CDoubleList m_userList;
};

#endif // __USER_MANAGER_H__
