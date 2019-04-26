#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <Windows.h>

using namespace std;

// Чтобы было сложнее разобраться в типах :)  (Для сокращения)
template<class Key, class Value>
using um = unordered_map<Key, Value>;
typedef um<string, vector<string>> s_graph;

const string NOT_FOUND = "No such word found in dictionary";

// Выбрать из словаря слова только заданной длины
vector<string> filter_dict(vector<string> dict, int length)
{
	vector<string> filtered(dict.size());
	auto it = copy_if(dict.begin(), dict.end(), filtered.begin(), [=](string w) { return w.length() == length; });
	filtered.resize(distance(filtered.begin(), it));
	return filtered;
}

// Являются ли слова соседними по одной букве
bool neighbours(string f, string s)
{
	int dist = 0;
	for (size_t i = 0; i < f.length(); i++)
		if (f[i] != s[i])
			if (++dist > 1)
				break;
	return dist == 1;
}

// Построение графа с ребрами, соединяющими слова, которые отличаются на одну букву
s_graph build_graph(vector<string> dict)
{
	s_graph graph;
	for (size_t i = 0; i < dict.size(); i++)
	{
		for (size_t j = i + 1; j < dict.size(); j++)
		{
			if (neighbours(dict[i], dict[j]))
			{
				graph[dict[i]].push_back(dict[j]);
				graph[dict[j]].push_back(dict[i]);
			}
		}
	}
	return graph;
}

// Поиск пути между словами в ширину
vector<string> bfs(s_graph graph, string src, string dest)
{
	um<string, bool> used;
	um<string, string> parent;
	queue<string> q;

	used[src] = true;
	parent[src] = src;
	q.push(src);

	bool found = false;

	while (!q.empty() && !found)
	{
		string w = q.front();
		q.pop();
		for (size_t i = 0; i < graph[w].size(); i++)
		{
			string v = graph[w][i];
			if (!used[v])
			{
				used[v] = true;
				parent[v] = w;
				q.push(v);
			}
			if (v == dest)
			{
				found = true;
				break;
			}
		}
	}

	vector<string> path;

	if (found)
	{
		string w = dest;
		do {
			path.insert(path.begin(), w);
			w = parent[w];
		} while (w != src);
		path.insert(path.begin(), w);
	}

	return path;
}

bool need_continue()
{
	char reply;

	bool firstTry = true;
	do
	{
		if (!firstTry)
		{
			while (cin.get() != '\n') {}
			cout << "Incorrect reply" << endl;
		}

		cout << "Continue? (Y/N)> ";
		cin >> reply;

		firstTry = false;
	} while (cin.peek() != '\n' || reply != 'n' && reply != 'N' && reply != 'y' && reply != 'Y');

	return reply == 'y' || reply == 'Y';
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	cout << "___ Word Morph ___" << endl;

	ifstream dictfile;
	do {
		string name;
		cout << "Input a dictionary filename> ";
		cin >> name;
		dictfile.clear();
		dictfile.open(name);

		if (!dictfile)
			cout << "Can't open dictionary file" << endl;
	} while (!dictfile);

	vector<string> dict;
	while (dictfile.good()) {
		string word;
		dictfile >> word;
		dict.push_back(word);
	}

	do
	{
		string src, dest;
		do {
			cout << "Input a source word> ";
			cin >> src;
			transform(src.begin(), src.end(), src.begin(), tolower);

			if (find(dict.begin(), dict.end(), src) == dict.end())
			{
				cout << NOT_FOUND << endl;
				src = "";
			}
		} while (src.length() == 0);
		do {
			cout << "Input a destination word> ";
			cin >> dest;
			transform(dest.begin(), dest.end(), dest.begin(), tolower);

			if (find(dict.begin(), dict.end(), dest) == dict.end())
			{
				cout << NOT_FOUND << endl;
				dest = "";
			}
			else if (dest.length() != src.length())
			{
				cout << "Destiantion word must be the same length as source word" << endl;
				dest = "";
			}
			else if (src == dest)
			{
				cout << "Source and destination words can't be the same" << endl;
				dest = "";
			}
		} while (dest.length() == 0);

		vector<string> field = filter_dict(dict, src.length());
		s_graph graph = build_graph(field);

		vector<string> path = bfs(graph, src, dest);

		if (path.size() > 0)
		{
			cout << src;
			for (size_t i = 1; i < path.size(); i++)
			{
				cout << " -> " << path[i];
			}
			cout << endl;
		}
		else
			cout << "No solution" << endl;
	} while (need_continue());

	return 0;
}