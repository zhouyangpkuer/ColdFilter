#ifndef _MYREV1_H
#define _MYREV1_H

#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <iterator>
#include "SPA.h"
#include "BOBHash32.h"

using namespace std;

class VirtualIBLT
{
public:
    virtual void build(uint32_t * data, int n) = 0;
    virtual void dump(unordered_map<uint32_t, int> &result) = 0;
    virtual int approximate_query(uint32_t key) = 0;
};

template<uint64_t total_in_bytes, uint32_t bf_k = 4, uint32_t iblt_k = 3>
class InsertableIBLT: public SPA, public VirtualIBLT
{
private:
    struct Node {
        uint32_t flow_sum;
        int flow_num;
        int packet_num;
    };

    static constexpr int bf_size_in_bytes = total_in_bytes * 0.1;
    static constexpr int iblt_size_in_bytes = total_in_bytes * 0.9;

    static constexpr int w_bf = bf_size_in_bytes * 8;
    static constexpr int w_iblt = iblt_size_in_bytes / sizeof(Node);
    BOBHash32 * bobhash[bf_k + iblt_k];
    uint64_t * bf;
    Node * iblt;

public:
    int num_flow = 0;

    InsertableIBLT() {
//        printf("w_ibf = %d\n", w_iblt);
        bf = new uint64_t[w_bf >> 6];
        memset(bf, 0, sizeof(uint64_t) * (w_bf >> 6));
        iblt = new Node[w_iblt];
        memset(iblt, 0, sizeof(Node) * w_iblt);

        for (unsigned i = 0; i < bf_k + iblt_k; i++) {
            bobhash[i] = new BOBHash32(i + 500);
        }
    }

    void insert(uint32_t key, int f = 1) {
        int flag = 1;
        int index[bf_k + iblt_k];

        for (unsigned i = 0; i < bf_k; i++) {
            index[i] = (bobhash[i]->run((const char *) &key, 4)) % w_bf;
            if (((bf[index[i] >> 6] >> (index[i] & 0x3F)) & 1) == 0) {
                flag = 0;
            }
            bf[index[i] >> 6] |= ((uint64) 1 << (index[i] & 0x3F));
        }

        for (unsigned i = 0; i < iblt_k; i++) {
            index[i] = (bobhash[i + bf_k]->run((const char *) &key, 4)) % w_iblt;
            if (flag == 0) {
                num_flow++;
                iblt[index[i]].flow_sum ^= key;
                iblt[index[i]].flow_num++;
            }
            iblt[index[i]].packet_num += f;
        }
    }

    void build(uint32_t * data, int n) {
        for (int i = 0; i < n; ++i) {
            insert(data[i]);
        }
    }

    void dump(unordered_map<uint32_t, int> &result) {
        uint32_t key;
        int count;
        int index[bf_k + iblt_k];

        while (1) {
            bool flag = true;
            for (int i = 0; i < w_iblt; i++) {
                if (iblt[i].flow_num == 1) {
                    key = iblt[i].flow_sum;
                    count = iblt[i].packet_num;
                    result[key] = count;

                    for (unsigned j = 0; j < iblt_k; j++) {
                        index[j] = (bobhash[j + bf_k]->run((const char *) &key, 4)) % w_iblt;

                        iblt[index[j]].flow_num--;
                        iblt[index[j]].flow_sum ^= key;
                        iblt[index[j]].packet_num -= count;
                    }
                    flag = false;
                }
            }
            if (flag)
                break;
        }
    }

    int approximate_query(uint32_t key) {
        for (unsigned i = 0; i < bf_k; i++) {
            int idx = (bobhash[i]->run((const char *) &key, 4)) % w_bf;
            if (((bf[idx >> 6] >> (idx & 0x3F)) & 1) == 0) {
                return 0;
            }
        }
        return -1;
    }
};

#endif//_MYREV1_H
