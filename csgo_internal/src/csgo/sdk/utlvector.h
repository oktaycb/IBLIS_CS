#pragma once

#ifndef UTLVECTOR_H
#define UTLVECTOR_H

#ifdef _WIN32
#pragma once
#endif

#define  Assert( _exp )										((void)0)
#define MEM_ALLOC_CREDIT_CLASS()

#include <string.h>
#include <malloc.h>
#include "utlmemory.h"
#include "utlblockmemory.h"
#include "platform.h"
#include "threadtools.h"

#define FOR_EACH_VEC( vecName, iteratorName ) \
	for ( int iteratorName = 0; iteratorName < (vecName).Count(); iteratorName++ )
#define FOR_EACH_VEC_BACK( vecName, iteratorName ) \
	for ( int iteratorName = (vecName).Count()-1; iteratorName >= 0; iteratorName-- )

//-----------------------------------------------------------------------------
// The CUtlVector class:
// A growable array class which doubles in size by default.
// It will always keep all elements consecutive in memory, and may move the
// elements around in memory (via a PvRealloc) when elements are inserted or
// removed. Clients should therefore refer to the elements of the vector
// by index (they should *never* maintain pointers to elements in the vector).
//-----------------------------------------------------------------------------
template< class T, class A = CUtlMemory<T> >
class CUtlVector
{
	typedef A CAllocator;
public:
	typedef T ElemType_t;

	// constructor, destructor
	CUtlVector(int growSize = 0, int initSize = 0);
	CUtlVector(T* pMemory, int allocationCount, int numElements = 0);
	~CUtlVector();

	// Copy the array.
	CUtlVector<T, A>& operator=(const CUtlVector<T, A>& other);

	// element access
	T& operator[](int i);
	const T& operator[](int i) const;
	T& Element(int i);
	const T& Element(int i) const;
	T& Head();
	const T& Head() const;
	T& Tail();
	const T& Tail() const;

	// Gets the base address (can change when adding elements!)
	T* Base() { return m_Memory.Base(); }
	const T* Base() const { return m_Memory.Base(); }

	// Returns the number of elements in the vector
	int Count() const;

	// Is element index valid?
	bool IsValidIndex(int i) const;
	static int InvalidIndex();

	// Adds an element, uses default constructor
	int AddToHead();
	int AddToTail();
	int InsertBefore(int elem);
	int InsertAfter(int elem);

	// Adds an element, uses copy constructor
	int AddToHead(const T& src);
	int AddToTail(const T& src);
	int InsertBefore(int elem, const T& src);
	int InsertAfter(int elem, const T& src);

	// Adds multiple elements, uses default constructor
	int AddMultipleToHead(int num);
	int AddMultipleToTail(int num);
	int AddMultipleToTail(int num, const T* pToCopy);
	int InsertMultipleBefore(int elem, int num);
	int InsertMultipleBefore(int elem, int num, const T* pToCopy);
	int InsertMultipleAfter(int elem, int num);

	// Calls RemoveAll() then AddMultipleToTail.
	void SetSize(int size);
	void SetSizeDirect(int size); //DYLAN ADDED DUE TO NOV 18, 2019 UPDATE, DO NOT USE THIS UNDER ANY CIRCUMSTANCE
	void SetCount(int count);
	void SetCountNonDestructively(int count); //sets count by adding or removing elements to tail TODO: This should probably be the default behavior for SetCount

	// Calls SetSize and copies each element.
	void CopyArray(const T* pArray, int size);

	// Fast swap
	void Swap(CUtlVector< T, A >& vec);

	// Add the specified array to the tail.
	int AddVectorToTail(CUtlVector<T, A> const& src);

	// Finds an element (element needs operator== defined)
	int Find(const T& src) const;
	void FillWithValue(const T& src);

	bool HasElement(const T& src) const;

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity(int num);

	// Makes sure we have at least this many elements
	void EnsureCount(int num);

	// Element removal
	void FastRemove(int elem);	// doesn't preserve order
	void Remove(int elem);		// preserves order, shifts elements
	bool FindAndRemove(const T& src);	// removes first occurrence of src, preserves order, shifts elements
	bool FindAndFastRemove(const T& src);	// removes first occurrence of src, doesn't preserve order
	void RemoveMultiple(int elem, int num);	// preserves order, shifts elements
	void RemoveMultipleFromHead(int num); // removes num elements from tail
	void RemoveMultipleFromTail(int num); // removes num elements from tail
	void RemoveAll();				// doesn't deallocate memory

	// Memory deallocation
	void Purge();

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements();

	// Compacts the vector to the number of elements actually in use 
	void Compact();

	// Set the size by which it grows when it needs to allocate more memory.
	void SetGrowSize(int size) { m_Memory.SetGrowSize(size); }

	int NumAllocated() const;	// Only use this if you really know what you're doing!

	void Sort(int(__cdecl* pfnCompare)(const T*, const T*));

#ifdef DBGFLAG_VALIDATE
	void Validate(CValidator& validator, char* pchName);		// Validate our internal structures
#endif // DBGFLAG_VALIDATE

//protected:
	// Can't copy this unless we explicitly do it!
	CUtlVector(CUtlVector const& vec) { /*Assert(0);*/ }

