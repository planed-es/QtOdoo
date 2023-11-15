#ifndef QODOO_GLOBAL_H
#define QODOO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTODOO_LIBRARY)
#  define QTODOO_LIBRARY_EXPORT Q_DECL_EXPORT
#else
#  define QTODOO_LIBRARY_EXPORT Q_DECL_IMPORT
#endif

#endif // QODOO_GLOBAL_H
