
// -*- C++ -*-
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -n RPG_Client
// ------------------------------
#ifndef RPG_CLIENT_EXPORT_H
#define RPG_CLIENT_EXPORT_H

#include "ace/config-all.h"

#if !defined (RPG_CLIENT_HAS_DLL)
#  define RPG_CLIENT_HAS_DLL 1
#endif /* ! RPG_CLIENT_HAS_DLL */

#if defined (RPG_CLIENT_HAS_DLL) && (RPG_CLIENT_HAS_DLL == 1)
#  if defined (RPG_CLIENT_BUILD_DLL)
#    define RPG_Client_Export ACE_Proper_Export_Flag
#    define RPG_CLIENT_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define RPG_CLIENT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* RPG_CLIENT_BUILD_DLL */
#    define RPG_Client_Export ACE_Proper_Import_Flag
#    define RPG_CLIENT_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define RPG_CLIENT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* RPG_CLIENT_BUILD_DLL */
#else /* RPG_CLIENT_HAS_DLL == 1 */
#  define RPG_Client_Export
#  define RPG_CLIENT_SINGLETON_DECLARATION(T)
#  define RPG_CLIENT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* RPG_CLIENT_HAS_DLL == 1 */

// Set RPG_CLIENT_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (RPG_CLIENT_NTRACE)
#  if (ACE_NTRACE == 1)
#    define RPG_CLIENT_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define RPG_CLIENT_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !RPG_CLIENT_NTRACE */

#if (RPG_CLIENT_NTRACE == 1)
#  define RPG_CLIENT_TRACE(X)
#else /* (RPG_CLIENT_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define RPG_CLIENT_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (RPG_CLIENT_NTRACE == 1) */

#endif /* RPG_CLIENT_EXPORT_H */

// End of auto generated file.
