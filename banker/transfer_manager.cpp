#include "transfer_manager.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

uint32_t TransferManager::m_transferNumber = 0;

void Transfer::CreateTransferUID( void )
{
	int fd = open( "/dev/urandom", O_RDONLY );

	if ( fd == -1 )
	{
		srand( time(NULL) ^ getpid() ^ 0xc3e78948 );

		m_transferUID = rand()<<32|rand();
		
		return;
	}

	if ( read( fd, &m_transferUID, 8 ) != 8 )
	{
		srand( time(NULL) ^ getpid() ^ 0xc326ef83 );

		m_transferUID = rand()<<32|rand();
		
		return;
	}

	close( fd );
}

TransferManager::TransferManager( )
{

}

TransferManager::~TransferManager( )
{

}

void TransferManager::Commit( void )
{
	char szTransferFileName[512];
	sprintf( szTransferFileName, "/tmp/transfer_%d_%d.tsf", m_transferNumber, time(NULL) );

	FILE *pTransferFile = fopen( szTransferFileName, "w" );

	if ( !pTransferFile )
		return;

	for ( Transfer *pCur = (Transfer *)m_transferList.GetFirst(); pCur; pCur = (Transfer *)m_transferList.GetNext( pCur ) )
	{
		fprintf( pTransferFile, "%llu,%s,%s,%s,%s,%d.%d\n", pCur->GetTransferUID(), pCur->GetName().CStr(), pCur->GetBankUsername().CStr(), pCur->GetFromAccount().CStr(), pCur->GetToAccount().CStr(), pCur->GetAmountDollars(), pCur->GetAmountCents() );
	}

	fclose( pTransferFile );
 
	m_transferNumber++;
}

void TransferManager::AddTransfer( String sBankUsername, String sName, String sFromAccount, String sToAccount, uint32_t amountDollars, uint32_t amountCents )
{
	m_transferList.AddLast( new Transfer( sBankUsername, sName, sFromAccount, sToAccount, amountDollars, amountCents ) );
}

CDoubleList *TransferManager::GetTransferList( void )
{
	return &m_transferList;
}

Transfer *TransferManager::FindTransferByName( const String &sName )
{
	for ( Transfer *pCur = (Transfer *)m_transferList.GetFirst(); pCur; pCur = (Transfer *)m_transferList.GetNext( pCur ) )
	{
		if ( sName == pCur->GetName() )
			return pCur; 
	}

	return (NULL);
}