	// Grows the vector
	void GrowVector(int num = 1);

	// Shifts elements....
	void ShiftElementsRight(int elem, int num = 1);
	void ShiftElementsLeft(int elem, int num = 1);

	CAllocator m_Memory;
	int m_Size;

#ifndef _X360
	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T* m_pElements;

	inline void ResetDbgInfo()
	{
		m_pElements = Base();
	}
#else
	inline void ResetDbgInfo() {}
#endif
};


// this is kind of ugly, but until C++ gets templatized typedefs in C++0x, it's our only choice
template < class T >
class CUtlBlockVector : public CUtlVector< T, CUtlBlockMemory< T, int > >
{
public:
	CUtlBlockVector(int growSize = 0, int initSize = 0)
		: CUtlVector< T, CUtlBlockMemory< T, int > >(growSize, initSize) {}
};

//-----------------------------------------------------------------------------
// The CUtlVectorMT class:
// A array class with some sort of mutex protection. Not sure which operations are protected from
// which others.
//-----------------------------------------------------------------------------

template< class BASE_UTLVECTOR, class MUTEX_TYPE = CThreadFastMutex >
class CUtlVectorMT : public BASE_UTLVECTOR, public MUTEX_TYPE
{
	typedef BASE_UTLVECTOR BaseClass;
public:
	MUTEX_TYPE Mutex_t;

	// constructor, destructor
	CUtlVectorMT(int growSize = 0, int initSize = 0) : BaseClass(growSize, initSize) {}
	CUtlVectorMT(typename BaseClass::ElemType_t* pMemory, int numElements) : BaseClass(pMemory, numElements) {}
};


//-----------------------------------------------------------------------------
// The CUtlVectorFixed class:
// A array class with a fixed allocation scheme
//-----------------------------------------------------------------------------
template< class T, size_t MAX_SIZE >
class CUtlVectorFixed : public CUtlVector< T, CUtlMemoryFixed<T, MAX_SIZE > >
{
	typedef CUtlVector< T, CUtlMemoryFixed<T, MAX_SIZE > > BaseClass;
public:

	// constructor, destructor
	CUtlVectorFixed(int growSize = 0, int initSize = 0) : BaseClass(growSize, initSize) {}
	CUtlVectorFixed(T* pMemory, int numElements) : BaseClass(pMemory, numElements) {}
};


//-----------------------------------------------------------------------------
// The CUtlVectorFixedGrowable class:
// A array class with a fixed allocation scheme backed by a dynamic one
//-----------------------------------------------------------------------------
template< class T, size_t MAX_SIZE >
class CUtlVectorFixedGrowable : public CUtlVector< T, CUtlMemoryFixedGrowable<T, MAX_SIZE > >
{
	typedef CUtlVector< T, CUtlMemoryFixedGrowable<T, MAX_SIZE > > BaseClass;

public:
	// constructor, destructor
	CUtlVectorFixedGrowable(int growSize = 0) : BaseClass(growSize, MAX_SIZE) {}
};


//-----------------------------------------------------------------------------
// The CUtlVectorConservative class:
// A array class with a conservative allocation scheme
//-----------------------------------------------------------------------------
template< class T >
class CUtlVectorConservative : public CUtlVector< T, CUtlMemoryConservative<T> >
{
	typedef CUtlVector< T, CUtlMemoryConservative<T> > BaseClass;
public:

	// constructor, destructor
	CUtlVectorConservative(int growSize = 0, int initSize = 0) : BaseClass(growSize, initSize) {}
	CUtlVectorConservative(T* pMemory, int numElements) : BaseClass(pMemory, numElements) {}
};


//-----------------------------------------------------------------------------
// The CUtlVectorUltra Conservative class:
// A array class with a very conservative allocation scheme, with customizable allocator
// Especialy useful if you have a lot of vectors that are sparse, or if you're
// carefully packing holders of vectors
//-----------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable : 4200) // warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable : 4815 ) // warning C4815: 'staticData' : zero-sized array in stack object will have no elements

class CUtlVectorUltraConservativeAllocator
{
public:
	static void* Alloc(size_t nSize)
	{
		return malloc(nSize);
	}

	static void* Realloc(void* pMem, size_t nSize)
	{
		return realloc(pMem, nSize);
	}

	static void Free(void* pMem)
	{
		free(pMem);
	}

	static size_t GetSize(void* pMem)
	{
		return _msize(pMem);
	}

};

template <typename T, typename A = CUtlVectorUltraConservativeAllocator >
class CUtlVectorUltraConservative : private A
{
public:
	CUtlVectorUltraConservative()
	{
		m_pData = StaticData();
	}

	~CUtlVectorUltraConservative()
	{
		RemoveAll();
	}

	int Count() const
	{
		return m_pData->m_Size;
	}

	static int InvalidIndex()
	{
		return -1;
	}

	inline bool IsValidIndex(int i) const
	{
		return (i >= 0) && (i < Count());
	}

	T& operator[](int i)
	{
		//Assert(IsValidIndex(i));
		return m_pData->m_Elements[i];
	}

	const T& operator[](int i) const
	{
		//Assert(IsValidIndex(i));
		return m_pData->m_Elements[i];
	}

