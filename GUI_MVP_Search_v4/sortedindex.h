#ifndef SORTEDINDEX_H
#define SORTEDINDEX_H

#include <QString>

class SortedIndex {
public:
    SortedIndex() : val(-1), idx(-1), str("") {}
    SortedIndex(float val,int idx, const QString& str ){ this->val=val; this->idx=idx; this->str = str; }
    ~SortedIndex() {}
    float   val;
    int     idx;
    QString str;
    bool operator<(const SortedIndex& other)  const {
        return ((*this).val<other.val);
    }
    QString toQString() const;
};

#endif // SORTEDINDEX_H
