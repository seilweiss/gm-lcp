/*
    _____               __  ___          __            ____        _      __
   / ___/__ ___ _  ___ /  |/  /__  ___  / /_____ __ __/ __/_______(_)__  / /_
  / (_ / _ `/  ' \/ -_) /|_/ / _ \/ _ \/  '_/ -_) // /\ \/ __/ __/ / _ \/ __/
  \___/\_,_/_/_/_/\__/_/  /_/\___/_//_/_/\_\\__/\_, /___/\__/_/ /_/ .__/\__/
                                               /___/             /_/
                                             
  See Copyright Notice in gmMachine.h

*/

#include "gmConfig.h"
#include "gmMemFixedSet.h"

void gmMemFixedSet::Reset()
{
  m_mem16.Reset();
  m_mem32.Reset();
  m_mem64.Reset();
  m_mem128.Reset();
  m_mem256.Reset();
  m_mem512.Reset();
  FreeBigAllocs();  //This actually frees the big fellas
}



void gmMemFixedSet::ResetAndFreeMemory()
{
  m_mem16.ResetAndFreeMemory();
  m_mem32.ResetAndFreeMemory();
  m_mem64.ResetAndFreeMemory();
  m_mem128.ResetAndFreeMemory();
  m_mem256.ResetAndFreeMemory();
  m_mem512.ResetAndFreeMemory();
  FreeBigAllocs();
}



unsigned int gmMemFixedSet::GetSystemMemUsed() const
{
  unsigned int total = 0;
  total += m_mem16.GetSystemMemUsed();
  total += m_mem32.GetSystemMemUsed();
  total += m_mem64.GetSystemMemUsed();
  total += m_mem128.GetSystemMemUsed();
  total += m_mem256.GetSystemMemUsed();
  total += m_mem512.GetSystemMemUsed();

  
  BigMemNode* curNode = m_bigAllocs.GetFirst();
  while(m_bigAllocs.IsValid(curNode))
  {
    total += curNode->m_size;
    curNode = m_bigAllocs.GetNext(curNode);
  }
  return total;
}



void gmMemFixedSet::FreeBigAllocs()
{
  BigMemNode* curNode = m_bigAllocs.GetFirst();
  while(m_bigAllocs.IsValid(curNode))
  {
    BigMemNode* nodeToDelete = curNode;
    curNode = m_bigAllocs.GetNext(curNode);

    delete [] (char*)nodeToDelete;
  }
  m_bigAllocs.RemoveAll();
}