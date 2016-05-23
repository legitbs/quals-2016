#ifndef __DOUBLE_LIST_H__
#define __DOUBLE_LIST_H__

// Forward declaration
class CDoubleList;

// A link item (carried by an item in a double list)
class CDoubleLink
{
public:
	friend class CDoubleList; // Allow double list to access private data members

public:
	CDoubleLink();
	~CDoubleLink();

	bool Unlink( void );	// Unlink it from its list

private:
	void AddToList( CDoubleList *pList ) { m_pList = pList; };
	void ClearList( void ) { m_pList = NULL; };

private:
	CDoubleLink *m_pNext;	// Next node
	CDoubleLink *m_pPrev;	// Previous node
	CDoubleList *m_pList;	// The list this item is in
};

// Doubly linked list class
class CDoubleList
{
public:
	friend class CDoubleLink;	// Allow double link to access RemoveLink function

public:
	CDoubleList();
	~CDoubleList();

	// Delete everyone
	void DeleteAll( void );

	// Accessors
	CDoubleLink *GetFirst( void ) { return m_pFirst; };
	CDoubleLink *GetLast( void ) { return m_pLast; };

	CDoubleLink *GetNext( CDoubleLink *pCur );
	CDoubleLink *GetPrev( CDoubleLink *pCur );

	// Remove
	CDoubleLink *RemoveFirst( void );
	CDoubleLink *RemoveLast( void );

	// Addition
	bool AddFirst( CDoubleLink *pItem );
	bool AddAfter( CDoubleLink *pPrev, CDoubleLink *pItem );
	bool AddLast( CDoubleLink *pItem );

	// Item count
	uint32_t GetItemCount( void ) { return m_itemCount; };

private:
	bool RemoveLink( CDoubleLink *pItem );

private:
	uint32_t m_itemCount;
	CDoubleLink *m_pFirst;
	CDoubleLink *m_pLast;
};

#endif // __DOUBLE_LIST_H__