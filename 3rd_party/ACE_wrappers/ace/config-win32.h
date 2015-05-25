// *NOTE*: uncomment the line corresponding to your platform !
#include "ace/config-win32.h"

// *NOTE*: don't use the regular pipe-based mechanism,
// it has several drawbacks (see relevant documentation)
#define ACE_HAS_REACTOR_NOTIFICATION_QUEUE

// *NOTE*: don't use the WFMO-reactor on Microsoft Windows (TM) platforms,
//         it only supports ~64 concurrent handles...
#define ACE_USE_SELECT_REACTOR_FOR_REACTOR_IMPL

// *NOTE*: the proactor ACE code contains SEH-enabled code; however, the default
//         MPC (auto-)generated project files do not apply the correct
//         compilation flags (sets /EHsc instead of /EHa), leading to memory leaks
//         --> as a workaround, disable SEH
#undef ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS
