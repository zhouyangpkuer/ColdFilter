#ifndef _ASKETCH_H
#define _ASKETCH_H

#include "params.h"
#include "BOBHash32.h"
#include "SPA.h"
#include <cstring>
#include <x86intrin.h>
#include <bmiintrin.h>
using namespace std;

template<int tot_memory_in_bytes, int filter_size, int d>
class ASketch
{
private:
    static constexpr int w = (tot_memory_in_bytes - filter_size * 12) * 8 / 32 / d;
    static constexpr int bucket_num = filter_size / 16;

    int new_count[filter_size];
    int old_count[filter_size];
    uint32_t items[filter_size];

    int cur_pos;

    int counter[d][w];

    BOBHash32 *bobhash[MAX_HASH_NUM];
public:
    ASketch()
    {
        memset(counter, 0, sizeof(counter));
        memset(items, 0, sizeof(items));
        memset(new_count, 0, sizeof(new_count));
        memset(old_count, 0, sizeof(old_count));
        cur_pos = 0;

        for (int i = 0; i < d; i++)
        {
            bobhash[i] = new BOBHash32(i + 1000);
        }
    }

    uint32_t * get_items()
    {
        return items;
    }

    int * get_freq()
    {
        return new_count;
    }

    void insert(uint32_t key, int f)
    {
        const __m128i item = _mm_set1_epi32((int)key);

        for (int i = 0; i < bucket_num; i++)
        {
            __m128i *keys_p = (__m128i *)(items + (i << 4));

            __m128i a_comp = _mm_cmpeq_epi32(item, keys_p[0]);
            __m128i b_comp = _mm_cmpeq_epi32(item, keys_p[1]);
            __m128i c_comp = _mm_cmpeq_epi32(item, keys_p[2]);
            __m128i d_comp = _mm_cmpeq_epi32(item, keys_p[3]);

            a_comp = _mm_packs_epi32(a_comp, b_comp);
            c_comp = _mm_packs_epi32(c_comp, d_comp);
            a_comp = _mm_packs_epi32(a_comp, c_comp);

            int matched = _mm_movemask_epi8(a_comp);

            if(matched != 0)
            {
                //return 32 if input is zero;
                int matched_index = _tzcnt_u32((uint32_t)matched) + (i << 4);
                new_count[matched_index] += f;
                return;
            }
        }

        if(cur_pos != filter_size)
        {
            items[cur_pos] = key;
            new_count[cur_pos] = f;
            old_count[cur_pos] = 0;
            cur_pos ++;
            return;
        }


        int estimate_value, min_index, min_value, temp;

        int index[MAX_HASH_NUM];
        estimate_value = (1 << 30);
        for(int i = 0; i < d; i++)
        {
            index[i] = (bobhash[i]->run((const char *)&key, 4)) % w;

            // counter[i][hash_value] ++;
            temp = counter[i][index[i]];

            estimate_value = estimate_value < temp ? estimate_value : temp;
        }
        estimate_value += f;
        for(int i = 0; i < d; i++)
        {
            if(counter[i][index[i]] < estimate_value)
            {
                counter[i][index[i]] = 	estimate_value;
            }
        }



        min_index = 0;
        min_value = (1 << 30);
        for(int i = 0; i < filter_size; i++)
        {
            if(items[i] != (uint32_t)(-1) && min_value > new_count[i])
            {
                min_value = new_count[i];
                min_index = i;
            }
        }
        if(estimate_value > min_value)
        {
            temp = new_count[min_index] - old_count[min_index];
            if(temp > 0)
            {
                min_value = (1 << 30);
                for(int i = 0; i < d; i++)
                {
                    index[i] = (bobhash[i]->run((const char *)&items[min_index], 4)) % w;
                    min_value = min_value < counter[i][index[i]] ? min_value : counter[i][index[i]];
                }
                for(int i = 0; i < d; i++)
                {
                    if(counter[i][index[i]] < min_value + temp)
                        counter[i][index[i]] = min_value + temp;
                }
            }
            items[min_index] = key;
            new_count[min_index] = estimate_value;
            old_count[min_index] = estimate_value;
        }
    }

    void build(uint32_t * data, int n) {
        for (int i = 0; i < n; ++i) {
            insert(data[i], 1);
        }
    }

    int query(uint32_t key)
    {
        const __m128i item = _mm_set1_epi32((int)key);
        for (int i = 0; i < bucket_num; i++)
        {
            __m128i *keys_p = (__m128i *)(items + (i << 4));

            __m128i a_comp = _mm_cmpeq_epi32(item, keys_p[0]);
            __m128i b_comp = _mm_cmpeq_epi32(item, keys_p[1]);
            __m128i c_comp = _mm_cmpeq_epi32(item, keys_p[2]);
            __m128i d_comp = _mm_cmpeq_epi32(item, keys_p[3]);

            a_comp = _mm_packs_epi32(a_comp, b_comp);
            c_comp = _mm_packs_epi32(c_comp, d_comp);
            a_comp = _mm_packs_epi32(a_comp, c_comp);

            int matched = _mm_movemask_epi8(a_comp);

            if(matched != 0)
            {
                //return 32 if input is zero;
                int matched_index = _tzcnt_u32((uint32_t)matched) + (i << 4);
                return new_count[matched_index];
            }
        }


        int hash_value, temp;
        int estimate_value = (1 << 30);
        for(int i = 0; i < d; i++)
        {
            hash_value = (bobhash[i]->run((const char *)&key, 4)) % w;
            temp = counter[i][hash_value];
            estimate_value = estimate_value < temp ? estimate_value : temp;
        }
        return estimate_value;
    }

    int batch_query(uint32_t * data, int n)
    {
        int ret = 0;
        for (int i = 0; i < n; ++i) {
            ret += query(data[i]);
        }

        return ret;
    }

    ~ASketch()
    {
        for(int i = 0; i < d; i++)
        {
            delete bobhash[i];
        }
    }
};

#endif//_ASKETCH_H