#ifndef _SC_CU_SKETCH_H
#define _SC_CU_SKETCH_H

#include "CUSketch.h"
#include "SC.h"

#define T1 15
#define T2 241
#define THRESHOLD (T1 + T2)


template<uint64_t total_memory_in_bytes, int filter_memory_percent, int bucket_num = 1000>
class CUSketchWithSC
{
public:
    StreamClassifier<int64_t(total_memory_in_bytes) * filter_memory_percent / 100, bucket_num, 16, T2> sc;
    CUSketch<int((total_memory_in_bytes) * (100 - filter_memory_percent) / 100), 3> cu;
    CUSketchWithSC()
    {
        sc.init_spa(&cu);
//        sc.print_basic_info();
//        cu.print_basic_info();
    }

    inline void insert(uint32_t item)
    {
        sc.insert(item);
    }

    inline void synchronize()
    {
        sc.refresh();
    }

    inline void build(uint32_t * items, int n)
    {
        for (int i = 0; i < n; ++i) {
            sc.insert(items[i]);
        }
        sc.refresh();
    }

    inline int query(uint32_t item)
    {
        int ret = sc.query(item);
        if (ret == THRESHOLD)
            ret += cu.query(item);

        return ret;
    }

    inline int batch_query(uint32_t items[], int n)
    {
        int ret = 0;
        for (int i = 0; i < n; ++i) {
            ret += CUSketchWithSC::query(items[i]);
        }

        return ret;
    }
};

#endif // _SCCUSKETCH_H