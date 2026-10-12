#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>
#include <algorithm>
#include <string>

using namespace std;

using Matrix = vector<vector<int>>;
using ResultMatrix = vector<vector<long long>>;

ResultMatrix multiply(const Matrix &A, const Matrix &B, int n)
{
    ResultMatrix C(n, vector<long long>(n, 0));

    for (int i = 0; i < n; i++)
    {
        for (int k = 0; k < n; k++)
        {
            for (int j = 0; j < n; j++)
            {
                C[i][j] +=
                    static_cast<long long>(A[i][k]) * B[k][j];
            }
        }
    }

    return C;
}

ResultMatrix multiply_parallel(
    const Matrix &A,
    const Matrix &B,
    int n,
    int thread_count)
{
    ResultMatrix C(n, vector<long long>(n, 0));
    vector<thread> threads;

    int rows_per_thread = n / thread_count;
    int remaining_rows = n % thread_count;
    int start_row = 0;

    for (int t = 0; t < thread_count; t++)
    {
        int rows = rows_per_thread + (t < remaining_rows ? 1 : 0);
        int end_row = start_row + rows;

        threads.emplace_back(
            [&A, &B, &C, n, start_row, end_row]()
            {
                for (int i = start_row; i < end_row; i++)
                {
                    for (int k = 0; k < n; k++)
                    {
                        for (int j = 0; j < n; j++)
                        {
                            C[i][j] +=
                                static_cast<long long>(A[i][k]) * B[k][j];
                        }
                    }
                }
            });

        start_row = end_row;
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return C;
}

int main(int argc, char *argv[])
{
    int thread_count = 4;

    if (argc > 2)
    {
        cerr << "Usage: ./build/matrix [thread_count]\n";
        return 1;
    }

    if (argc == 2)
    {
        try
        {
            size_t pos = 0;
            thread_count = stoi(argv[1], &pos);

            if (pos != string(argv[1]).length())
            {
                cerr << "Invalid thread count\n";
                return 1;
            }
        }
        catch (const exception &)
        {
            cerr << "Invalid thread count\n";
            return 1;
        }
    }

    if (thread_count <= 0)
    {
        cerr << "Thread count must be positive\n";
        return 1;
    }

    ifstream inA("A.txt");
    ifstream inB("B.txt");

    if (!inA || !inB)
    {
        cerr << "Cannot open input files\n";
        return 1;
    }

    int nA, nB;

    if (!(inA >> nA) || !(inB >> nB) || nA <= 0 || nA != nB)
    {
        cerr << "Invalid matrix sizes\n";
        return 1;
    }

    int n = nA;
    thread_count = min(thread_count, n);

    Matrix A(n, vector<int>(n));
    Matrix B(n, vector<int>(n));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (!(inA >> A[i][j]) || !(inB >> B[i][j]))
            {
                cerr << "Cannot read matrix elements\n";
                return 1;
            }
        }
    }

    auto start = chrono::steady_clock::now();

    ResultMatrix C = multiply(A, B, n);

    auto finish = chrono::steady_clock::now();

    double sequential_time =
        chrono::duration<double, milli>(finish - start).count();

    start = chrono::steady_clock::now();

    ResultMatrix parallel_C =
        multiply_parallel(A, B, n, thread_count);

    finish = chrono::steady_clock::now();

    double parallel_time =
        chrono::duration<double, milli>(finish - start).count();

    if (C != parallel_C)
    {
        cerr << "Error: results do not match\n";
        return 1;
    }

    ofstream out("C.txt");

    if (!out)
    {
        cerr << "Cannot create output file\n";
        return 1;
    }

    out << n << '\n';

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            out << parallel_C[i][j] << ' ';
        }

        out << '\n';
    }

    cout << "Matrix size: " << n << "x" << n << '\n';
    cout << "Thread count: " << thread_count << '\n';

    cout << fixed << setprecision(3);

    cout << "Sequential time: "
         << sequential_time << " ms\n";

    cout << "Parallel time: "
         << parallel_time << " ms\n";

    cout << "Results match: yes\n";

    if (parallel_time > 0.0)
    {
        cout << "Speedup: "
             << sequential_time / parallel_time << "x\n";
    }

    return 0;
}