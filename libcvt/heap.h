#ifndef MXHEAP_INCLUDED
#define MXHEAP_INCLUDED

/************************************************************************

Heap
Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

************************************************************************/

#include <iostream>

/*******************************************************/
// stdmix.h

#ifndef _STDMIX_H_
#define _STDMIX_H_

#include <cassert>

#if SAFETY > 0
#  define AssertBound(t)  assert(t)
#  define PRECAUTION(x) x
#else
#  define AssertBound(t)
#  define PRECAUTION(x)
#endif

#endif

/*******************************************************/

#define SanityCheck(t)  assert(t)

typedef unsigned int uint;

enum MxAllocPolicy { MX_ALLOC_INPLACE };

//#define MIN(x,y) ((x)<(y))?(x):(y)

inline int MIN(int n, int m){return (n < m) ? n : m;}

inline void *operator new(size_t, void *spot, MxAllocPolicy)
{
	return spot;  // MX_ALLOC_INPLACE
}

//
// Default equality class
//
class MxEQ {
public: bool operator==(const MxEQ& x) const { return this==&x; }
};

template<class T>
class MxBlock : public MxEQ
{
private:
	uint N;
	T *block;

protected:
	MxBlock() { }
	void init_block(uint n)
	{
		// Allocate memory for block
		N=n; block = (T *)malloc(sizeof(T)*n);

		// Initialize each entry
		for(uint i=0; i<n; i++) new((void *)&block[i], MX_ALLOC_INPLACE) T;
	}
	void resize_block(uint n)
	{
		T *old = block;

		// Allocate new block
		block = (T *)realloc(old, sizeof(T)*n);

		// Initialize all the newly allocated entries
		for(uint i=N; i<n; i++) new((void *)&block[i], MX_ALLOC_INPLACE) T;

		N = n;
	}
	void free_block()
	{
#ifdef WIN32
		// The Microsoft compiler has a problem with the
		// standard syntax below.  For some reason,
		// expanding it into the following pointer-based
		// version makes it work.  Don't ask me why.
		//
		for(uint i=0; i<N; i++)  { T *p = &block[i]; p->~T(); }
		free(block);
#else
		// Call the relevant destructors for each element before
		// freeing the block.  Has now effect for types like 'int'.
		//
		for(uint i=0; i<N; i++)  block[i].~T();
		free(block);
#endif
	}

	T&       raw(uint i)       { return block[i]; }
	const T& raw(uint i) const { return block[i]; }


public:
	MxBlock(uint n) { init_block(n); }
	~MxBlock() { free_block(); }

	// Standard accessors
	//
	T&       ref(uint i)              { AssertBound(i<N);return block[i]; }
	const T& ref(uint i) const        { AssertBound(i<N);return block[i]; }
	T&       operator()(uint i)       { return ref(i);   }
	const T& operator()(uint i) const { return ref(i);   }
#ifdef __GNUC__
	// Most compilers can apply the casting operators and then use the
	// standard array [].  GCC (2.7.2) seems to need explicit operators.
	//
	T&       operator[](uint i)       { return block[i]; }
	const T& operator[](uint i) const { return block[i]; }
#endif

	operator const T*() const { return block; }
	operator       T*()       { return block; }
	uint length() const { return N; }

	// Primitive methods for altering the data block
	//
	void resize(uint n) { resize_block(n); }
	void bitcopy(const T *array, uint n) // copy bits directly
	{ memcpy(block, array, MIN(n,N)*sizeof(T)); }
	void copy(const T *array, const uint n) // copy using assignment operator
	{ for(uint i=0; i<MIN(n,N); i++) block[i] = array[i]; }
	void bitcopy(const MxBlock<T>& b) { bitcopy(b, b.length()); }
	void copy(const MxBlock<T>& b) { copy(b, b.length()); }
};

template<class T>
class MxDynBlock : public MxBlock<T>
{
private:
	uint fill;

public:
	MxDynBlock() {MxBlock<T>::init_block(8); fill=0; }
	MxDynBlock(uint n) {MxBlock<T>::init_block(n); fill=0; }

	T&       operator()(uint i)       { AssertBound(i<fill); return MxBlock<T>::ref(i); }
	const T& operator()(uint i) const { AssertBound(i<fill); return MxBlock<T>::ref(i); }

	uint length() const { return fill; }
	uint total_space() const { return MxBlock<T>::length(); }
	T& last() { AssertBound(fill>0); return MxBlock<T>::raw(fill-1); }
	const T& last() const { AssertBound(fill>0); return MxBlock<T>::raw(fill-1); }

	uint add()
	{
		if( length()==total_space() ) resize(total_space() * 2);
		return fill++;
	}
	uint add(const T& t)
	{
		uint i=add();
		MxBlock<T>::raw(i) = t;
		return i;
	}

