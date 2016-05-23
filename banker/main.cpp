#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#include "cpp_helper.h"
#include "string.h"
#include "input_reader.h"
#include "user_manager.h"
#include "transfer_manager.h"
#include "settings.h"

#define BAD_USERNAME_ERROR	(0)

void DeleteTransferMenu( CInputReader *pInputReader, TransferManager *pTransfers, User *pUser )
{
	printf( "Enter Transfer Name to Delete: " );
	String sTransferName = pInputReader->ReadLine( 512 );

	Transfer *pDeleteTransfer = pTransfers->FindTransferByName( sTransferName );

	if ( pDeleteTransfer )
	{
		printf( "Deleting transfer: %s\n", pDeleteTransfer->GetName().CStr() );

		delete pDeleteTransfer;
		return;
	}

	printf( "Could not find transfer: %s\n", sTransferName.CStr() );
}

void ViewTransferMenu( CInputReader *pInputReader, TransferManager *pTransfers, User *pUser )
{
	printf( "Viewing Transfers:\n" );
	for ( Transfer *pCur = (Transfer *)pTransfers->GetTransferList()->GetFirst(); pCur; pCur = (Transfer *)pTransfers->GetTransferList()->GetNext( pCur ) )
	{
		printf( "Transfer: %s\n", pCur->GetName().CStr() );
		printf( "UID: %llu\n", pCur->GetTransferUID() );
		printf( "Bank User: %s\n", pCur->GetBankUsername().CStr() );
		printf( "From Account: %09s  ToAccount: %09s  Amount: $%u.%u\n", pCur->GetFromAccount().CStr(), pCur->GetToAccount().CStr(), pCur->GetAmountDollars(), pCur->GetAmountCents() );
	}
	
	printf( "\n" );
}

void NewTransferMenu( CInputReader *pInputReader, TransferManager *pTransfers, User *pUser )
{
	String sTransferName;
	String sFromAccount;
	String sToAccount;
	uint32_t amountDollars;
	uint32_t amountCents;

	printf( "Setup New Transfer:\n" );

	do
	{
		printf( "Enter Transfer Name: " );
		sTransferName = pInputReader->ReadLine( 512 );
		
		bool bInvalid = false;
		for ( uint32_t i = 0; i < strlen(sTransferName.CStr()); i++ )
		{
			if ( !isalnum(sTransferName.CStr()[i]) )
			{
				printf( "Invalid transfer name, only alpha numeric allowed\n" );
				bInvalid = true;
				break;
			}
		}

		if ( bInvalid )
			continue;

		if ( pTransfers->FindTransferByName( sTransferName ) )
		{
			printf( "A transfer by that name already exists!\n" );
			continue;
		}

		break;
	} while ( true);

	do
	{
		printf( "Enter From Account: " );
		sFromAccount = pInputReader->ReadLine( 128 );

		if ( sFromAccount.GetLength() != 9 )
		{
			printf( "Invalid account number, must be 9 digits\n" );
			continue;
		}

		// Check for all digits
		bool bInvalid = false;
		for ( uint32_t i = 0; i < strlen(sFromAccount.CStr()); i++ )
		{
			if ( !isdigit(sFromAccount.CStr()[i]) )
			{
				printf( "Invalid account number, must be 9 digits\n" );
				bInvalid = true;
				break;
			}
		}

		if ( !bInvalid )
			break;

	} while (true);
	
	do
	{
		printf( "Enter To Account: " );
		sToAccount = pInputReader->ReadLine( 128 );

		if ( sToAccount.GetLength() != 9 )
		{
			printf( "Invalid account number, must be 9 digits\n" );
			continue;
		}

		// Check for all digits
		bool bInvalid = false;
		for ( uint32_t i = 0; i < strlen(sToAccount.CStr()); i++ )
		{
			if ( !isdigit(sToAccount.CStr()[i]) )
			{
				printf( "Invalid account number, must be 9 digits\n" );
				bInvalid = true;
				break;
			}
		}

		if ( !bInvalid )
			break;

	} while (true);

	do
	{
		printf( "Enter dollar amount and cents to transfer (ex: 100.00): " );
		String sTransferAmount = pInputReader->ReadLine( 128 );

		// Check format
		if ( sscanf( sTransferAmount.CStr(), "%u.%02u", &amountDollars, &amountCents ) != 2 )
		{
			printf( "Invalid format, re-enter.\n" );
			continue;
		}

		if ( amountCents >= 100 )
		{
			printf( "Invalid format, re-enter.\n" );
			continue;
		}

		if ( amountDollars > MAX_TRANSFER_AMOUNT )
		{
			printf( "Transfer amount limit exceed, re-enter.\n" );
			continue;
		}

		break;	
	} while (true);

	pTransfers->AddTransfer( pUser->GetUsername(), sTransferName, sFromAccount, sToAccount, amountDollars, amountCents );
}

