#include "common.h"

CDoubleLink::CDoubleLink( )
	: m_pPrev( NULL ), m_pNext( NULL ), m_pList( NULL )
{

}

CDoubleLink::~CDoubleLink( )
{
	// On delete unlink ourselves (iterators beware!!!)
	Unlink();
}

bool CDoubleLink::Unlink( void )
{
	if ( m_pList )
		return m_pList->RemoveLink( this );

	return (false);
}

CDoubleList::CDoubleList( )
	: m_pFirst( NULL ), m_pLast( NULL ), m_itemCount( 0 )
{

}

CDoubleList::~CDoubleList( )
{
	// Remove everyone
	DeleteAll();
}

void CDoubleList::DeleteAll( void )
{
	// Delete all items in list
	CDoubleLink *pNext = NULL;
	CDoubleLink *pCur = NULL;

	for ( pCur = m_pFirst; pCur; pCur = pNext )
	{
		pNext = pCur->m_pNext;

		delete pCur;
	}

	m_itemCount = 0;
}

CDoubleLink *CDoubleList::GetNext( CDoubleLink *pCur )
{
	if ( !pCur )
		return (NULL);

	// Are we in this list!?!?!
	if ( pCur->m_pList != this )
		return (NULL);

	return (pCur->m_pNext);
}

CDoubleLink *CDoubleList::GetPrev( CDoubleLink *pCur )
{
	if ( !pCur )
		return (NULL);

	// Are we in this list!?!?!
	if ( pCur->m_pList != this )
		return (NULL);

	return (pCur->m_pPrev);
}

CDoubleLink *CDoubleList::RemoveFirst( void )
{
	CDoubleLink *pItem = m_pFirst;

	RemoveLink( pItem );

	return (pItem);
}

CDoubleLink *CDoubleList::RemoveLast( void )
{
	CDoubleLink *pItem = m_pLast;

	RemoveLink( pItem );

	return (pItem);
}

bool CDoubleList::AddFirst( CDoubleLink *pItem )
{
	if ( pItem == NULL || pItem->m_pList )
		return (false);	// In a list or NULL -- don't add!

	if ( m_pFirst == NULL )
	{
		// List empty!!!
		pItem->m_pNext = pItem->m_pPrev = NULL;

		m_pFirst = m_pLast = pItem;
	}
	else
	{
		pItem->m_pNext = m_pFirst;

		m_pFirst->m_pPrev = pItem;
		pItem->m_pPrev = NULL;

		m_pFirst = pItem;
	}

	// Remember our list
	pItem->AddToList( this );

	// Update item count
	m_itemCount++;

	return (true);
}

bool CDoubleList::AddAfter( CDoubleLink *pPrev, CDoubleLink *pItem )
{
	// If previous is NULL -- add to first
	if ( pPrev == NULL )
		return AddFirst( pItem );

	if ( pItem == NULL || pItem->m_pList )
		return (false);	// In a list or NULL -- don't add!

	if ( pPrev->m_pList != this )
		return (false);	// Previous is not our list!

	pItem->m_pNext = pPrev->m_pNext;
	pItem->m_pPrev = pPrev;

	if ( pPrev->m_pNext )
		pPrev->m_pNext->m_pPrev = pItem;
	
	pPrev->m_pNext = pItem;

	if ( m_pLast == pPrev )
		m_pLast = pItem;

	// Remember our list
	pItem->AddToList( this );

	// Update item count
	m_itemCount++;

	return (true);
}

bool CDoubleList::AddLast( CDoubleLink *pItem )
{
	if ( pItem == NULL || pItem->m_pList )
		return (false);	// In a list or NULL -- don't add!

	if ( m_pFirst == NULL )
	{
		// List empty!!!
		pItem->m_pNext = pItem->m_pPrev = NULL;

		m_pFirst = m_pLast = pItem;
	}
	else
	{
		pItem->m_pNext = NULL;
		pItem->m_pPrev = m_pLast;

		m_pLast->m_pNext = pItem;

		m_pLast = pItem;
	}

	// Remember our list
	pItem->AddToList( this );

	// Update item count
	m_itemCount++;

	return (true);
}

bool CDoubleList::RemoveLink( CDoubleLink *pItem )
{
	if ( pItem == NULL || pItem->m_pList != this )
		return (false);	// NULL or not in list

	if ( pItem->m_pNext )
		pItem->m_pNext->m_pPrev = pItem->m_pPrev;

	if ( pItem->m_pPrev )
		pItem->m_pPrev->m_pNext = pItem->m_pNext;

	if ( pItem == m_pLast )
		m_pLast = pItem->m_pPrev;

	if ( pItem == m_pFirst )
		m_pFirst = pItem->m_pNext;

	// Clear list that this item is in
	pItem->ClearList();

	// Update item count
	m_itemCount--;

	return (true);
}