	T& Element(int i)
	{
		//Assert(IsValidIndex(i));
		return m_pData->m_Elements[i];
	}

	const T& Element(int i) const
	{
		//Assert(IsValidIndex(i));
		return m_pData->m_Elements[i];
	}

	void EnsureCapacity(int num)
	{
		int nCurCount = Count();
		if (num <= nCurCount)
		{
			return;
		}
		if (m_pData == StaticData())
		{
			m_pData = (Data_t*)A::Alloc(sizeof(int) + (num * sizeof(T)));
			m_pData->m_Size = 0;
		}
		else
		{
			int nNeeded = sizeof(int) + (num * sizeof(T));
			int nHave = A::GetSize(m_pData);
			if (nNeeded > nHave)
			{
				m_pData = (Data_t*)A::Realloc(m_pData, nNeeded);
			}
		}
	}

	int AddToTail(const T& src)
	{
		int iNew = Count();
		EnsureCapacity(Count() + 1);
		m_pData->m_Elements[iNew] = src;
		m_pData->m_Size++;
		return iNew;
	}

	void RemoveAll()
	{
		if (Count())
		{
			for (int i = m_pData->m_Size; --i >= 0; )
			{
				Destruct(&m_pData->m_Elements[i]);
			}
		}
		if (m_pData != StaticData())
		{
			A::Free(m_pData);
			m_pData = StaticData();

		}
	}

	void PurgeAndDeleteElements()
	{
		if (m_pData != StaticData())
		{
			for (int i = 0; i < m_pData->m_Size; i++)
			{
				delete Element(i);
			}
			RemoveAll();
		}
	}

	void FastRemove(int elem)
	{
		//Assert(IsValidIndex(elem));

		Destruct(&Element(elem));
		if (Count() > 0)
		{
			if (elem != m_pData->m_Size - 1)
				memcpy(&Element(elem), &Element(m_pData->m_Size - 1), sizeof(T));
			--m_pData->m_Size;
		}
		if (!m_pData->m_Size)
		{
			A::Free(m_pData);
			m_pData = StaticData();
		}
	}

	void Remove(int elem)
	{
		Destruct(&Element(elem));
		ShiftElementsLeft(elem);
		--m_pData->m_Size;
		if (!m_pData->m_Size)
		{
			A::Free(m_pData);
			m_pData = StaticData();
		}
	}

	int Find(const T& src) const
	{
		int nCount = Count();
		for (int i = 0; i < nCount; ++i)
		{
			if (Element(i) == src)
				return i;
		}
		return -1;
	}

	bool FindAndRemove(const T& src)
	{
		int elem = Find(src);
		if (elem != -1)
		{
			Remove(elem);
			return true;
		}
		return false;
	}


	bool FindAndFastRemove(const T& src)
	{
		int elem = Find(src);
		if (elem != -1)
		{
			FastRemove(elem);
			return true;
		}
		return false;
	}

	struct Data_t
	{
		int m_Size;
		T m_Elements[];
	};

	Data_t* m_pData;
private:
	void ShiftElementsLeft(int elem, int num = 1)
	{
		int Size = Count();
		//Assert(IsValidIndex(elem) || (Size == 0) || (num == 0));
		int numToMove = Size - elem - num;
		if ((numToMove > 0) && (num > 0))
		{
			memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));

#ifdef _DEBUG
			memset(&Element(Size - num), 0xDD, num * sizeof(T));
#endif
		}
	}



	static Data_t* StaticData()
	{
		static Data_t staticData;
		//Assert(staticData.m_Size == 0);
		return &staticData;
	}
};

#pragma warning(pop)


//-----------------------------------------------------------------------------
// The CCopyableUtlVector class:
// A array class that allows copy construction (so you can nest a CUtlVector inside of another one of our containers)
//  WARNING - this class lets you copy construct which can be an expensive operation if you don't carefully control when it happens
// Only use this when nesting a CUtlVector() inside of another one of our container classes (i.e a CUtlMap)
//-----------------------------------------------------------------------------
template< class T >
class CCopyableUtlVector : public CUtlVector< T, CUtlMemory<T> >
{
	typedef CUtlVector< T, CUtlMemory<T> > BaseClass;
public:
	CCopyableUtlVector(int growSize = 0, int initSize = 0) : BaseClass(growSize, initSize) {}
	CCopyableUtlVector(T* pMemory, int numElements) : BaseClass(pMemory, numElements) {}
	virtual ~CCopyableUtlVector() {}
	CCopyableUtlVector(CCopyableUtlVector const& vec) { CopyArray(vec.Base(), vec.Count()); }
};

// TODO (Ilya): It seems like all the functions in CUtlVector are simple enough that they should be inlined.

//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline CUtlVector<T, A>::CUtlVector(int growSize, int initSize) :
	m_Memory(growSize, initSize), m_Size(0)
{
	ResetDbgInfo();
}

template< typename T, class A >
inline CUtlVector<T, A>::CUtlVector(T* pMemory, int allocationCount, int numElements) :
	m_Memory(pMemory, allocationCount), m_Size(numElements)
{
	ResetDbgInfo();
}

template< typename T, class A >
inline CUtlVector<T, A>::~CUtlVector()
{
	Purge();
}

