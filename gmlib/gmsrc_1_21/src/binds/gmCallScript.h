/*
    _____               __  ___          __            ____        _      __
   / ___/__ ___ _  ___ /  |/  /__  ___  / /_____ __ __/ __/_______(_)__  / /_
  / (_ / _ `/  ' \/ -_) /|_/ / _ \/ _ \/  '_/ -_) // /\ \/ __/ __/ / _ \/ __/
  \___/\_,_/_/_/_/\__/_/  /_/\___/_//_/_/\_\\__/\_, /___/\__/_/ /_/ .__/\__/
                                               /___/             /_/
                                             
  See Copyright Notice in gmMachine.h

*/

#ifndef _GMCALLSCRIPT_H_
#define _GMCALLSCRIPT_H_

#include "gmConfig.h"
#include "gmThread.h"
#include "gmMachine.h"

#undef GetObject //Fix for Win32 where GetObject is #defined

/// \class gmCallScript
/// \brief A helper class to call script functions from C
class gmCallScript
{
public:
  
  /// \brief Used to check if the machine has been set, or is the right one.
  /// \return Pointer to a gmMachine or NULL if not set.
  GM_FORCEINLINE static gmMachine * GetMachine()
  {
    return s_machine;
  }

  /// \brief Set the gmMachine
  /// \param a_machine The machine to use when calling script functions.
  GM_FORCEINLINE static void SetMachine(gmMachine * a_machine)
  {
    s_machine = a_machine;
  }

  /// \brief Begin the call of a global function
  /// \param a_funcName Name of function
  /// \return true on sucess, false if function was not found.
  GM_FORCEINLINE static bool BeginGlobal(const char* a_funcName, bool a_delayExecuteFlag = false, gmVariable* a_this = NULL)
  {
    GM_ASSERT(s_machine);

    gmStringObject* funcNameStringObj = s_machine->AllocPermanantStringObject(a_funcName); // Slow
    
    return BeginGlobal(funcNameStringObj, a_delayExecuteFlag, a_this);
  }

  /// \brief Begin the call of a global function
  /// \param a_funcNameStringObj A string object that was found or created earlier, much faster than creating from c string.
  /// \return true on sucess, false if function was not found.
  GM_FORCEINLINE static bool BeginGlobal(gmStringObject* a_funcNameStringObj, bool a_delayExecuteFlag = false, gmVariable* a_this = NULL)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(a_funcNameStringObj);
    
#ifdef GM_DEBUG_BUILD
    // YOU CANNOT NEST gmCallScript::Begin
    GM_ASSERT(s_locked == false);
    s_locked = true;
#endif
    
    Reset();
  
    gmVariable lookUpVar;
    gmVariable foundFunc;

    lookUpVar.SetString(a_funcNameStringObj);
    foundFunc = s_machine->GetGlobals()->Get(lookUpVar);

    if( GM_FUNCTION == foundFunc.m_type )         // Check found variable is a function
    {
      s_thread = s_machine->CreateThread();       // Create thread for func to run on
      gmFunctionObject * functionObj = (gmFunctionObject *)foundFunc.m_value.m_ref; //Func Obj from variable
      if(!a_this)
      {
        s_thread->PushNull();                     // this
      }
      else
      {
        s_thread->Push(*a_this);                  // this
      }
      s_thread->PushFunction(functionObj);        // function
      s_delayExecuteFlag = a_delayExecuteFlag;
      return true;
    }

#ifdef GM_DEBUG_BUILD
    GM_ASSERT(s_locked == true);
    s_locked = false;
#endif

