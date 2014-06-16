// *NOTE*: uncomment the line corresponding to your platform !
//#include "ace/config-win32.h"
#include "ace/config-linux.h"

// *NOTE*: don't use the regular pipe-based mechanism,
// it has several drawbacks (see relevant documentation)
#define ACE_HAS_REACTOR_NOTIFICATION_QUEUE

// *NOTE*: don't use the WFMO-reactor on windows platforms
#define ACE_USE_SELECT_REACTOR_FOR_REACTOR_IMPL
