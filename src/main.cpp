#include "CUSketch.h"
#include "SC_CUSketch.h"

#include "SpaceSaving.h"
#include "SC_SpaceSaving.h"

#include "InsertableIblt.h"
#include "SC_InsertableIBLT.h"

#include <ctime>
#include <set>

#define MAX_INSERT_PACKAGE 32000000

unordered_map<uint32_t, int> ground_truth;
uint32_t insert_data[MAX_INSERT_PACKAGE];
uint32_t query_data[MAX_INSERT_PACKAGE];

int load_data(const char *filename) {
    FILE *pf = fopen(filename, "rb");
    if (!pf) {
        cerr << filename << " not found." << endl;
        exit(-1);
    }

    ground_truth.clear();

    char ip[8];
    int ret = 0;
    while (fread(ip, 1, 8, pf)) {
        uint32_t key = *(uint32_t *) ip;
        insert_data[ret] = key;
        ground_truth[key]++;
        ret++;
        if (ret == MAX_INSERT_PACKAGE)
            break;
    }
    fclose(pf);

    int i = 0;
    for (auto itr: ground_truth) {
        query_data[i++] = itr.first;
    }

    printf("Total stream size = %d\n", ret);
    printf("Distinct item number = %d\n", ground_truth.size());

    int max_freq = 0;
    for (auto itr: ground_truth) {
        max_freq = std::max(max_freq, itr.second);
    }
    printf("Max frequency = %d\n", max_freq);

    return ret;
}

void demo_cu(int packet_num)
{
    printf("\nExp for frequency estimation:\n");
    printf("\tAllocate 10KB memory for each algorithm\n");

    auto cu = new CUSketch<10 * 1024, 3>();
    auto sc_cu = new CUSketchWithSC<10 * 1024, 90, 16>();

    int tot_ae;

    // build and query for cu
    for (int i = 0; i < packet_num; ++i) {
        cu->insert(insert_data[i]);
    }

    tot_ae = 0;
    for (auto itr: ground_truth) {
        int report_val = cu->query(itr.first);
        tot_ae += abs(report_val - itr.second);
    }

    printf("\tCU AAE: %lf\n", double(tot_ae) / ground_truth.size());

    // build and query for cu + sc
    for (int i = 0; i < packet_num; ++i) {
        sc_cu->insert(insert_data[i]);
    }
    sc_cu->synchronize(); // refresh SIMD buffer

    tot_ae = 0;
    for (auto itr: ground_truth) {
        int report_val = sc_cu->query(itr.first);
        tot_ae += abs(report_val - itr.second);
    }

    printf("\tSC+CU AAE: %lf\n", double(tot_ae) / ground_truth.size());
}

// return accuary and AAE
pair<double, double> ss_compare_value_with_ground_truth(uint32_t k, vector<pair<uint32_t, uint32_t>> & result)
{
    // prepare top-k ground truth
    vector<pair<uint32_t, int>> gt_ordered;
    for (auto itr: ground_truth) {
        gt_ordered.emplace_back(itr);
    }
    std::sort(gt_ordered.begin(), gt_ordered.end(), [](const std::pair<uint32_t, int> &left, const std::pair<uint32_t, int> &right) {
        return left.second > right.second;
    });
    set<uint32_t> set_gt;
    int i = 0;
    int th;
    for (auto itr: gt_ordered) {
        if (i >= k && itr.second < th) {
            break;
        }
        set_gt.insert(itr.first);
        i++;
        if (i == k) {
            th = itr.second;
        }
    }

    double aae = 0;

    set<uint32_t> set_rp;
    unordered_map<uint32_t, uint32_t> mp_rp;

    int lp = 0;
    for (lp = 0; lp < k; ++lp) {
        set_rp.insert(result[lp].first);
        mp_rp[result[lp].first] = result[lp].second;
    }

    vector<uint32_t> intersection(k);
    auto end_itr = std::set_intersection(
            set_gt.begin(), set_gt.end(),
            set_rp.begin(), set_rp.end(),
            intersection.begin()
    );

    for (auto itr = intersection.begin(); itr != end_itr; ++itr) {
        int diff = int(mp_rp[*itr]) - int(ground_truth[*itr]);
        aae += abs(diff);
    }

    int num = end_itr - intersection.begin();
    num = num > 0 ? num : 1;

    return make_pair(double(num) / k, aae / num);
}

void demo_ss(int packet_num)
{
    printf("\nExp for top-k query:\n");

    const int k = 32;
    printf("\tk = %d\n", k);

    auto ss = new SpaceSaving<128>();
    auto sc_ss = new SC_SpaceSaving<k, 5 * 1024, 150, 16>();

    ss->build(insert_data, packet_num);
    sc_ss->build(insert_data, packet_num);

    vector<pair<uint32_t, uint32_t>> result(k);
    ss->get_top_k_with_frequency(k, result);
    auto ret = ss_compare_value_with_ground_truth(k, result);
    printf("\tSpaceSaving: 128 buckets\n");
    printf("\t  accuracy %lf, AAE %lf\n", ret.first, ret.second);

    sc_ss->get_top_k_with_frequency(k, result);
    ret = ss_compare_value_with_ground_truth(k, result);
    printf("\tSpaceSaving with SC: 32 buckets, 5KB SC\n");
    printf("\t  accuracy %lf, AAE %lf\n", ret.first, ret.second);
}

