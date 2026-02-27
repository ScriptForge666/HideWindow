#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(GLOBAL_HOOK_LIB)
#  define GLOBAL_HOOK_EXPORT Q_DECL_EXPORT
# else
#  define GLOBAL_HOOK_EXPORT Q_DECL_IMPORT
# endif
#else
# define GLOBAL_HOOK_EXPORT
#endif
