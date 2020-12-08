/*
    _____               __  ___          __            ____        _      __
   / ___/__ ___ _  ___ /  |/  /__  ___  / /_____ __ __/ __/_______(_)__  / /_
  / (_ / _ `/  ' \/ -_) /|_/ / _ \/ _ \/  '_/ -_) // /\ \/ __/ __/ / _ \/ __/
  \___/\_,_/_/_/_/\__/_/  /_/\___/_//_/_/\_\\__/\_, /___/\__/_/ /_/ .__/\__/
                                               /___/             /_/
                                             
  See Copyright Notice in gmMachine.h

*/

#ifndef _GMMEMCHAIN_H_
#define _GMMEMCHAIN_H_

#include "gmConfig.h"

/// \class gmMemChain
/// \brief gmMemChain is a simple memory allocator that allows many small allocations from one heap allocation.
class gmMemChain
{
public:

  gmMemChain(unsigned int a_elementSize, unsigned int a_numElementsInChunk);
  virtual ~gmMemChain();

  /// \brief Alloc()
  void* Alloc(unsigned int a_numElements);
  void* Alloc();

  /// \brief Reset()
  void Reset();
  
  /// \brief ResetAndFreeMemory()
  void ResetAndFreeMemory();
  
  /// \brief GetElementSize()
  inline unsigned int GetElementSize() { return m_elementSize; }

  /// \brief GetSystemMemUsed will return the number of bytes allocated by the system.
  unsigned int GetSystemMemUsed() const;

protected:

  struct MemChunk
  {
    MemChunk* m_nextChunk;
    MemChunk* m_lastChunk;
    void* m_curAddress;
    void* m_minAddress;
    void* m_lastAddress;
  };

  unsigned int m_chunkSize;     //!< Size of memory chunk
  unsigned int m_elementSize;   //!< Size of element
  MemChunk* m_rootChunk;        //!< First chunk in chain
  MemChunk* m_currentChunk;     //!< Current chunk alloc's use

  inline MemChunk* NewChunk();
  inline void FreeChunks();
};


#endif // _GMMEMCHAIN_H_
