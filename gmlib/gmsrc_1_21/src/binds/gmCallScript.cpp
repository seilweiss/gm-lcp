/*
    _____               __  ___          __            ____        _      __
   / ___/__ ___ _  ___ /  |/  /__  ___  / /_____ __ __/ __/_______(_)__  / /_
  / (_ / _ `/  ' \/ -_) /|_/ / _ \/ _ \/  '_/ -_) // /\ \/ __/ __/ / _ \/ __/
  \___/\_,_/_/_/_/\__/_/  /_/\___/_//_/_/\_\\__/\_, /___/\__/_/ /_/ .__/\__/
                                               /___/             /_/
                                             
  See Copyright Notice in gmMachine.h

*/

#include "gmConfig.h"
#include "gmCallScript.h"

// Init statics
#ifdef GM_DEBUG_BUILD
bool gmCallScript::s_locked = false;
#endif
gmMachine * gmCallScript::s_machine;
gmThread * gmCallScript::s_thread;
int gmCallScript::s_paramCount;
void * gmCallScript::s_retValue;
int gmCallScript::s_retValueType;
bool gmCallScript::s_delayExecuteFlag;
