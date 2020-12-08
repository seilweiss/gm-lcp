/*
    _____               __  ___          __            ____        _      __
   / ___/__ ___ _  ___ /  |/  /__  ___  / /_____ __ __/ __/_______(_)__  / /_
  / (_ / _ `/  ' \/ -_) /|_/ / _ \/ _ \/  '_/ -_) // /\ \/ __/ __/ / _ \/ __/
  \___/\_,_/_/_/_/\__/_/  /_/\___/_//_/_/\_\\__/\_, /___/\__/_/ /_/ .__/\__/
                                               /___/             /_/
                                             
  See Copyright Notice in gmMachine.h

*/

#ifndef _GMMEM_H_
#define _GMMEM_H_

// include malloc and alloca if required within gmconfig_p.h

#include "gmConfig.h"

// define _gmDumpLeaks within gmconfig_p.h to dump memory leaks

#ifndef _gmDumpLeaks
#define _gmDumpLeaks()
#endif

#define _gmAlignMem(PTR, ALIGN)              (void *) ((unsigned int) (((unsigned char *) (PTR)) + ((ALIGN) - 1)) & ~((ALIGN) - 1))


/// \brief gmConstructElement will construct a single object at location
template<class TYPE>
inline TYPE* gmConstructElement(TYPE* a_element)
{
#ifdef new
#undef new
  return (TYPE*) ::new((void*)a_element) TYPE;
#define new GM_DEBUG_NEW
#else
  return (TYPE*) ::new((void*)a_element) TYPE;
#endif  
}



/// \brief gmDestructElement will destruct a single object at location
template<class TYPE>
inline void gmDestructElement(TYPE* a_element)
{
  a_element->~TYPE();
}



/// \brief gmConstructElements will construct multiple objects at location
template<class TYPE>
inline void gmConstructElements(TYPE* a_elements, int a_count)
{
  // Call the constructor(s)
  for (; a_count--; a_elements++)
  {
#ifdef new
#undef new
    ::new((void*)a_elements) TYPE;
#define new GM_DEBUG_NEW
#else
    ::new((void*)a_elements) TYPE;
#endif  
  }
}



/// \brief gmDestructElements will destruct multiple objects at location
template<class TYPE>
inline void gmDestructElements(TYPE* a_elements, int a_count)
{
  // Call the destructor(s)
  for (; a_count--; a_elements++)
  {
    a_elements->~TYPE();
  }
}

#endif // _GMMEM_H_