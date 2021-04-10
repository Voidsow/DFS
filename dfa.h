#pragma once
#include"nfa.h"
#include<map>

using std::map;


class DFA
{
	int start = 0;
	set<int> end;
	//����ÿһ�������ַ���ӦΨһ��ת��״̬��״̬��Ȼͨ���������;�����ܵ��ַ�ת��������-1��������������״̬��
	vector<map<char, int>> table;
	vector<char>alphabet;

public:

	//�Ӽ����취
	DFA(NFA& nfa, vector<char>alphaBet)
		:alphabet(alphaBet)
	{

		map< vector<int>, int > stateMap;
		//met��ʾ�Ѿ���ǹ���״̬����
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

			//����ÿ���������
			for (auto a : alphabet)
			{
				auto to = nfa.moveToWithinEpsilon(*it, a);
				if (!to.empty())
				{
					//��¼ת����ϵ
					auto moveToTrans = stateMap.find(to);
					//�µ�״̬
					if (moveToTrans == stateMap.end())
					{
						stateMap[to] = stateMap.size();
						table[cur][a] = table.size();
						//����Ƿ�Ϊ��̬���ɽ���״̬��
						for (const auto& s : to)
						{
							if (s == nfa.end)
								end.insert(table.size());
						}
						table.push_back(map<char, int>());
					}
					else
						table[cur][a] = (*moveToTrans).second;

					//��״̬��toδ��ǹ�����to���뵽toHandle��������
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

	//��С��
	void minimize()
	{
		//�����Ψһ��ʶ
		int id = 0;
		//״̬����ӳ�䵽��������
		map<int, int> stateToDstrbt;
		std::map<int, set<int>> dstrbt;
		dstrbt[0] = set<int>();
		dstrbt[1] = set<int>();
		//��ʼʱ������̬����̬���ֵ���������
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
					//���ڵ�ǰ�����������ͬת����״̬ͬ��һ������
					//map<ת�����ķ����ʶ��set<��ǰ�����Ԥ����>>,value��״̬���������ļ���
					map<int, set<int>> cur;
					for (auto i : (*it).second)
						cur[stateToDstrbt[table[i][c]]].insert(i);
					//�����б䶯
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