void AdminCreateUser( CInputReader *pInputReader, UserManager *pUserManager )
{
	String sUsername;
	String sPassword;

	do
	{
		printf( "Enter New Username: " );

		sUsername = pInputReader->ReadLine( 256 );

		if ( sUsername.GetLength() > MAX_USERNAME_LEN )
		{
			printf( "Invalid username, must be %d characters or less.\n", MAX_USERNAME_LEN );
			continue;
		}

		bool bBadCharacter = false;
		for ( uint32_t i = 0; i < sUsername.GetLength(); i++ )
		{
			if ( !isalnum( sUsername.CStr()[i] ) )
			{
				printf( "Invalid character %c in username, only alphanumeric is accepted.\n", sUsername.CStr()[i] );
				bBadCharacter = true;
				break;
			}
		}

		if ( bBadCharacter )
			continue;

		if ( pUserManager->GetUser( sUsername ) )
		{
			printf( "Invalid username, %s already exists.\n", sUsername.CStr() );
			continue;
		}

		break;
	} while ( true );
	
	do
	{
		printf( "Enter New Password: " );

		sPassword = pInputReader->ReadLine( 256 );

		if ( sPassword.GetLength() < MIN_PASSWORD_LEN )
		{
			printf( "Invalid password, must be a minimum of %d characters.\n", MIN_PASSWORD_LEN );
			continue;
		}

		bool bBadCharacter = false;
		for ( uint32_t i = 0; i < sPassword.GetLength(); i++ )
		{
			if ( !isalnum( sPassword.CStr()[i] ) )
			{
				printf( "Invalid character %c in password, only alphanumeric is accepted.\n", sPassword.CStr()[i] );
				break;
			}
		}

		if ( bBadCharacter )
			continue;

		break;
	} while ( true );

	printf( "User %s added.\n", sUsername.CStr() );

	pUserManager->AddUser( sUsername, sPassword, ACCESS_USER );
}

void AdminViewUsers( CInputReader *pInputReader, UserManager *pUserManager )
{
	printf( "Viewing active users:\n" );
	
	for ( User *pCur = (User *)pUserManager->GetUserList()->GetFirst(); pCur; pCur = (User *)pUserManager->GetUserList()->GetNext( pCur ) )
	{
		if ( pCur->IsAdmin() )
			printf( "%8s : ADMIN\n", pCur->GetUsername().CStr() );
		else
			printf( "%8s : USER\n", pCur->GetUsername().CStr() );
	}
}

void AdminDeleteUser( CInputReader *pInputReader, UserManager *pUserManager )
{
	printf( "Enter Username to delete: " );
	String sUsername = pInputReader->ReadLine( 128 );

	User *pToDeleteUser = pUserManager->GetUser( sUsername );

	if ( !pToDeleteUser )
	{
		printf( "User %s not found!\n", sUsername.CStr() );
		return;
	}

	if ( pToDeleteUser->IsAdmin() )
	{
		printf( "Cannot delete an admin user!\n" );
		return;
	}

	if ( pUserManager->DeleteUser( sUsername ) )
	{
		printf( "User %s deleted.\n", sUsername.CStr() );
		return;
	}
	else
	{
		printf( "Critical error, failed to delete user!\n" );
	}

}

void AdminConsoleMenu( CInputReader *pInputReader, UserManager *pUserManager, User *pUser )
{
	if ( !pUser->IsAdmin() )
	{
		printf( "Not an admin!\n" );
		return;
	}

	bool bExit = false;
	do
	{
		printf( "Admin Commands:\n" );
		printf( "1) Create New User\n" );
		printf( "2) View Users\n" );
		printf( "3) Delete User\n" );
		printf( "4) Exit\n" );

		String sSelection = pInputReader->ReadLine( 2 );

		int32_t selectionNumber = sSelection.ToInt();

		switch( selectionNumber )
		{
		case 1:
			// Do Admin Create User
			AdminCreateUser( pInputReader, pUserManager );
			break;

		case 2:
			// Do Admin View Users
			AdminViewUsers( pInputReader, pUserManager );
			break;
	
		case 3:
			// Do Admin Delete User
			AdminDeleteUser( pInputReader, pUserManager );
			break;

		case 4:
			bExit = true;
			break;
		
		default:
			printf( "Invalid selection.\n" );
			break;

		}
	} while ( !bExit );

	printf( "Exiting.\n" );
}

