// *NOTE*: uncomment the line corresponding to your platform !
#include "ace/config-linux.h"

// *NOTE*: don't use the regular pipe-based mechanism,
// it has several drawbacks (see relevant documentation)
#define ACE_HAS_REACTOR_NOTIFICATION_QUEUE

// *NOTE*: libpthread now supports thread names
//         (see /usr/include/pthread.h::453: pthread_setname_np())
//#define ACE_HAS_PTHREAD_ATTR_SETNAME

// *NOTE*: set FD_SETSIZE so select-based reactors can dispatch more than the
//         default (1024, see below) handles
//#include <bits/typesizes.h>
#include <sys/types.h>
#undef __FD_SETSIZE
#define __FD_SETSIZE 65536
#include <sys/select.h>
//#include <linux/posix_types.h>
