#ifndef __TRANSFER_MANAGER_H__
#define __TRANSFER_MANAGER_H__

#include "doublelist.h"
#include "string.h"

class Transfer : public CDoubleLink
{
public: 
	Transfer( String sBankUsername, String sName, String fromAccount, String toAccount, uint32_t amountDollars, uint32_t amountCents )
		: m_sBankUsername( sBankUsername ), m_sName( sName ), m_fromAccount( fromAccount ), m_toAccount( toAccount ), m_amountDollars( amountDollars ), m_amountCents( amountCents )
	{
		CreateTransferUID();
	}

	~Transfer( )
	{

	}

	void CreateTransferUID( void );

	uint64_t GetTransferUID( void ) const { return m_transferUID; };
	const String &GetName( void ) const { return m_sName; };
	const String &GetBankUsername( void ) const { return m_sBankUsername; };
	const String &GetFromAccount( void ) const { return m_fromAccount; };
	const String &GetToAccount( void ) const { return m_toAccount; };
	uint32_t GetAmountDollars( void ) const { return m_amountDollars; };
	uint32_t GetAmountCents( void ) const { return m_amountCents; };

private:
	uint64_t m_transferUID;
	String m_sBankUsername;
	String m_sName;
	String m_fromAccount;
	String m_toAccount;
	uint32_t m_amountDollars;
	uint32_t m_amountCents;
	
};

class TransferManager
{
public:
	TransferManager( );
	~TransferManager( );

	void Commit( void );
	
	void AddTransfer( String sBankUsername, String sName, String sFromAccount, String sToAccount, uint32_t amountDollars, uint32_t amountCents );

	CDoubleList *GetTransferList( void );

	Transfer *FindTransferByName( const String &sName );

private:
	static uint32_t m_transferNumber;
	CDoubleList m_transferList;
};

#endif // TRANSFER_MANAGER_H__
