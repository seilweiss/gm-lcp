/*
    _____               __  ___          __            ____        _      __
   / ___/__ ___ _  ___ /  |/  /__  ___  / /_____ __ __/ __/_______(_)__  / /_
  / (_ / _ `/  ' \/ -_) /|_/ / _ \/ _ \/  '_/ -_) // /\ \/ __/ __/ / _ \/ __/
  \___/\_,_/_/_/_/\__/_/  /_/\___/_//_/_/\_\\__/\_, /___/\__/_/ /_/ .__/\__/
                                               /___/             /_/
                                             
  See Copyright Notice in gmMachine.h

*/

#ifndef _GMARRAYCOMPLEX_H_
#define _GMARRAYCOMPLEX_H_

#include "gmConfig.h"
#include "gmUtil.h"
#include "gmMem.h"

#define TMPL template <class T>
#define QUAL gmArrayComplex<T>

/// \class gmArrayComplex
/// \brief templated array for complex types, power of 2 autosizing.
///        Coding note: Any complex classes you define which might EVER be referenced by pointer must provide an operator=() which
///        fixes all the pointers.
///        If you do not wish to (for development time, speed, or design reasons) then be sure to provide a no-op PRIVATE operator=()
///        to prevent the CPP compiler for making one up for you. It will screw you up.
///        Same goes for QUAL::operator(const QUAL&);
TMPL class gmArrayComplex
{  
public:
  
  enum
  {
    NULL_INDEX = ~0
  };
  
  gmArrayComplex(void);
  gmArrayComplex(const QUAL &a_array);
  ~gmArrayComplex(void);
  
  template <class J>
  inline void InsertLast(const J &a_elem)
  {
    if(m_count >= m_size)
    {
      Resize(m_count + 1);
    }
#ifdef new
#undef new
    new(&m_elem[m_count]) T(a_elem);
#define new GM_DEBUG_NEW
#else
    new(&m_elem[m_count]) T(a_elem);
#endif
    ++m_count;
  }

  /// \brief SetBlockSize() will set the hysteresis memory grow by in elements. 
  /// \param a_blockSize as 0 will set automatic power of 2.
  inline void SetBlockSize(gmuint a_blockSize) { m_blockSize = a_blockSize; }

  inline bool InsertLastIfUnique(const T &a_elem);
  inline T& InsertLast(void);
  inline void InsertBefore(gmuint a_index, const T &a_elem);
  inline void Remove(gmuint a_index);
  inline void RemoveSwapLast(gmuint a_index);
  inline void RemoveLast(void);
  
  inline T &operator[](gmuint a_index);
  inline const T &operator[](gmuint a_index) const;

  inline gmuint Count(void) const { return m_count; }
  inline bool IsEmpty(void) const { return (m_count == 0); }

  inline void Reset(void) { SetCount(0); }
  void ResetAndFreeMemory(void);

  void SetCount(gmuint a_count);
  void SetCountAndFreeMemory(gmuint a_count);
  inline void Touch(gmuint a_element);

  inline T* GetData(void) { return m_elem; }
  inline const T* GetData(void) const { return m_elem; }
  inline gmuint GetSize(void) { return m_size; }
  bool IsValid(const T* a_elem) const;
  
  QUAL &operator=(const QUAL &a_array);
  
  inline bool FindRemove(const T &a_elem);
  
  template <class Q>
  inline gmuint FindIndex(const Q &a_elem) const
  {
    // iterate backwards, better chance of finding a_elem, given InsertLast() is 
    // used commonly which presents possible element coherence
    if(m_count == 0) return NULL_INDEX;
    gmuint i = m_count - 1;
    do
    {
      if (m_elem[i] == a_elem)    // used commonly which presents possible element coherence
        return i;
    } while(i-- > 0);
    return NULL_INDEX;
  }
  
private:
    
  T *m_elem;
  gmuint m_count, m_size;
  gmuint m_blockSize; //!< 0 and will be power of 2 sizing.
  
  /// \brief Resize() will resize the array.
  /// \param a_size is the required size.
  void Resize(gmuint a_size, bool a_shrinkIfPossible = false);
};


//
// implementation
//


TMPL
inline QUAL::gmArrayComplex(void)
{
  m_elem = NULL;
  m_count = 0;
  m_size = 0;
  m_blockSize = 0; // power of 2 auto
}


TMPL inline QUAL::gmArrayComplex(const QUAL &a_array)
{
  m_elem = NULL;
  m_count = 0;
  m_size = 0;
  m_blockSize = 0; // power of 2 auto
  operator=(a_array);
}


TMPL
inline QUAL::~gmArrayComplex(void)
{
  ResetAndFreeMemory();
}


TMPL bool QUAL::InsertLastIfUnique(const T &a_elem)
{
  if(FindIndex(a_elem) == NULL_INDEX)
  {
    InsertLast(a_elem);
    return false;
  }
  return true;
}


TMPL inline T& QUAL::InsertLast(void)
{
  if(m_count >= m_size)
  {
    Resize(m_count + 1);
  }
#ifdef new
#undef new
  return *new(&m_elem[m_count++]) T();
#define new GM_DEBUG_NEW
#else
  return *new(&m_elem[m_count++]) T();
#endif
}


TMPL
void QUAL::InsertBefore(gmuint a_index, const T &a_elem)
{
  if (a_index >= m_count)
  {
    InsertLast(a_elem);
  }
  else
  {
    if(m_count >= m_size)
    {
      Resize(m_count + 1);
    }
#ifdef new
#undef new
    new(&m_elem[m_count]) T();
#define new GM_DEBUG_NEW
#else
    new(&m_elem[m_count]) T();
#endif
    
    for (gmuint i = m_count; i > a_index; i--)
    {
      m_elem[i] = m_elem[i-1];
    }
    ++m_count;
    m_elem[a_index] = a_elem;
  }
}


