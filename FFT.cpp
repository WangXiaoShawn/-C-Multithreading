// Fast Fourier Transform
#include <iostream>
#include <complex>
#include <math.h>
#include <vector>
#include <fstream>
#include <thread>
using namespace std;
typedef complex<double> cx;
using namespace chrono;
const double PI = 3.1415926536;
const cx J(0, 1);
class Timer {
public:
    system_clock::time_point Begin, End;
    Timer() { Begin = system_clock::now(); }
    ~Timer() {
        End = system_clock::now();
        system_clock::duration Runtime{ End - Begin };
        cout << "Runtime = " << duration_cast<milliseconds>(Runtime).count() << " ms" << "\n";
    }

};
int BitReverse(unsigned int num, int log2n);
void myfun(vector<cx>& Reordered, int start, int interval, cx curr, cx step);
void RecursiveFFT(vector<cx>& Reordered, int len, int start);
void IterationFFT(vector<cx>& Reordered);
void MtulThreadIterationFFT(vector<cx>& Reordered);
void MtulThreadRecursiveFFT(vector<cx>& Reordered, int len, int start);

int main()
{
/*multhread MtulThreadRecursiveFFT*/
/****************************************************************************************************/
    vector<cx> a;
    ifstream In("input1024.txt");//Use the right file name described in HW2 
    if (!In) {
        cout << "Fail to open file" << endl;
        return 0;
    }
    double d1;
    while (In >> d1) {
        a.emplace_back(cx(d1, 0));
    }
    In.close();

    vector<cx> Rdata,Idata,Ronethread, IoneThread;
/*bit reverse &  data preperation*/
    int log2n = log2(a.size());

    for (int i = 0; i < a.size(); i++)
    {
        Rdata.emplace_back(a[BitReverse(i, log2n)]);
        Idata.emplace_back(a[BitReverse(i, log2n)]);
        Ronethread.emplace_back(a[BitReverse(i, log2n)]);
        IoneThread.emplace_back(a[BitReverse(i, log2n)]);

    }

    {
        Timer T;
        RecursiveFFT(Ronethread, Ronethread.size(), 0);

    }
    {
        Timer t;
        MtulThreadRecursiveFFT(Rdata, Rdata.size(), 0);
    }

    for (auto it : Rdata)
    {
        //cout << it << endl;
    }

    ofstream Out("recursive_output_wangxiao1024.txt");//Use the right file name described in HW2
    if (!Out) {

        cout << "Fail to access file ...." << endl;
    }


    for (int i = 0; i < a.size(); ++i)
        Out << Rdata[i] << "\n";

    Out.close();
 /****************************************************************************************************/
    {
        Timer T;
        IterationFFT(IoneThread);

    }
    {
        Timer t;
        MtulThreadIterationFFT(Idata);
    }

   /* for (auto it : Idata)
    {
        cout << it << endl;
    }*/

    ofstream Out1("Iteration_output_wangxiao1024.txt");//Use the right file name described in HW2
    if (!Out1) {

        cout << "Fail to access file ...." << endl;
    }


    for (int i = 0; i < a.size(); ++i)
        Out1 << Idata[i] << "\n";

    Out1.close();

    return 0;
}


int BitReverse(unsigned int num, int log2n)
{
    int output = 0;
    for (int i = 0; i < log2n; i++)
    {
        output <<= 1;
        output |= (num & 1);
        num >>= 1;
    }
    return output;
}
void myfun(vector<cx>& Reordered, int start, int interval, cx curr, cx step)
{

    for (int i = start; i < start + interval; i++)
    {
        cx even = Reordered[i];
        cx odd = Reordered[i + interval];

        Reordered[i] = even + curr * odd;
        Reordered[i + interval] = even - curr * odd;

        curr *= step;
    }
}
void RecursiveFFT(vector<cx>& Reordered, int len, int start)
{
    if (len == 1) return;
    int interval = len >> 1;
    int evenStart = start;
    int oddStart = start + interval;
    RecursiveFFT(Reordered, interval, evenStart);
    RecursiveFFT(Reordered, interval, oddStart);
    cx curr, step;
    curr = cx(1, 0);
    step = exp(cx(-2) * J * (PI / len));
    myfun(Reordered, start, interval, curr, step);

}

void MtulThreadRecursiveFFT(vector<cx>& Reordered, int len, int start)
{
    if (len == 1) return;
    int interval = len >> 1;
    int evenStart = start;
    int oddStart = start + interval;
    RecursiveFFT(Reordered, interval, evenStart);
    RecursiveFFT(Reordered, interval, oddStart);
    cx curr, step;
    curr = cx(1, 0);
    step = exp(cx(-2) * J * (PI / len));
    if (len == Reordered.size() / 4)
    {
        int gap = len / 4;
        int interval1 = gap / 2;
        thread t1(myfun, ref(Reordered), start, interval1, curr, step);
        thread t2(myfun, ref(Reordered), start + gap, interval1, curr, step);
        thread t3(myfun, ref(Reordered), start + 2 * gap, interval1, curr, step);
        myfun(Reordered, start + 3 * gap, interval1, curr, step);
        t1.join();
        t2.join();
        t3.join();

    }
    if (len == Reordered.size() / 2)
    {
        int gap = len / 2;
        int interval1 = gap / 2;
        thread t(myfun, ref(Reordered), start, interval1, curr, step);
        myfun(Reordered, start + gap, interval1, curr, step);
        t.join();
    }

    else
    {
        myfun(Reordered, start, interval, curr, step);
    }
}

void IterationFFT(vector<cx>& Reordered)
{


    for (int len = 2; len <= Reordered.size(); len *= 2)
    {

        cx step = exp(cx(-2) * J * (PI / len));

        for (int i = 0; i < Reordered.size(); i += len)
        {
            cx curr = cx(1, 0);
            int interval = len >> 1;
            myfun(Reordered, i, interval, curr, step);

        }

    }
}
void MtulThreadIterationFFT(vector<cx>& Reordered)
{
    int threshold = Reordered.size() >> 2;
    for (int len = 2; len <= Reordered.size(); len *= 2)
    {
        if (len == threshold) break;

        cx step = exp(cx(-2) * J * (PI / len));

        for (int i = 0; i < Reordered.size(); i += len)
        {
            cx curr = cx(1, 0);
            int interval = len >> 1;
            myfun(Reordered, i, interval, curr, step);

        }

    }
    cx step1 = exp(cx(-2) * J * (PI / threshold));
    cx curr1 = cx(1, 0);
    int interval1 = threshold >> 1;
    thread t1 = thread(myfun, ref(Reordered), 0, interval1, curr1, step1);
    thread t2 = thread(myfun, ref(Reordered), threshold, interval1, curr1, step1);
    thread t3 = thread(myfun, ref(Reordered), 2 * threshold, interval1, curr1, step1);
    myfun(Reordered, 3 * threshold, interval1, curr1, step1);
    t1.join();
    t2.join();
    t3.join();

    int len2 = threshold * 2;
    cx step2 = exp(cx(-2) * J * (PI / len2));
    cx curr2 = cx(1, 0);
    int interval2 = len2 >> 1;
    thread t = thread(myfun, ref(Reordered), 0, interval2, curr2, step2);
    myfun(Reordered, len2, interval2, curr2, step2);
    t.join();

    int len3 = Reordered.size();
    cx step3 = exp(cx(-2) * J * (PI / len3));
    cx curr3 = cx(1, 0);
    int interval3 = len3 >> 1;
    myfun(Reordered, 0, interval3, curr3, step3);

}