	void reset() { fill = 0; }
	T& drop() { return MxBlock<T>::raw(--fill); }
	void drop(uint d) { fill -= d; }
	bool find(const T& t, uint *index=NULL)
	{
		for(uint i=0; i<fill; i++)
		if( MxBlock<T>::raw(i) == t )
		{
			if( index ) *index = i;
			return true;
		}
		return false;
	}
	void remove(uint i)
	{ AssertBound(i<fill); fill--; MxBlock<T>::raw(i) = MxBlock<T>::raw(fill); }
	void remove_inorder(uint i)
	{ memmove(&MxBlock<T>::raw(i), &MxBlock<T>::raw(i+1), (--fill - i)*sizeof(T)); }
};

template<class T, int T_SIZE>
class MxSizedDynBlock : public MxDynBlock<T>
{
public:   MxSizedDynBlock(uint n=T_SIZE) : MxDynBlock<T>(n) { }
};

class MxHeapable
{
private:
	float import;
	int token;

public:
	MxHeapable() { not_in_heap(); heap_key(0.0f); }

	inline bool is_in_heap() { return token != -47; }
	inline void not_in_heap() { token = -47; }
	inline int get_heap_pos() { return token; }
	inline void set_heap_pos(int t) { token=t; }

	inline void  heap_key(float k) { import=k; }
	inline float heap_key() const  { return import; }
};


class MxHeap : private MxDynBlock<MxHeapable *>
{
private:
	void place(MxHeapable *x, unsigned int i);
	void swap(unsigned int i, unsigned int j);

	unsigned int parent(unsigned int i) { return (i-1)/2; }
	unsigned int left(unsigned int i) { return 2*i+1; }
	unsigned int right(unsigned int i) { return 2*i+2; }

	void upheap(unsigned int i);
	void downheap(unsigned int i);

public:
	MxHeap() : MxDynBlock<MxHeapable *>(8) { }
	MxHeap(unsigned int n) : MxDynBlock<MxHeapable *>(n) { }

	void insert(MxHeapable *t) { insert(t, t->heap_key()); }
	void insert(MxHeapable *, float);
	void update(MxHeapable *t) { update(t, t->heap_key()); }
	void update(MxHeapable *, float);

	unsigned int size() const { return length(); }
	MxHeapable       *item(uint i)       { return ref(i); }
	const MxHeapable *item(uint i) const { return ref(i); }
	MxHeapable *extract();
	MxHeapable *top() { return (length()<1 ? (MxHeapable *)NULL : raw(0)); }
	MxHeapable *remove(MxHeapable *);
};

////////////////////////////////////////////////////////////////////////
//
// Internal functions for manipulating the heap
//

inline void MxHeap::place(MxHeapable *x, unsigned int i)
{
	ref(i) = x;
	x->set_heap_pos(i);
}

void MxHeap::swap(unsigned int i, unsigned int j)
{
	MxHeapable *tmp = ref(i);

	place(ref(j), i);
	place(tmp, j);
}

void MxHeap::upheap(unsigned int i)
{
	MxHeapable *moving = ref(i);
	uint index = i;
	uint p = parent(i);

	while( index>0 && moving->heap_key() > ref(p)->heap_key() )
	{
		place(ref(p), index);
		index = p;
		p = parent(p);
	}

	if( index != i )
	place(moving, index);
}

void MxHeap::downheap(unsigned int i)
{
	MxHeapable *moving = ref(i);
	uint index = i;
	uint l = left(i);
	uint r = right(i);
	uint largest;

	while( l<length() )
	{
		if( r<length() && ref(l)->heap_key() < ref(r)->heap_key() )
		largest = r;
		else 
		largest = l;

		if( moving->heap_key() < ref(largest)->heap_key() )
		{
			place(ref(largest), index);
			index = largest;
			l = left(index);
			r = right(index);
		}
		else
		break;
	}

	if( index != i )
	place(moving, index);
}


/* implementation */

void MxHeap::insert(MxHeapable *t, float v)
{
	if( t->is_in_heap() ) return;

	t->heap_key(v);

	unsigned int i = add(t);
	t->set_heap_pos(i);

	upheap(i);
}

void MxHeap::update(MxHeapable *t, float v)
{
	SanityCheck( t->is_in_heap() );
	t->heap_key(v);

	unsigned int i = t->get_heap_pos();

	if( i>0 && v>ref(parent(i))->heap_key() )
	upheap(i);
	else
	downheap(i);
}

MxHeapable *MxHeap::extract()
{
	if( length() < 1 ) return NULL;

	swap(0, length()-1);
	MxHeapable *dead=drop();

	downheap(0);
	dead->not_in_heap();
	return dead;
}

MxHeapable *MxHeap::remove(MxHeapable *t)
{
	if( !t->is_in_heap() ) return NULL;

	int i = t->get_heap_pos();
	swap(i, length()-1);
	drop();
	t->not_in_heap();

	if( ref(i)->heap_key() < t->heap_key() )
	downheap(i);
	else
	upheap(i);

	return t;
}

// MXHEAP_INCLUDED
#endif