template< typename T, class A >
inline CUtlVector<T, A>& CUtlVector<T, A>::operator=(const CUtlVector<T, A>& other)
{
	int nCount = other.Count();
	SetSize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		(*this)[i] = other[i];
	}
	return *this;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< typename T, class A >
inline T& CUtlVector<T, A>::operator[](int i)
{
	//Assert(i < m_Size);
	return m_Memory[i];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::operator[](int i) const
{
	//Assert(i < m_Size);
	return m_Memory[i];
}

template< typename T, class A >
inline T& CUtlVector<T, A>::Element(int i)
{
	//Assert(i < m_Size);
	return m_Memory[i];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::Element(int i) const
{
	//Assert(i < m_Size);
	return m_Memory[i];
}

template< typename T, class A >
inline T& CUtlVector<T, A>::Head()
{
	//Assert(m_Size > 0);
	return m_Memory[0];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::Head() const
{
	//Assert(m_Size > 0);
	return m_Memory[0];
}

template< typename T, class A >
inline T& CUtlVector<T, A>::Tail()
{
	//Assert(m_Size > 0);
	return m_Memory[m_Size - 1];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::Tail() const
{
	//Assert(m_Size > 0);
	return m_Memory[m_Size - 1];
}


//-----------------------------------------------------------------------------
// Count
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::Count() const
{
	return m_Size;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< typename T, class A >
inline bool CUtlVector<T, A>::IsValidIndex(int i) const
{
	return (i >= 0) && (i < m_Size);
}


//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::InvalidIndex()
{
	return -1;
}


//-----------------------------------------------------------------------------
// Grows the vector
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::GrowVector(int num)
{
	if (m_Size + num > m_Memory.NumAllocated())
	{
		MEM_ALLOC_CREDIT_CLASS();
		m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());
	}

	m_Size += num;
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Sorts the vector
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::Sort(int(__cdecl* pfnCompare)(const T*, const T*))
{
	typedef int(__cdecl* QSortCompareFunc_t)(const void*, const void*);
	if (Count() <= 1)
		return;

	if (Base())
	{
		qsort(Base(), Count(), sizeof(T), (QSortCompareFunc_t)(pfnCompare));
	}
	else
	{
		//Assert(0);
		// this path is untested
		// if you want to sort vectors that use a non-sequential memory allocator,
		// you'll probably want to patch in a quicksort algorithm here
		// I just threw in this bubble sort to have something just in case...

		for (int i = m_Size - 1; i >= 0; --i)
		{
			for (int j = 1; j <= i; ++j)
			{
				if (pfnCompare(&Element(j - 1), &Element(j)) < 0)
				{
					V_swap(Element(j - 1), Element(j));
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::EnsureCapacity(int num)
{
	MEM_ALLOC_CREDIT_CLASS();
	m_Memory.EnsureCapacity(num);
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Makes sure we have at least this many elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::EnsureCount(int num)
{
	if (Count() < num)
	{
		AddMultipleToTail(num - Count());
	}
}


//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::ShiftElementsRight(int elem, int num)
{
	//Assert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
		memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
}

template< typename T, class A >
void CUtlVector<T, A>::ShiftElementsLeft(int elem, int num)
{
	//Assert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
	{
		memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));

#ifdef _DEBUG
		//memmove(&Element(m_Size - num), 0xDD, num * sizeof(T));
#endif
	}
}


//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::AddToHead()
{
	return InsertBefore(0);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddToTail()
{
	return InsertBefore(m_Size);
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertAfter(int elem)
{
	return InsertBefore(elem + 1);
}

template< typename T, class A >
int CUtlVector<T, A>::InsertBefore(int elem)
{
	// Can insert at the end
	//Assert((elem == Count()) || IsValidIndex(elem));

	GrowVector();
	ShiftElementsRight(elem);
	Construct(&Element(elem));
	return elem;
}


//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::AddToHead(const T& src)
{
	// Can't insert something that's in the list... reallocation may hose us
	//Assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
	return InsertBefore(0, src);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddToTail(const T& src)
{
	// Can't insert something that's in the list... reallocation may hose us
	//Assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
	return InsertBefore(m_Size, src);
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertAfter(int elem, const T& src)
{
	// Can't insert something that's in the list... reallocation may hose us
	//Assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
	return InsertBefore(elem + 1, src);
}

template< typename T, class A >
int CUtlVector<T, A>::InsertBefore(int elem, const T& src)
{
	// Can't insert something that's in the list... reallocation may hose us
	//Assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));

	// Can insert at the end
	//Assert((elem == Count()) || IsValidIndex(elem));

	GrowVector();
	ShiftElementsRight(elem);
	CopyConstruct(&Element(elem), src);
	return elem;
}


//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::AddMultipleToHead(int num)
{
	return InsertMultipleBefore(0, num);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddMultipleToTail(int num)
{
	return InsertMultipleBefore(m_Size, num);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddMultipleToTail(int num, const T* pToCopy)
{
	// Can't insert something that's in the list... reallocation may hose us
	//Assert((Base() == NULL) || !pToCopy || (pToCopy + num <= Base()) || (pToCopy >= (Base() + Count())));

	return InsertMultipleBefore(m_Size, num, pToCopy);
}

template< typename T, class A >
int CUtlVector<T, A>::InsertMultipleAfter(int elem, int num)
{
	return InsertMultipleBefore(elem + 1, num);
}


template< typename T, class A >
void CUtlVector<T, A>::SetCount(int count)
{
	RemoveAll();
	AddMultipleToTail(count);
}

template< typename T, class A >
inline void CUtlVector<T, A>::SetSize(int size)
{
	SetCount(size);
}

template< typename T, class A >
inline void CUtlVector<T, A>::SetSizeDirect(int size)
{
	m_Size = 0;
}

template< typename T, class A >
void CUtlVector<T, A>::SetCountNonDestructively(int count)
{
	int delta = count - m_Size;
	if (delta > 0) AddMultipleToTail(delta);
	else if (delta < 0) RemoveMultipleFromTail(-delta);
}

template< typename T, class A >
void CUtlVector<T, A>::CopyArray(const T* pArray, int size)
{
	// Can't insert something that's in the list... reallocation may hose us
	//Assert((Base() == NULL) || !pArray || (Base() >= (pArray + size)) || (pArray >= (Base() + Count())));

	SetSize(size);
	for (int i = 0; i < size; i++)
	{
		(*this)[i] = pArray[i];
	}
}

template< typename T, class A >
void CUtlVector<T, A>::Swap(CUtlVector< T, A >& vec)
{
	m_Memory.Swap(vec.m_Memory);
	V_swap(m_Size, vec.m_Size);
#ifndef _X360
	V_swap(m_pElements, vec.m_pElements);
#endif
}

template< typename T, class A >
int CUtlVector<T, A>::AddVectorToTail(CUtlVector const& src)
{
	//Assert(&src != this);

	int base = Count();

	// Make space.
	int nSrcCount = src.Count();
	EnsureCapacity(base + nSrcCount);

	// Copy the elements.	
	m_Size += nSrcCount;
	for (int i = 0; i < nSrcCount; i++)
	{
		CopyConstruct(&Element(base + i), src[i]);
	}
	return base;
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertMultipleBefore(int elem, int num)
{
	if (num == 0)
		return elem;

	// Can insert at the end
	//Assert((elem == Count()) || IsValidIndex(elem));

	GrowVector(num);
	ShiftElementsRight(elem, num);

	// Invoke default constructors
	for (int i = 0; i < num; ++i)
	{
		Construct(&Element(elem + i));
	}

	return elem;
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertMultipleBefore(int elem, int num, const T* pToInsert)
{
	if (num == 0)
		return elem;

	// Can insert at the end
	//Assert((elem == Count()) || IsValidIndex(elem));

	GrowVector(num);
	ShiftElementsRight(elem, num);

	// Invoke default constructors
	if (!pToInsert)
	{
		for (int i = 0; i < num; ++i)
		{
			Construct(&Element(elem + i));
		}
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			CopyConstruct(&Element(elem + i), pToInsert[i]);
		}
	}

	return elem;
}


//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template< typename T, class A >
int CUtlVector<T, A>::Find(const T& src) const
{
	for (int i = 0; i < Count(); ++i)
	{
		if (Element(i) == src)
			return i;
	}
	return -1;
}

template< typename T, class A >
void CUtlVector<T, A>::FillWithValue(const T& src)
{
	for (int i = 0; i < Count(); i++)
	{
		Element(i) = src;
	}
}

template< typename T, class A >
bool CUtlVector<T, A>::HasElement(const T& src) const
{
	return (Find(src) >= 0);
}


//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::FastRemove(int elem)
{
	//Assert(IsValidIndex(elem));

	Destruct(&Element(elem));
	if (m_Size > 0)
	{
		if (elem != m_Size - 1)
			memcpy(&Element(elem), &Element(m_Size - 1), sizeof(T));
		--m_Size;
	}
}

template< typename T, class A >
void CUtlVector<T, A>::Remove(int elem)
{
	Destruct(&Element(elem));
	ShiftElementsLeft(elem);
	--m_Size;
}

template< typename T, class A >
bool CUtlVector<T, A>::FindAndRemove(const T& src)
{
	int elem = Find(src);
	if (elem != -1)
	{
		Remove(elem);
		return true;
	}
	return false;
}

template< typename T, class A >
bool CUtlVector<T, A>::FindAndFastRemove(const T& src)
{
	int elem = Find(src);
	if (elem != -1)
	{
		FastRemove(elem);
		return true;
	}
	return false;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveMultiple(int elem, int num)
{
	//Assert(elem >= 0);
	//Assert(elem + num <= Count());

	for (int i = elem + num; --i >= elem; )
		Destruct(&Element(i));

	ShiftElementsLeft(elem, num);
	m_Size -= num;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveMultipleFromHead(int num)
{
	//Assert(num <= Count());

	for (int i = num; --i >= 0; )
		Destruct(&Element(i));

	ShiftElementsLeft(0, num);
	m_Size -= num;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveMultipleFromTail(int num)
{
	//Assert(num <= Count());

	for (int i = m_Size - num; i < m_Size; i++)
		Destruct(&Element(i));

	m_Size -= num;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveAll()
{
	for (int i = m_Size; --i >= 0; )
	{
		Destruct(&Element(i));
	}

	m_Size = 0;
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------

template< typename T, class A >
inline void CUtlVector<T, A>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
	ResetDbgInfo();
}


template< typename T, class A >
inline void CUtlVector<T, A>::PurgeAndDeleteElements()
{
	for (int i = 0; i < m_Size; i++)
	{
		delete Element(i);
	}
	Purge();
}

template< typename T, class A >
inline void CUtlVector<T, A>::Compact()
{
	m_Memory.Purge(m_Size);
}

template< typename T, class A >
inline int CUtlVector<T, A>::NumAllocated() const
{
	return m_Memory.NumAllocated();
}


//-----------------------------------------------------------------------------
// Data and memory validation
//-----------------------------------------------------------------------------
#ifdef DBGFLAG_VALIDATE
template< typename T, class A >
void CUtlVector<T, A>::Validate(CValidator& validator, char* pchName)
{
	validator.Push(typeid(*this).name(), this, pchName);

	m_Memory.Validate(validator, "m_Memory");

	validator.Pop();
}
#endif // DBGFLAG_VALIDATE

// A vector class for storing pointers, so that the elements pointed to by the pointers are deleted
// on exit.
template<class T> class CUtlVectorAutoPurge : public CUtlVector< T, CUtlMemory< T, int> >
{
public:
	~CUtlVectorAutoPurge(void)
	{
		this->PurgeAndDeleteElements();
	}

};

// easy string list class with dynamically allocated strings. For use with V_SplitString, etc.
// Frees the dynamic strings in destructor.
class CUtlStringList : public CUtlVectorAutoPurge< char*>
{
public:
	void CopyAndAddToTail(char const* pString)			// clone the string and add to the end
	{
		char* pNewStr = new char[1 + strlen(pString)];
		strcpy(pNewStr, pString);
		AddToTail(pNewStr);
	}

	static int __cdecl SortFunc(char* const* sz1, char* const* sz2)
	{
		return strcmp(*sz1, *sz2);
	}

};



// <Sergiy> placing it here a few days before Cert to minimize disruption to the rest of codebase
class CSplitString : public CUtlVector<char*, CUtlMemory<char*, int> >
{
public:
	CSplitString(const char* pString, const char* pSeparator);
	CSplitString(const char* pString, const char** pSeparators, int nSeparators);
	~CSplitString();
	//
	// NOTE: If you want to make Construct() public and implement Purge() here, you'll have to free m_szBuffer there
	//
private:
	void Construct(const char* pString, const char** pSeparators, int nSeparators);
	void PurgeAndDeleteElements();
private:
	char* m_szBuffer; // a copy of original string, with '\0' instead of separators
};


#endif // CCVECTOR_H


typedef unsigned short UtlSymId_t;

#define UTL_INVAL_SYMBOL  ((UtlSymId_t)~0)

//class CUtlSymbolTable;
class CUtlSymbol
{
public:
	// constructor, destructor
	CUtlSymbol() : m_Id(UTL_INVAL_SYMBOL) {}
	CUtlSymbol(UtlSymId_t id) : m_Id(id) {}
	CUtlSymbol(CUtlSymbol const& sym) : m_Id(sym.m_Id) {}

	// operator=
	CUtlSymbol& operator=(CUtlSymbol const& src) { m_Id = src.m_Id; return *this; }

	// operator==
	bool operator==(CUtlSymbol const& src) const { return m_Id == src.m_Id; }

	// Is valid?
	bool IsValid() const { return m_Id != UTL_INVAL_SYMBOL; }

	// Gets at the symbol
	operator UtlSymId_t const() const { return m_Id; }
protected:
	UtlSymId_t   m_Id;
protected:

	static bool s_bAllowStaticSymbolTable;
	friend class CCleanupUtlSymbolTable;
};

typedef unsigned int UtlHashHandle_t;

template<class Data, typename C = bool(*)(Data const&, Data const&), typename K = unsigned int(*)(Data const&) >
class CUtlHash
{
public:
	// compare and key functions - implemented by the 
	typedef C CompareFunc_t;
	typedef K KeyFunc_t;

	// constructor/deconstructor
	CUtlHash(int bucketCount = 0, int growCount = 0, int initCount = 0,
		CompareFunc_t compareFunc = 0, KeyFunc_t keyFunc = 0);
	~CUtlHash();

	// invalid handle
	static UtlHashHandle_t InvalidHandle(void) { return (UtlHashHandle_t)~0; }
	bool IsValidHandle(UtlHashHandle_t handle) const;

	// size
	int Count(void) const;

	// memory
	void Purge(void);

	// insertion methods
	UtlHashHandle_t Insert(Data const& src);
	UtlHashHandle_t Insert(Data const& src, bool* pDidInsert);
	UtlHashHandle_t AllocEntryFromKey(Data const& src);

	// removal methods
	void Remove(UtlHashHandle_t handle);
	void RemoveAll();

	// retrieval methods
	UtlHashHandle_t Find(Data const& src) const;

	Data& Element(UtlHashHandle_t handle);
	Data const& Element(UtlHashHandle_t handle) const;
	Data& operator[](UtlHashHandle_t handle);
	Data const& operator[](UtlHashHandle_t handle) const;

	UtlHashHandle_t GetFirstHandle() const;
	UtlHashHandle_t GetNextHandle(UtlHashHandle_t h) const;

	// debugging!!
	void Log(const char* filename);
	void Dump();

protected:

	int GetBucketIndex(UtlHashHandle_t handle) const;
	int GetKeyDataIndex(UtlHashHandle_t handle) const;
	UtlHashHandle_t BuildHandle(int ndxBucket, int ndxKeyData) const;

	bool DoFind(Data const& src, unsigned int* pBucket, int* pIndex) const;

protected:

	// handle upper 16 bits = bucket index (bucket heads)
	// handle lower 16 bits = key index (bucket list)
	typedef CUtlVector<Data> HashBucketList_t;
	CUtlVector<HashBucketList_t>	m_Buckets;

	CompareFunc_t					m_CompareFunc;			// function used to handle unique compares on data
	KeyFunc_t						m_KeyFunc;				// function used to generate the key value

	bool							m_bPowerOfTwo;			// if the bucket value is a power of two, 
	unsigned int					m_ModMask;				// use the mod mask to "mod"	
};

template <typename T>
inline bool IsPowerOfTwo(T value)
{
	return (value & (value - (T)1)) == (T)0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
CUtlHash<Data, C, K>::CUtlHash(int bucketCount, int growCount, int initCount,
	CompareFunc_t compareFunc, KeyFunc_t keyFunc) :
	m_CompareFunc(compareFunc),
	m_KeyFunc(keyFunc)
{
	bucketCount = std::min(bucketCount, 65536);
	m_Buckets.SetSize(bucketCount);
	for (int ndxBucket = 0; ndxBucket < bucketCount; ndxBucket++)
	{
		m_Buckets[ndxBucket].SetSize(initCount);
		m_Buckets[ndxBucket].SetGrowSize(growCount);
	}

	// check to see if the bucket count is a power of 2 and set up
	// optimizations appropriately
	m_bPowerOfTwo = IsPowerOfTwo(bucketCount);
	m_ModMask = m_bPowerOfTwo ? (bucketCount - 1) : 0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
CUtlHash<Data, C, K>::~CUtlHash()
{
	Purge();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline bool CUtlHash<Data, C, K>::IsValidHandle(UtlHashHandle_t handle) const
{
	int ndxBucket = GetBucketIndex(handle);
	int ndxKeyData = GetKeyDataIndex(handle);

	if ((ndxBucket < m_Buckets.Count()))
	{
		if ((ndxKeyData < m_Buckets[ndxBucket].Count()))
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline int CUtlHash<Data, C, K>::Count(void) const
{
	int count = 0;

	int bucketCount = m_Buckets.Count();
	for (int ndxBucket = 0; ndxBucket < bucketCount; ndxBucket++)
	{
		count += m_Buckets[ndxBucket].Count();
	}

	return count;
}

template<class Data, typename C, typename K>
inline int CUtlHash<Data, C, K>::GetBucketIndex(UtlHashHandle_t handle) const
{
	return (((handle >> 16) & 0x0000ffff));
}

template<class Data, typename C, typename K>
inline int CUtlHash<Data, C, K>::GetKeyDataIndex(UtlHashHandle_t handle) const
{
	return (handle & 0x0000ffff);
}

template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::BuildHandle(int ndxBucket, int ndxKeyData) const
{
	UtlHashHandle_t handle = ndxKeyData;
	handle |= (ndxBucket << 16);

	return handle;
}

template<class Data, typename C, typename K>
inline void CUtlHash<Data, C, K>::Purge(void)
{
	int bucketCount = m_Buckets.Count();
	for (int ndxBucket = 0; ndxBucket < bucketCount; ndxBucket++)
	{
		m_Buckets[ndxBucket].Purge();
	}
}

template<class Data, typename C, typename K>
inline bool CUtlHash<Data, C, K>::DoFind(Data const& src, unsigned int* pBucket, int* pIndex) const
{
	// generate the data "key"
	unsigned int key = m_KeyFunc(src);

	// hash the "key" - get the correct hash table "bucket"
	unsigned int ndxBucket;
	if (m_bPowerOfTwo)
	{
		*pBucket = ndxBucket = (key & m_ModMask);
	}
	else
	{
		int bucketCount = m_Buckets.Count();
		*pBucket = ndxBucket = key % bucketCount;
	}

	int ndxKeyData;
	const CUtlVector<Data>& bucket = m_Buckets[ndxBucket];
	int keyDataCount = bucket.Count();
	for (ndxKeyData = 0; ndxKeyData < keyDataCount; ndxKeyData++)
	{
		if (m_CompareFunc(bucket.Element(ndxKeyData), src))
			break;
	}

	if (ndxKeyData == keyDataCount)
		return false;

	*pIndex = ndxKeyData;
	return true;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::Find(Data const& src) const
{
	unsigned int ndxBucket;
	int ndxKeyData;

	if (DoFind(src, &ndxBucket, &ndxKeyData))
	{
		return (BuildHandle(ndxBucket, ndxKeyData));
	}
	return (InvalidHandle());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::Insert(Data const& src)
{
	unsigned int ndxBucket;
	int ndxKeyData;

	if (DoFind(src, &ndxBucket, &ndxKeyData))
	{
		return (BuildHandle(ndxBucket, ndxKeyData));
	}

	ndxKeyData = m_Buckets[ndxBucket].AddToTail(src);

	return (BuildHandle(ndxBucket, ndxKeyData));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::Insert(Data const& src, bool* pDidInsert)
{
	unsigned int ndxBucket;
	int ndxKeyData;

	if (DoFind(src, &ndxBucket, &ndxKeyData))
	{
		*pDidInsert = false;
		return (BuildHandle(ndxBucket, ndxKeyData));
	}

	*pDidInsert = true;
	ndxKeyData = m_Buckets[ndxBucket].AddToTail(src);

	return (BuildHandle(ndxBucket, ndxKeyData));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::AllocEntryFromKey(Data const& src)
{
	unsigned int ndxBucket;
	int ndxKeyData;

	if (DoFind(src, &ndxBucket, &ndxKeyData))
	{
		return (BuildHandle(ndxBucket, ndxKeyData));
	}

	ndxKeyData = m_Buckets[ndxBucket].AddToTail();

	return (BuildHandle(ndxBucket, ndxKeyData));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline void CUtlHash<Data, C, K>::Remove(UtlHashHandle_t handle)
{
	//Assert(IsValidHandle(handle));

	// check to see if the bucket exists
	int ndxBucket = GetBucketIndex(handle);
	int ndxKeyData = GetKeyDataIndex(handle);

	if (m_Buckets[ndxBucket].IsValidIndex(ndxKeyData))
	{
		m_Buckets[ndxBucket].FastRemove(ndxKeyData);
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline void CUtlHash<Data, C, K>::RemoveAll()
{
	int bucketCount = m_Buckets.Count();
	for (int ndxBucket = 0; ndxBucket < bucketCount; ndxBucket++)
	{
		m_Buckets[ndxBucket].RemoveAll();
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline Data& CUtlHash<Data, C, K>::Element(UtlHashHandle_t handle)
{
	int ndxBucket = GetBucketIndex(handle);
	int ndxKeyData = GetKeyDataIndex(handle);

	return (m_Buckets[ndxBucket].Element(ndxKeyData));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline Data const& CUtlHash<Data, C, K>::Element(UtlHashHandle_t handle) const
{
	int ndxBucket = GetBucketIndex(handle);
	int ndxKeyData = GetKeyDataIndex(handle);

	return (m_Buckets[ndxBucket].Element(ndxKeyData));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline Data& CUtlHash<Data, C, K>::operator[](UtlHashHandle_t handle)
{
	int ndxBucket = GetBucketIndex(handle);
	int ndxKeyData = GetKeyDataIndex(handle);

	return (m_Buckets[ndxBucket].Element(ndxKeyData));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline Data const& CUtlHash<Data, C, K>::operator[](UtlHashHandle_t handle) const
{
	int ndxBucket = GetBucketIndex(handle);
	int ndxKeyData = GetKeyDataIndex(handle);

	return (m_Buckets[ndxBucket].Element(ndxKeyData));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::GetFirstHandle() const
{
	return GetNextHandle((UtlHashHandle_t)-1);
}

template<class Data, typename C, typename K>
inline UtlHashHandle_t CUtlHash<Data, C, K>::GetNextHandle(UtlHashHandle_t handle) const
{
	++handle; // start at the first possible handle after the one given

	int bi = GetBucketIndex(handle);
	int ki = GetKeyDataIndex(handle);

	int nBuckets = m_Buckets.Count();
	for (; bi < nBuckets; ++bi)
	{
		if (ki < m_Buckets[bi].Count())
			return BuildHandle(bi, ki);

		ki = 0;
	}

	return InvalidHandle();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline void CUtlHash<Data, C, K>::Log(const char* filename)
{
	FILE* pDebugFp;
	pDebugFp = fopen(filename, "w");
	if (!pDebugFp)
		return;

	int maxBucketSize = 0;
	int numBucketsEmpty = 0;

	int bucketCount = m_Buckets.Count();
	fprintf(pDebugFp, "\n%d Buckets\n", bucketCount);

	for (int ndxBucket = 0; ndxBucket < bucketCount; ndxBucket++)
	{
		int count = m_Buckets[ndxBucket].Count();

		if (count > maxBucketSize) { maxBucketSize = count; }
		if (count == 0)
			numBucketsEmpty++;

		fprintf(pDebugFp, "Bucket %d: %d\n", ndxBucket, count);
	}

	fprintf(pDebugFp, "\nBucketHeads Used: %d\n", bucketCount - numBucketsEmpty);
	fprintf(pDebugFp, "Max Bucket Size: %d\n", maxBucketSize);

	fclose(pDebugFp);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<class Data, typename C, typename K>
inline void CUtlHash<Data, C, K>::Dump()
{
	int maxBucketSize = 0;
	int numBucketsEmpty = 0;

	int bucketCount = m_Buckets.Count();

	for (int ndxBucket = 0; ndxBucket < bucketCount; ndxBucket++)
	{
		int count = m_Buckets[ndxBucket].Count();

		if (count > maxBucketSize) { maxBucketSize = count; }
		if (count == 0)
			numBucketsEmpty++;
	}
}