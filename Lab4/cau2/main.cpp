#include "scheduler.hpp"
#include <random>
#include <chrono>
#include <iomanip>
#include <vector>

using namespace std;

struct TestSummary {
    int caseId;
    int quantum;
    uint32_t seed;
    float avgResponse;
    float avgTaT;
    float avgWait;
};

int main() {
    random_device rd;
    uint32_t masterSeed = static_cast<uint32_t>(rd()) ^
        static_cast<uint32_t>(chrono::steady_clock::now().time_since_epoch().count());
    mt19937 seedGen(masterSeed);

    const int NUM_TEST_CASES = 3;
    const int PROCESS_COUNT = 5;
    const int QUANTUM = 4;

    // 1. Random ra 3 seed khác nhau
    vector<uint32_t> seeds;
    for (int i = 0; i < NUM_TEST_CASES; ++i) {
        seeds.push_back(seedGen());
    }

    vector<TestSummary> summaries;

    // 2. Mỗi test case dùng 1 seed riêng để sinh process
    for (int tc = 0; tc < NUM_TEST_CASES; ++tc) {
        cout << "====================================================\n";
        cout << "            TEST CASE #" << (tc + 1) << " (Seed: " << seeds[tc] << ")\n";
        cout << "====================================================\n";

        scheduler sim(PROCESS_COUNT, QUANTUM, seeds[tc]);
        sim.runRR();
        sim.printGanttChart();
        sim.printResultTable();

        summaries.push_back({
            tc + 1,
            QUANTUM,
            seeds[tc],
            sim.averageResponse(),
            sim.averageTaT(),
            sim.averageWaitTime()
        });

        cout << "\n";
    }

    cout << "====================================================\n";
    cout << "           SUMMARY COMPARISON (3 TEST CASES)\n";
    cout << "====================================================\n";
    cout << "Case\tSeed\t\tQuantum\tAvg Resp\tAvg TaT\t\tAvg Wait\n";
    cout << "----------------------------------------------------\n";
    cout << fixed << setprecision(2);
    for (const auto& s : summaries) {
        cout << "#" << s.caseId << "\t"
             << s.seed << "\t"
             << s.quantum << "\t"
             << s.avgResponse << "\t\t"
             << s.avgTaT << "\t\t"
             << s.avgWait << "\n";
    }
    cout << "====================================================\n";

    return 0;
}