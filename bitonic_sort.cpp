#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
using namespace std;
using namespace chrono;
//bitonic sort implementation

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
/*******************************************************************************************/
void BitonicMergeRecursive(vector<int>& data, int startIndex, int count, bool up);
void BitonicSortRecursive(vector<int>& data, int startIndex, int count, bool up);
void BitonicSortRecursiveMulThread(vector<int>& data, int startIndex, int count, bool up);
/*******************************************************************************************/
void BitonicMergeIterative(vector<int>& data, int start, int len, bool up);
void BitonicSortIterative(vector<int>& data, int start, int len, bool up);
void BitonicSortIterativeMulthread(vector<int>& data, int start, int len, bool up);
/*******************************************************************************************/
void check(vector<int>& data, bool up);

int main() {
	//4194304
	//1048576
	int n{ 4194304 * 2 }, m{ 100 };//different number will be used during grading
	vector<int> Data(n);
	cout << "Time of Recursive:" << endl;
	cout << "One thread:" << endl;
	for (auto& i : Data) i = rand() % m;

	{
		Timer t1;
		BitonicSortRecursive(Data, 0, Data.size(), 1);

	}
	check(Data, 1);
	cout << "Four thread:" << endl;
	for (auto& i : Data) i = rand() % m;
	{
		Timer t1;
		BitonicSortRecursiveMulThread(Data, 0, Data.size(), 1);

	}
	check(Data, 1);
	ofstream Output1;
	Output1.open("Output1.txt");
	if (Output1.is_open())
	{
		for (int i = 0; i < Data.size(); ++i)
		{
			Output1 << Data[i] << " ";
			if (i % 50 == 0) Output1 << '\n';
		}
		Output1.close();
	}
	else
	{
		cout << "Unable to open file for writing." << endl;
	}


	cout << "Time of Iterative :" << endl;
	cout << "One thread:" << endl;
	for (auto& i : Data) i = rand() % m;
	{
		Timer t2;
		BitonicSortIterative(Data, 0, Data.size(), 0);
	}
	check(Data, 0);
	cout << "Four thread:" << endl;
	for (auto& i : Data) i = rand() % m;
	{
		Timer t2;
		BitonicSortIterativeMulthread(Data, 0, Data.size(), 0);
	}
	check(Data, 0);
	//Save the result to "Output2.txt".
	ofstream Output2;
	Output2.open("Output2.txt");
	if (Output2.is_open())
	{
		for (int i = 0; i < Data.size(); ++i)
		{
			Output2 << Data[i] << " ";
			if (i % 50 == 0) Output2 << '\n';
		}
		Output2.close();
	}
	else
	{
		cout << "Unable to open file for writing." << endl;
	}

	return 0;
}


/********************************************************************************/
void BitonicMergeRecursive(vector<int>& data, int startIndex, int count, bool up)
{
	if (count == 1) return;
	int m = count >> 1;
	if (count > 1)
	{

		for (int i = startIndex; i < startIndex + m; i++)
		{
			if (up == true)
			{
				if (data[i] > data[i + m])
				{
					swap(data[i], data[i + m]);
				}
			}
			else if (up == false)
			{
				if (data[i] < data[i + m]) swap(data[i], data[i + m]);
			}
		}
	}
	BitonicMergeRecursive(data, startIndex, m, up);
	BitonicMergeRecursive(data, startIndex + m, m, up);

}
void BitonicSortRecursive(vector<int>& data, int startIndex, int count, bool up)
{
	if (count == 1) return;

	if (count > 1)
	{
		int m = count >> 1;
		BitonicSortRecursive(data, startIndex, m, true);
		BitonicSortRecursive(data, startIndex + m, m, false);
		BitonicMergeRecursive(data, startIndex, count, up);

	}

}

void BitonicSortRecursiveMulThread(vector<int>& data, int startIndex, int count, bool up)
{

	int len = count >> 2;
	thread T[3];
	bool flag = up;
	for (int i = 0; i < 3; i++)
	{
		T[i] = thread(BitonicSortRecursive, ref(data), len * i, len, flag);
		flag = !flag;
	}
	BitonicSortRecursive(data, 3 * len, len, flag);
	for (int i = 0; i < 3; i++)
	{
		T[i].join();
	}

	thread t1{ BitonicMergeRecursive,ref(data),0,count / 2,up };
	BitonicMergeRecursive(data, count / 2, count / 2, !up);
	t1.join();
	BitonicMergeRecursive(data, 0, count, up);
}
/********************************************************************************/
void BitonicMergeIterative(vector<int>& data, int start, int len, bool up)
{
	int step = len;

	while (step != 0)
	{
		int s = start;
		while (s + step < start + len)
		{
			for (int i = s; i < s + step; i++)
			{

				if (up == true)
				{
					if (data[i] > data[i + step])
					{
						swap(data[i], data[i + step]);
					}
				}
				else if (up == false)
				{
					if (data[i] < data[i + step]) swap(data[i], data[i + step]);
				}
			}
			s = s + step;
		}
		step = step >> 1;
	}

}
void BitonicSortIterative(vector<int>& data, int start, int len, bool up)
{
	int step = 2;
	while (step <= len)
	{
		bool flag = up;
		for (int i = start; i < (start + len); i = i + step)
		{
			BitonicMergeIterative(data, i, step, flag);
			flag = !flag;
		}
		step <<= 1;
	}

}
void BitonicSortIterativeMulthread(vector<int>& data, int start, int len, bool up)
{
	int step = 2;
	int length = len >> 2;
	thread T[3];
	bool flag = up;
	for (int i = 0; i < 3; i++)
	{
		T[i] = thread(BitonicSortIterative, ref(data), i * length, length, flag);
		flag = !flag;
	}
	BitonicSortIterative(data, length * 3, length, !up);
	for (int i = 0; i < 3; i++)
	{
		T[i].join();
	}

	thread t{ BitonicMergeIterative,ref(data),0,len / 2,up };
	BitonicMergeIterative(data, len / 2, len / 2, !up);
	t.join();
	BitonicMergeIterative(data, 0, len, up);

}

void check(vector<int>& data, bool up) {
	if (up == 0)
	{
		for (int i = 1; i < data.size(); ++i)
		{
			if (data[i] > data[i - 1])
			{
				cout << "Fail the test" << endl;
				return;
			}
		}

	}
	else
	{
		for (int i = 1; i < data.size(); ++i)
		{
			if (data[i] < data[i - 1])
			{
				cout << "Fail the test" << endl;
				return;
			}
		}
	}
	cout << "Pass the test" << endl;
	return;
}