void RunUserConsole( CInputReader *pInputReader, User *pUser, UserManager *pUserManager )
{
	TransferManager oPendingTransfers;
	bool bCommitTransfers = false;

	bool bExit = false;
	do
	{
		printf( "Commands:\n" );
		printf( "1) New Transfer\n" );
		printf( "2) View Pending Transfers\n" );
		printf( "3) Delete Transfer\n" );
		printf( "4) Logout and Commit Transfers\n" );
		printf( "5) Logout and DO NOT commit Transfers\n" );
		if ( pUser->IsAdmin() )
		{
			printf( "6) Admin Console\n" );
		}

		String sSelection = pInputReader->ReadLine( 2 );

		int32_t selectionNumber = sSelection.ToInt();

		switch( selectionNumber )
		{
		case 1:
			NewTransferMenu( pInputReader, &oPendingTransfers, pUser );
			break;

		case 2:
			// View pending transfers
			ViewTransferMenu( pInputReader, &oPendingTransfers, pUser );
			break;

		case 3:
			// Delete Transfer
			DeleteTransferMenu( pInputReader, &oPendingTransfers, pUser );
			break;

		case 4:
			// Logout and Commit Transfers
			printf( "Logging out and commiting.\n" );
			bExit = true;
			bCommitTransfers = true;
			break;

		case 5:
			// Logout and do not commit transfers
			printf( "Logging out and not commiting\n" );
			bExit = true;
			bCommitTransfers = false;
			break;

		case 6:
			if ( pUser->IsAdmin() )
			{
				AdminConsoleMenu( pInputReader, pUserManager, pUser );
			}
			else
				printf( "Invalid selection.\n" );

			break;

		default:
			printf( "Invalid selection.\n" );
			break;
		}	

	} while ( !bExit );

	if ( bCommitTransfers )
		oPendingTransfers.Commit();
	return;	
}

void sig_alarm_handler( int signum )
{
	printf( "Timeout! Disconnecting.\n" );
		
	exit( 1 );
}

int main( void )
{
	UserManager oUserManager;
	time_t curtime;
	struct tm *timeinfo;

	setbuf( stdout, NULL );

	signal( SIGALRM, sig_alarm_handler );
	alarm( MAX_IDLE_SECS );

	time( &curtime );
	timeinfo = localtime(&curtime);
	
	printf( "LegitBS Bank Terminal, Routing Number 766683678470\n" );
	printf( "Current UTC time is: %s\n", asctime(timeinfo) );

	uint32_t invalidTryCount = 0;
	bool bDone = false;
	do
	{
		oUserManager.LoadUserTable();

		printf( "Enter username: " );

		CInputReader oInputReader;

		String sUsername = oInputReader.ReadLine( MAX_USERNAME_LEN+1 );

		printf( "Enter password: " );
		String sPassword = oInputReader.ReadLine( MAX_PASSWORD_LEN+1 );

		// Find username from table
		User *pFindUser = oUserManager.GetUser( sUsername );

		if ( !pFindUser )
		{
			invalidTryCount++;

			if ( invalidTryCount > MAX_TRY_TILL_TIMEOUT )
			{
				printf( "Login being delayed due to too many failed attempts!\n" );
				sleep( 1 );
				
				invalidTryCount = 0;
			}
	
			printf( "Invalid username/password, error code=%d\n", BAD_USERNAME_ERROR );

			continue;
		}

		int8_t errorCode = pFindUser->GetPassword().Compare( sPassword );

		if ( errorCode != 0 )
		{
			invalidTryCount++;
			
			if ( invalidTryCount > MAX_TRY_TILL_TIMEOUT )
			{
				printf( "Login being delayed due to too many failed attempts!\n" );
				sleep( 1 );

				invalidTryCount = 0;
			}

			printf( "Invalid username/password, error code=%d\n", errorCode );
			continue;
		}

		printf( "Successfully logged in as: %s\n", pFindUser->GetUsername().CStr() );

		RunUserConsole( &oInputReader, pFindUser, &oUserManager );
		
	} while ( !bDone );

	return (0);
}
