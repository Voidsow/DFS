#pragma once
#include"nfa.h"
#include<map>

using std::map;


class DFA
{
	int start = 0;
	set<int> end;
	//对于每一个接受字符对应唯一的转移状态，状态依然通过索引获得;不接受的字符转换到索引-1（不存在这样的状态）
	vector<map<char, int>> table;
	vector<char>alphabet;

public:

	//子集构造法
	DFA(NFA& nfa, vector<char>alphaBet)
		:alphabet(alphaBet)
	{

		map< vector<int>, int > stateMap;
		//met表示已经标记过的状态集合
		set<vector<int>> met;
		set<vector<int>> toHandle;
		auto tmp = nfa.epsilonClosure(nfa.start);
		auto tmpStart = nfa.epsilonClosure(tmp);
		stateMap[tmpStart] = 0;
		toHandle.insert(tmpStart);
		table.push_back(map<char, int>());
		while (!toHandle.empty())
		{
			auto it = toHandle.begin();
			for (auto& I : *it)
				printf("%d", I);
			int cur = stateMap[*it];
			met.insert(*it);

			//对于每个输入符号
			for (auto a : alphabet)
			{
				auto to = nfa.moveToWithinEpsilon(*it, a);
				if (!to.empty())
				{
					//记录转换关系
					auto moveToTrans = stateMap.find(to);
					//新的状态
					if (moveToTrans == stateMap.end())
					{
						stateMap[to] = stateMap.size();
						table[cur][a] = table.size();
						//标记是否为终态（可接受状态）
						for (const auto& s : to)
						{
							if (s == nfa.end)
								end.insert(table.size());
						}
						table.push_back(map<char, int>());
					}
					else
						table[cur][a] = (*moveToTrans).second;

					//若状态集to未标记过，将to加入到toHandle待处理集中
					if (met.find(to) == met.end())
					{
						toHandle.insert(to);
					}
				}
				else
					table[cur][a] = -1;
			}
			toHandle.erase(it);
		}
	}

	//最小化
	void minimize()
	{
		//分组的唯一标识
		int id = 0;
		//状态索引映射到分组索引
		map<int, int> stateToDstrbt;
		std::map<int, set<int>> dstrbt;
		dstrbt[0] = set<int>();
		dstrbt[1] = set<int>();
		//初始时将非终态和终态划分到两个集合
		for (size_t i = 0; i < table.size(); i++)
		{
			if (end.find(i) == end.end())
			{
				stateToDstrbt[i] = 0;
				dstrbt[0].insert(i);
			}
			else
			{
				stateToDstrbt[i] = 1;
				dstrbt[1].insert(i);
			}
		}
		id = 2;
		bool changed = true;
		while (changed)
		{
			changed = false;
			for (const auto& c : alphabet)
			{
				for (auto it = dstrbt.begin(); it != dstrbt.end(); it++)
				{
					//对于当前输入符号有相同转换的状态同处一个集合
					//map<转换到的分组标识，set<当前分组的预划分>>,value是状态（索引）的集合
					map<int, set<int>> cur;
					for (auto i : (*it).second)
						cur[stateToDstrbt[table[i][c]]].insert(i);
					//划分有变动
					if (cur.size() > 1)
					{
						changed = true;
						(*it).second = set<int>(cur.begin()->second);
						for (auto it = ++cur.begin(); it != cur.end(); it++)
						{
							for (auto i : it->second)
								stateToDstrbt[i] = id;
							dstrbt[id++] = set<int>(it->second);
						}
					}
				}
			}
		}
		vector<map<char, int>> newTable;
		map<int, int> idToNewId;
		for (const auto& section : dstrbt)
		{
			idToNewId[section.first] = newTable.size();
			newTable.push_back(map<char, int>());
		}
		for (const char& c : alphabet)
		{
			for (auto& section : dstrbt)
			{
				int first = *(section.second.begin());
				newTable[idToNewId[stateToDstrbt[first]]][c] = idToNewId[stateToDstrbt[table[first][c]]];
			}
		}
		table = newTable;
		start = idToNewId[stateToDstrbt[start]];
		set<int> newEnd;
		for (const auto& i : end)
			newEnd.insert(idToNewId[stateToDstrbt[i]]);
		end = newEnd;
	}

	bool accept(){}
};