typedef unordered_map<uint32_t, int> ItemSet;
ItemSet fr_dump_diff(VirtualIBLT * algo1, VirtualIBLT * algo2)
{
    ItemSet a, b, c;
    algo1->dump(a);
    algo2->dump(b);

    // try another method to dump diff
    for (auto itr: a) {
        if (b.find(itr.first) != b.end()) {
            c[itr.first] = a[itr.first] - b[itr.first];
        } else {
            int appr_b = algo2->approximate_query(itr.first);
            if (appr_b != -1) {
                c[itr.first] = a[itr.first] - appr_b;
            }
        }
    }

    for (auto itr: b) {
        if (c.find(itr.first) != c.end()) {
            continue;
        }
        if (a.find(itr.first) != a.end()) {
            c[itr.first] = a[itr.first] - b[itr.first];
            cout << "error" << endl;
        } else {
            int appr_a = algo1->approximate_query(itr.first);
            if (appr_a != -1) {
                c[itr.first] = appr_a - b[itr.first];
            }
        }
    }

    return c;
}

void fr_get_heavy_changer(ItemSet & diff, int threshold, set<uint32_t>& detected)
{
    detected.clear();
    for (auto itr: diff) {
        if (abs(itr.second) >= threshold) {
            detected.insert(itr.first);
        }
    }
}

int fr_get_gt_diff(int package_num, ItemSet & gt_diff)
{
    int i;
    gt_diff.clear();
    for (i = 0; i < package_num / 2; i++) {
        gt_diff[insert_data[i]]++;
    }
    for (; i < package_num; i++) {
        gt_diff[insert_data[i]]--;
    }

    int ret = 0;
    for (auto itr: gt_diff) {
        ret += abs(itr.second);
    }

    return ret;
}

void demo_flow_radar(int packet_num)
{
    printf("\nExp for heavy change detection:\n");

    const int threshold = 200;
    ItemSet detected_diff, gt_diff;
    set<uint32_t> detected_changer, gt_changer;

    fr_get_gt_diff(packet_num, gt_diff);
    fr_get_heavy_changer(gt_diff, threshold, gt_changer);

    int num_intersection;
    double precision, recall, f1;

    // Raw FlowRadar
    auto * fr_a = new InsertableIBLT<100 * 1024>();
    auto * fr_b = new InsertableIBLT<100 * 1024>();
    fr_a->build(insert_data, packet_num / 2);
    fr_b->build(insert_data + packet_num / 2, packet_num / 2);
    detected_diff = fr_dump_diff(fr_a, fr_b);
    fr_get_heavy_changer(detected_diff, threshold, detected_changer);
    vector<uint32_t> intersection(ground_truth.size());
    auto end_itr = std::set_intersection(
            gt_changer.begin(), gt_changer.end(),
            detected_changer.begin(), detected_changer.end(),
            intersection.begin()
    );

    num_intersection = end_itr - intersection.begin();
    precision = detected_changer.size() ? double(num_intersection) / detected_changer.size() : (gt_changer.size() ? 0 : 1);
    recall = gt_changer.size() ? double(num_intersection) / gt_changer.size() : 1;
    f1 = (2 * precision * recall) / (precision + recall);
    printf("\tFlowRadar: 100KB\n");
    printf("\t  precision: %lf, recall: %lf, f1-score: %lf\n", precision, recall, f1);

    // FlowRadar with SC
    auto * sc_fr_a = new SC_InsertableIBLT<2 * 1024, 10 * 1024, 16>();
    auto * sc_fr_b = new SC_InsertableIBLT<2 * 1024, 10 * 1024, 16>();
    sc_fr_a->build(insert_data, packet_num / 2);
    sc_fr_b->build(insert_data + packet_num / 2, packet_num / 2);
    detected_diff = fr_dump_diff(sc_fr_a, sc_fr_b);
    fr_get_heavy_changer(detected_diff, threshold, detected_changer);
    end_itr = std::set_intersection(
            gt_changer.begin(), gt_changer.end(),
            detected_changer.begin(), detected_changer.end(),
            intersection.begin()
    );

    num_intersection = end_itr - intersection.begin();
    precision = detected_changer.size() ? double(num_intersection) / detected_changer.size() : (gt_changer.size() ? 0 : 1);
    recall = gt_changer.size() ? double(num_intersection) / gt_changer.size() : 1;
    f1 = (2 * precision * recall) / (precision + recall);
    printf("\tFlowRadar with SC: 2KB FR + 10KB SC\n");
    printf("\t  precision: %lf, recall: %lf, f1-score: %lf\n", precision, recall, f1);
}

int main()
{
    int packet_num = load_data("../data/sample.dat");
    demo_cu(packet_num);
    demo_ss(packet_num);
    demo_flow_radar(packet_num);
    return 0;
}
