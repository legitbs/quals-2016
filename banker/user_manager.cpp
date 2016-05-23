#include "user_manager.h"
#include "settings.h"
#include "base64.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

UserManager::UserManager( )
{

}

UserManager::~UserManager( )
{

}

bool GetUserDataFromFile( FILE *pFile, char *pUsername, char *pPassword, uint32_t *accessType )
{
	if ( !pFile )
		return (false);

	char szLine[1024];
	char szPasswordTemp[1024];
	char szAccessTypeTemp[1024];

	if ( fgets( szLine, 1024, pFile ) == NULL )
		return (false);

	uint32_t lineLength = strlen(szLine);
	uint32_t itemIdx = 0;
	uint32_t itemPos = 0;
	for ( uint32_t i = 0; i < lineLength; i++ )
	{
		if ( itemIdx == 3 )
			break;

		if ( szLine[i] == ' ' )
		{
			
			switch ( itemIdx )
			{
			case 0:
				pUsername[itemPos] = '\0';
				break;

			case 1:
				szPasswordTemp[itemPos] = '\0';
				break;

			case 2:
				szAccessTypeTemp[itemPos++] = '\0';
				break;
			}

			itemIdx++;
			itemPos = 0;
			continue;
		}

		if ( szLine[i] == '\n' )
		{
			itemIdx++;
			break;
		}

		switch ( itemIdx )
		{
		case 0:
			if ( itemPos > MAX_USERNAME_LEN )
				return (false);

			pUsername[itemPos++] = szLine[i];
			break;

		case 1:
			szPasswordTemp[itemPos++] = szLine[i];
			break;

		case 2:
			szAccessTypeTemp[itemPos++] = szLine[i];
			break;	
		}	
	}

	if ( itemIdx != 3 )
		return (false);

	szAccessTypeTemp[itemPos] = '\0';
	(*accessType) = atoi( szAccessTypeTemp );

	// Base 64 decode password (don't check length)
	size_t out_len;
	uint8_t *pDecodedPassword = base64_decode( (const uint8_t*)szPasswordTemp, strlen(szPasswordTemp), &out_len );	

	memcpy( pPassword, pDecodedPassword, out_len );
	pPassword[out_len] = '\0';

	delete pDecodedPassword;

	if ( out_len > MAX_PASSWORD_LEN )
		return (false);

	return true;
}

void UserManager::LoadUserTable( void )
{
	char szPassword[MAX_PASSWORD_LEN+1];
	char szUsername[MAX_USERNAME_LEN+1];
	uint32_t accessType;

	FILE *pUserFile = fopen( USER_FILE, "r" );

	if ( !pUserFile )
	{
		printf( "Unable to open users file: %s\n", USER_FILE );
		exit(-1);
	}

	// Reset user list
	m_userList.DeleteAll();
	
	while ( !feof(pUserFile) )
	{
		if ( !GetUserDataFromFile( pUserFile, szUsername, szPassword, &accessType ) )
			return;

		AddUser( String(szUsername), String(szPassword), accessType );	
	}
	
	fclose( pUserFile );
}

void UserManager::SaveUserTable( void )
{
	FILE *pUserFile = fopen( USER_FILE, "w" );

	if ( !pUserFile )
	{
		printf( "Unable to open users file for writing: %s\n", USER_FILE );
		exit(-1);
	}	

	for ( User *pCur = (User *)m_userList.GetFirst(); pCur; pCur = (User *)m_userList.GetNext( pCur ) )
	{
		size_t out_len;
		uint8_t *pEncodedPassword = base64_encode( (uint8_t*)pCur->GetPassword().CStr(), pCur->GetPassword().GetLength(), NULL );

		
		fprintf( pUserFile, "%s %s %d\n", pCur->GetUsername().CStr(), pEncodedPassword, pCur->GetAccessType() );

		delete pEncodedPassword;	
	}

	fclose( pUserFile );
}

void UserManager::AddUser( String sUsername, String sPassword, uint8_t accessType )
{
	m_userList.AddLast( new User( sUsername, sPassword, accessType ) );

	SaveUserTable();
}

bool UserManager::DeleteUser( String sUsername )
{
	// Find user
	bool bUserDeleted = false;
	for ( User *pCur = (User *)m_userList.GetFirst(); pCur; pCur = (User *)m_userList.GetNext( pCur ) )
	{
		if ( pCur->GetUsername() == sUsername )
		{
			delete pCur;

			bUserDeleted = true;
			break;
		}
	}

	if ( bUserDeleted )
	{
		SaveUserTable();
		return (true);
	}

	return (false);
}

User *UserManager::GetUser( String sUsername )
{
	for ( User *pCur = (User *)m_userList.GetFirst(); pCur; pCur = (User *)m_userList.GetNext( pCur ) )
	{
		if ( pCur->GetUsername() == sUsername )
		{
			return (pCur);
		}
	}

	return (NULL);
}

CDoubleList *UserManager::GetUserList( void )
{
	return &m_userList;
}