    return false;
  }

  /// \brief Begin the call of a object function
  /// \param a_funcName Name of function.
  /// \param a_tableObj The table on the object to look up the function.
  /// \param a_userObj The 'this' pointer used by the function.
  /// \return true on sucess, false if function was not found.
  GM_FORCEINLINE static bool BeginObject(const char* a_funcName, gmTableObject* a_tableObj, gmUserObject * a_userObj, bool a_delayExecuteFlag = false)
  {
    GM_ASSERT(s_machine);

    gmStringObject* funcNameStringObj = s_machine->AllocPermanantStringObject(a_funcName); // Slow
    
    return BeginObject(funcNameStringObj, a_tableObj, a_userObj, a_delayExecuteFlag);
  }

  /// \brief Begin the call of a object function
  /// \param a_funcNameStringObj A string object that was found or created earlier, much faster than creating from c string.
  /// \param a_tableObj The table on the object to look up the function.
  /// \param a_userObj The 'this' pointer used by the function.
  /// \return true on sucess, false if function was not found.
  GM_FORCEINLINE static bool BeginObject(gmStringObject * a_funcNameStringObj, gmTableObject * a_tableObj, gmUserObject * a_userObj, bool a_delayExecuteFlag = false)
  {
    GM_ASSERT(s_machine);   
    GM_ASSERT(a_tableObj);
    GM_ASSERT(a_funcNameStringObj);
    GM_ASSERT(a_userObj);

#ifdef GM_DEBUG_BUILD
    // YOU CANNOT NEST gmCallScript::Begin
    GM_ASSERT(s_locked == false);
    s_locked = true;
#endif

    Reset();
  
    gmVariable lookUpVar;
    gmVariable foundFunc;

    lookUpVar.SetString(a_funcNameStringObj);
    foundFunc = a_tableObj->Get(lookUpVar);

    if( GM_FUNCTION == foundFunc.m_type )         // Check found variable is a function
    {
      s_thread = s_machine->CreateThread();       // Create thread for func to run on
      gmFunctionObject * functionObj = (gmFunctionObject *)foundFunc.m_value.m_ref; //Func Obj from variable
      s_thread->PushUser(a_userObj);              // this
      s_thread->PushFunction(functionObj);        // function
      s_delayExecuteFlag = a_delayExecuteFlag;
      return true;
    }

#ifdef GM_DEBUG_BUILD
    GM_ASSERT(s_locked == true);
    s_locked = false;
#endif

    return false;
  }
 
   /// \brief Add a parameter that is null
  GM_FORCEINLINE static void AddParamNull()
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_thread->PushNull();
    ++s_paramCount;
  }

  /// \brief Add a parameter that is a integer
  GM_FORCEINLINE static void AddParamInt(const int a_value)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_thread->PushInt(a_value);
    ++s_paramCount;
  }

  /// \brief Add a parameter that is a float
  GM_FORCEINLINE static void AddParamFloat(const float a_value)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_thread->PushFloat(a_value);
    ++s_paramCount;
  }

  /// \brief Add a parameter that is a string
  GM_FORCEINLINE static void AddParamString(const char * a_value, int a_len = -1)
  {
    GM_ASSERT(s_machine);    
    GM_ASSERT(s_thread);

    s_thread->PushNewString(a_value, a_len);
    ++s_paramCount;
  }

  /// \brief Add a parameter that is a string (faster version since c string does not need lookup)
  GM_FORCEINLINE static void AddParamString(gmStringObject * a_value)
  {
    GM_ASSERT(s_machine);    
    GM_ASSERT(s_thread);

    s_thread->PushString(a_value);
    ++s_paramCount;
  }

  /// \brief Add a parameter that is a user object.  Creates a new user object.
  /// \param a_value Pointer to user object data
  /// \param a_userType Type of user object beyond GM_USER
  GM_FORCEINLINE static void AddParamUser(void * a_value, int a_userType)
  {
    GM_ASSERT(s_machine);    
    GM_ASSERT(s_thread);

    s_thread->PushNewUser(a_value, a_userType);
    ++s_paramCount;
  }

  /// \brief Add a parameter that is a user object.
  /// \param a_userObj Pushes an existing user object without creating a new one.
  GM_FORCEINLINE static void AddParamUser(gmUserObject * a_userObj)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_thread->PushUser(a_userObj);
    ++s_paramCount;
  }

  /// \brief Signal that we expect a return value that is a integer
  GM_FORCEINLINE static void SetReturnInt(int& a_valuePtr)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_retValueType = GM_INT;
    s_retValue = &a_valuePtr;
  }

  /// \brief Signal that we expect a return value that is a float
  GM_FORCEINLINE static void SetReturnFloat(float& a_valuePtr)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_retValueType = GM_FLOAT;
    s_retValue = &a_valuePtr;
  }

  /// \brief Signal that we expect a return value that is a string
  // WARNING: You cannot hold onto this pointer.  User it, or string copy immediately.
  // The string may be deleted in the next garbage collect cycle.
  GM_FORCEINLINE static void SetReturnString(const char*& a_valuePtr)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);

    s_retValueType = GM_STRING;
    s_retValue = &a_valuePtr;
  }

  /// \brief Signal that we expect a return value that is a user object
  GM_FORCEINLINE static void SetReturnUser(void*& a_valuePtr, int a_userType)
  {
    GM_ASSERT(s_machine);    
    GM_ASSERT(s_thread);

    s_retValueType = a_userType;
    s_retValue = &a_valuePtr;
  }

  /// \brief Make the call.  If a return value was expected, it will be set in here.
  /// \param a_threadId Optional 
  GM_FORCEINLINE static void End(int * a_threadId = NULL)
  {
    GM_ASSERT(s_machine);
    GM_ASSERT(s_thread);
    
#ifdef GM_DEBUG_BUILD
    // CAN ONLY CALL ::End() after a successful ::Begin
    GM_ASSERT(s_locked == true);
    s_locked = false;
#endif
        
    gmVariable returnVar;

    int state = s_thread->PushStackFrame(s_paramCount);

    if(state != gmThread::KILLED) //Can be killed immedialy if it was a C function
    {
      if(s_delayExecuteFlag)
      {
        state = s_thread->GetState();
      }
      else
      {
        state = s_thread->Sys_Execute(&returnVar);
      }
    }
    else
    {
      //Was a C function call, grab return var off top of stack
      returnVar = *(s_thread->GetTop() - 1);
    }

    // If we requested a thread Id
    if(a_threadId)
    {
      if(state != gmThread::KILLED)
      {
        *a_threadId = s_thread->GetId();
      }
      else
      {
        *a_threadId = GM_INVALID_THREAD;
      }
    }

    // If we requested a return value
    if(s_retValue)
    {
      GM_ASSERT(gmThread::KILLED == state); // Thread must exit to return value, not yeild etc.
      
      if(s_retValueType == returnVar.m_type)
      {
        if(s_retValueType < GM_STRING)
        {
          *(gmptr *)s_retValue = returnVar.m_value.m_ref;
        }
        else if(GM_STRING == s_retValueType)
        {
          *(const char **)s_retValue = ((gmStringObject*)s_machine->GetObject(returnVar.m_value.m_ref))->GetString();
        }
        else // Some kind of user object
        {
          gmUserObject * userObj = (gmUserObject *)s_machine->GetObject(returnVar.m_value.m_ref);
          *(gmptr*)s_retValue = (gmptr)userObj->m_user;
        }
      }
      else
      {
        s_retValue = NULL;
      }
    }
  }

  static GM_FORCEINLINE gmThread * Thread() { return s_thread; }

protected:

  /// \brief Used internally to clear call information.
  GM_FORCEINLINE static void Reset()
  {
    GM_ASSERT(s_machine);
    s_thread = NULL;
    s_retValue = NULL;
    s_paramCount = 0;
    s_retValueType = -1;
    s_delayExecuteFlag = false;
  };

#ifdef GM_DEBUG_BUILD
  static bool s_locked;
#endif

  static gmMachine * s_machine;
  static gmThread * s_thread;
  static int s_paramCount;
  static void * s_retValue;
  static int s_retValueType;
  static bool s_delayExecuteFlag;
};


#endif // _GMCALLSCRIPT_H_