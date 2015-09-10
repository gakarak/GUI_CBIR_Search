#include "sortedindex.h"

QString SortedIndex::toQString() const
{
    return QString("(%1:%2:%3)").arg(idx).arg(val).arg(str);
}



