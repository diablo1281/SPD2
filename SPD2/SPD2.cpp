// SPD2.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Task
{
public:
	int p, w, d, task_nr;

	Task()
		: p(0)
		, w(0)
		, d(0)
		, task_nr(0)
	{}

	Task(int xp, int xw, int xd, int xnr)
		: p(xp)
		, w(xw)
		, d(xd)
		, task_nr(xnr)
	{}

	~Task() {}

	friend ostream &operator << (ostream & stream, Task const& out);
};

ostream &operator << (ostream & stream, Task const& out)		//Przeci¹¿enie operatora lewostronnego strumienia dla klasy Task
{
	stream << out.task_nr << "\t" << out.p << " " << out.w << " " << out.d << endl;
	return stream;
}

bool load_file(vector<Task> &task_vector, string filename, string select_position)
{
	fstream file;
	file.open(filename, ios::in);

	if (!file.is_open())
	{
		cerr << "Error: Couldn't open file " << filename << endl;
		return false;
	}

	string position = "";
	select_position += ":";
	
	while (!(position == select_position))
		getline(file, position);

	int N = 0;
	
	file >> N;

	for (int i = 0; i < N; i++)
	{
		Task tmp;
		file >> tmp.p >> tmp.w >> tmp.d;
		tmp.task_nr = i + 1;
		task_vector.push_back(tmp);
	}

	return true;
}

vector<Task>::iterator find_task_nr(vector<Task>::iterator iter_begin, vector<Task>::iterator iter_end, int task_nr)
{
	for (;iter_begin != iter_end; iter_begin++)
		if (iter_begin->task_nr == task_nr)
			return iter_begin;

	return iter_end;
}

int CMAX(vector<Task> queue)
{
	int cmax = 0;
	for (auto iter = queue.begin(); iter != queue.end(); iter++)
		cmax += iter->p;

	return cmax;
}

int penalty_for_task(vector<Task>::iterator task, int cmax)
{ 
	return max(0, (cmax - task->d) * task->w); 
}

int total_penalty(vector<Task> queue)
{
	int cmax = 0;
	int penalty = 0;
	for (auto iter = queue.begin(); iter != queue.end(); iter++)
	{
		cmax += iter->p;
		penalty += penalty_for_task(iter, cmax);
	}
	return penalty;
}

void all_permutation(vector<Task> task_vector, vector<Task> &best_permutation, int &opt_penalty)
{
	
	int N = (int)pow(2, task_vector.size());
	vector<Task> tmp;
	vector<int> penalty_list(N, INT_MAX);
	vector<vector<Task>> permutation(N, tmp);
	vector<int> cmax(N, 0);

	penalty_list[0] = 0;
	
	for (uint32_t mask = 0; mask < (1 << task_vector.size()); mask++)
	{
		
		for (uint32_t i = 0; mask >= (1 << i); i++)
		{
			if (mask & (1 << i))
			{
				uint32_t b = mask ^ (1 << i);

				if (penalty_list[mask] >(penalty_list[b] + penalty_for_task(task_vector.begin() + i, cmax[b] + (task_vector.begin() + i)->p)))
				{
					penalty_list[mask] = (penalty_list[b] + penalty_for_task(task_vector.begin() + i, cmax[b] + (task_vector.begin() + i)->p));

					tmp.clear();
					tmp = permutation[b];
					tmp.push_back(task_vector[i]);

					permutation[mask] = tmp;

					cmax[mask] = CMAX(permutation[mask]);
				}
			}
		}
		
	}
	
	best_permutation = permutation.back();
	opt_penalty = penalty_list.back();


}

int main()
{
	vector<Task> best_permutation;
	vector<Task> task_vector;
	string filename = "data.txt";
	string position = "data.";
	clock_t all_start = clock(), all_end;

	for (int i = 10; i <= 20; i++)
	{
		if (!load_file(task_vector, filename, position + to_string(i)))
		{
			cerr << "Error reading: " << position + to_string(i) << endl;
			continue;
		}
				
		int penalty = 0;
		all_permutation(task_vector, best_permutation, penalty);
		
		cout << position + to_string(i) << endl << endl;

		cout << "Opt: " << penalty << endl;

		for (auto iter : best_permutation)
			cout << iter.task_nr << " ";

		cout << endl << endl;

		task_vector.clear();
		best_permutation.clear();
	}
	all_end = clock();
	cout << "Total time: " << double(all_end - all_start) / CLOCKS_PER_SEC << "s" << endl;

	system("pause");
    return 0;
}