TMPL
inline void QUAL::Remove(gmuint a_index)
{
  if (a_index >= m_count) return;
  
  for (unsigned int i = a_index + 1; i < m_count; i++)
    m_elem[i-1] = m_elem[i];
    
  m_elem[--m_count].~T();
}


TMPL
inline void QUAL::RemoveSwapLast(gmuint a_index)
{
  if (a_index >= m_count) return;
  if(--m_count != a_index)
  {
    m_elem[a_index] = m_elem[m_count];
  }
  m_elem[m_count].~T();
}


TMPL
inline void QUAL::RemoveLast(void)
{
  GM_ASSERT(m_count > 0);
  m_elem[--m_count].~T();
}


TMPL
inline T &QUAL::operator[](gmuint a_index)
{
  GM_ASSERT(a_index >= 0 && a_index < m_count);
  return m_elem[a_index];
}


TMPL
inline const T &QUAL::operator[](gmuint a_index) const
{
  GM_ASSERT(a_index >= 0 && a_index < m_count);
  return m_elem[a_index];
}


TMPL
void QUAL::ResetAndFreeMemory(void)
{
  if(m_elem)
  {
    for (gmuint i = 0; i < m_count; i++)
      m_elem[i].~T();
    
    delete [] (char*)m_elem;
    
    m_elem = NULL;
  }
  m_count = m_size = 0;
}


TMPL
void QUAL::SetCount(gmuint a_count)
{
  if (a_count < m_count)
  {
    for (gmuint i = a_count; i < m_count; i++)
      m_elem[i].~T();
  }
  else if (a_count > m_count)
  {
    if (a_count > m_size)
      Resize(a_count);
    
    for (gmuint i = m_count; i < a_count; i++)
    {
#ifdef new
#undef new
      new(&m_elem[i]) T();
#define new GM_DEBUG_NEW
#else
      new(&m_elem[i]) T();
#endif
    }
  }
  
  m_count = a_count;
}


TMPL
void QUAL::SetCountAndFreeMemory(gmuint a_count)
{
  if (a_count < m_count)
  {
    for (gmuint i = a_count; i < m_count; i++)
      m_elem[i].~T();

    Resize(a_count, true);
  }
  else if (a_count > m_count)
  {
    if (a_count > m_size)
      Resize(a_count);
    
    for (gmuint i = m_count; i < a_count; i++)
    {
#ifdef new
#undef new
      new(&m_elem[i]) T();
#define new GM_DEBUG_NEW
#else
      new(&m_elem[i]) T();
#endif
    }
  }
  
  m_count = a_count;
}


TMPL
inline void QUAL::Touch(gmuint a_element)
{
  if(a_element >= m_count)
  {
    SetCount(a_element + 1);
  }
}


TMPL bool QUAL::IsValid(const T* a_elem) const
{
  gmuint index = (a_elem - m_elem);
  return (index < m_count);
}


TMPL
QUAL &QUAL::operator=(const QUAL &a_array)
{
  for (unsigned int i = 0; i < m_count; i++)
    m_elem[i].~T();

  if (m_size >= a_array.m_count)
  {
    m_count = a_array.m_count;
      
    for (unsigned int i = 0; i < m_count; i++)
    {
#ifdef new
#undef new
      new(&m_elem[i]) T(a_array.m_elem[i]);
#define new GM_DEBUG_NEW
#else
      new(&m_elem[i]) T(a_array.m_elem[i]);
#endif
    }

    return (*this);
  }
  
  if (m_elem)
  {
    delete [] (char*)m_elem;
    m_elem = NULL;
  }
  
  m_count = a_array.m_count;
  m_size = a_array.m_count;
  
  if (m_size)
  {
    m_elem = (T*)new char[sizeof(T) * m_size];
    for (unsigned int i = 0; i < m_count; i++)
    {
#ifdef new
#undef new
      new(&m_elem[i]) T(a_array.m_elem[i]);
#define new GM_DEBUG_NEW
#else
      new(&m_elem[i]) T(a_array.m_elem[i]);
#endif
    }
  }
  
  return (*this);
}


TMPL bool QUAL::FindRemove(const T &a_elem)
{
  gmuint index = FindIndex(a_elem);
  if(index != NULL_INDEX)
  {
    Remove(index);
    return true;
  }
  return false;
}


TMPL
void QUAL::Resize(gmuint a_size, bool a_shrinkIfPossible)
{
  if(m_size >= a_size)
  {
    // todo, handle a_shrinkIfPossible.
    a_shrinkIfPossible;
    return;
  }

  // we need to grow, figure out a new size.
  gmuint size = 0;
  if(m_blockSize > 0)
  {
    size = ((a_size / m_blockSize) + 1) * m_blockSize;
  }
  else
  {
    size = gmLog2ge(gmMax<gmuint>(8, a_size + 1));
  }

  // alloc, copy, free
  {
    T *nelem = (T*)(new char[sizeof(T) * size]);
    
    if (m_elem)
    {
      for (unsigned int i = 0; i < m_count; i++)
      {
#ifdef new
#undef new
        new(&nelem[i]) T(m_elem[i]);
#define new GM_DEBUG_NEW
#else
        new(&nelem[i]) T(m_elem[i]);
#endif
        m_elem[i].~T();
      }
    
      delete [] (char*)m_elem;
    }
    
    m_elem = nelem;
  }

  m_size = size;
}

#undef QUAL
#undef TMPL

#endif

