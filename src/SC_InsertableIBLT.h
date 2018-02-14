#ifndef STREAMCLASSIFIER_SC_IBLT_H
#define STREAMCLASSIFIER_SC_IBLT_H

#include "InsertableIblt.h"
#include "SC.h"

#define T2 241

template<uint64_t iblt_size_in_bytes, uint64_t sc_memory_in_bytes, uint32_t bucket_num = 200>
class SC_InsertableIBLT: public VirtualIBLT
{
    StreamClassifier<sc_memory_in_bytes, bucket_num, 16, T2> sc;
    InsertableIBLT<iblt_size_in_bytes> iblt;
public:
    SC_InsertableIBLT() {
        sc.init_spa(&iblt);
//        sc.print_basic_info();
    }

    inline void build(uint32_t * items, int n)
    {
        for (int i = 0; i < n; ++i) {
            sc.insert(items[i]);
        }

        sc.refresh();
    }


    void dump(unordered_map<uint32_t, int> &result) {
        iblt.dump(result);

        for (auto itr: result) {
            int remain = sc.query(itr.first);
            result[itr.first] += remain;
        }
    }

    int approximate_query(uint32_t key) {
        int val_sc = sc.query(key);
        int val_iblt = iblt.approximate_query(key);

        if (val_iblt == 0) {
            return val_sc;
        } else if (val_sc != 15 + T2) {
            return val_sc;
        } else {
            return -1;
        }
    }
};

#endif //STREAMCLASSIFIER_SC_IBLT_H
