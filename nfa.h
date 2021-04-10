#pragma once
#include<vector>
#include<string>
#include<set>

#define LEFT_BRACKET (-1)
#define VERTICAL (-2)

using std::vector;
using std::set;



struct NFAstate
{
	int index;
	int epsilon = -1;//-1��ʾ�����ܿմ�
	//����ÿһ��������ţ�����һ��ת��״̬���϶�Ӧ,״̬ͨ��������ȡ
	vector<std::pair<char, vector<int>>> edge = vector<std::pair<char, vector<int> > >();

	NFAstate(int i) :index(i) {}

	//���ת�Ʊ�
	void addMove(char c, int stateIndex)
	{
		for (auto& i : edge)
			if (i.first == c)
			{
				i.second.push_back(stateIndex);
				return;
			}
		edge.push_back(std::pair<char, vector<int>>(c, vector<int>()));
		edge.back().second.push_back(stateIndex);
		if (c == 0)
			epsilon = edge.size() - 1;
	}

	//ͨ������c�ƶ�����״̬����
	vector<int> move(char c)
	{
		for (auto& i : edge)
			if (i.first == c)
				return i.second;
		return vector<int>();
	}

	//��ȡepsilon�հ�
	vector<int> epsilonClosure()
	{
		if (epsilon != -1)
		{
			vector<int> tmp = vector<int>(edge[epsilon].second);
			tmp.push_back(index);
			return tmp;
		}
		else
			return vector<int>(index);
	}
};

class NFA
{
public:

	int start=0;
	int end;
	vector<NFAstate> stateSet;

	void connect(vector<int>& begin, vector<int>& fin)
	{
		int rightBegin = begin.back(), End = fin.back();
		begin.pop_back();
		fin.pop_back();
		//����ֹ����һ��������ķ��Ŵ���������
		while ((!begin.empty()) && (begin.back() != LEFT_BRACKET) && begin.back() != VERTICAL)
		{
			stateSet[fin.back()].addMove(0, rightBegin);
			fin.pop_back();
			rightBegin = begin.back();
			begin.pop_back();
		}
		begin.push_back(rightBegin);
		fin.push_back(End);
	}

	void Union(vector<int>& begin, vector<int>& fin)
	{
		int tmpBegin = stateSet.size();
		stateSet.push_back(NFAstate(stateSet.size()));
		int tmpEnd = stateSet.size();
		stateSet.push_back(NFAstate(stateSet.size()));

		//�ϲ����ܴ��ڵ��ұߵķ��Ŵ�
		connect(begin, fin);
		int tmpRightBegin = begin.back();
		int tmpRightEnd = fin.back();
		//�����ұߵĺϲ�������
		begin.pop_back();
		fin.pop_back();
		//��������begin��end�д��ڵ�'|'
		begin.pop_back();
		fin.pop_back();
		//������ߵķ��Ŵ�
		connect(begin, fin);
		//���ת�Ʊ�
		stateSet[tmpBegin].addMove(0, tmpRightBegin);
		stateSet[tmpBegin].addMove(0, begin.back());
		stateSet[tmpRightEnd].addMove(0, tmpEnd);
		stateSet[fin.back()].addMove(0, tmpEnd);
		//������ߵĲ�����
		begin.pop_back();
		fin.pop_back();
		//���µĿ�ʼ����ֹ״̬���뵽ջ
		begin.push_back(tmpBegin);
		fin.push_back(tmpEnd);
	}

	void star(vector<int>& begin, vector<int>& fin)
	{
		int tmpBegin = stateSet.size();
		stateSet.push_back(NFAstate(stateSet.size()));
		int tmpEnd = stateSet.size();
		stateSet.push_back(NFAstate(stateSet.size()));

		stateSet[fin.back()].addMove(0, begin.back());
		stateSet[fin.back()].addMove(0, tmpEnd);
		printf("%d", begin.back());
		stateSet[tmpBegin].addMove(0, begin.back());
		stateSet[tmpBegin].addMove(0, tmpEnd);
		begin.pop_back();
		begin.push_back(tmpBegin);
		fin.pop_back();
		fin.push_back(tmpEnd);
	}

	NFA() {}

	void buildNFA(const std::string& regex)
	{
		vector<int> begin;
		vector<int> fin;
		bool u = false;
		for (size_t i = 0; i < regex.length(); i++)
		{
			char c = regex[i];
			switch (c)
			{
			case '(':
				begin.push_back(LEFT_BRACKET);
				fin.push_back(LEFT_BRACKET);
				break;
			case ')':
				if (u)
				{
					Union(begin, fin);
					u = false;
				}
				else
					connect(begin, fin);
				begin[begin.size() - 2] = begin.back();
				begin.pop_back();
				fin[fin.size() - 2] = fin.back();
				fin.pop_back();
				break;
			case '*':
				star(begin, fin);
				break;

				//������һ��|����(���ߵ���ĩβʱ��UnionΪ��ϲ�
			case '|':
				if (u)
					Union(begin, fin);
				u = true;
				begin.push_back(VERTICAL);
				fin.push_back(VERTICAL);
				break;
			case '\\'://ת����ƥ��������ʽʹ�õ����������
				if (i < regex.length() - 1)
				{
					switch (regex[i + 1])
					{
					case '*':case '|':case '(':case')':
						begin.push_back(stateSet.size());
						stateSet.push_back(NFAstate(stateSet.size()));
						stateSet.back().addMove(regex[i + 1], stateSet.size());
						fin.push_back(stateSet.size());
						stateSet.push_back(NFAstate(stateSet.size()));
						i++;
						break;
					default:
						break;
					}
					break;
				}
			default:
				begin.push_back(stateSet.size());
				stateSet.push_back(NFAstate(stateSet.size()));
				stateSet.back().addMove(c, stateSet.size());
				fin.push_back(stateSet.size());
				stateSet.push_back(NFAstate(stateSet.size()));
				break;
			}
		}
		if (u)
			Union(begin, fin);
		else
			connect(begin, fin);
		start = begin.back();
		end = fin.back();
	}

	//״̬stateIndexͨ���մ������״̬������״̬stateIndex��epsilon�հ�
	inline vector<int> epsilonClosure(int stateIndex)
	{
		return stateSet[stateIndex].epsilonClosure();
	}

	//״̬��statesͨ���մ������״̬���ϣ���״̬��states��epsilon�հ�
	vector<int> epsilonClosure(const vector<int>& states)
	{
		set<int> des;
		des.insert(states.begin(), states.end());
		vector<int> stack(states.begin(), states.end());
		auto dest(states);
		while (!stack.empty())
		{
			auto tmp = epsilonClosure(stack.back());
			stack.pop_back();
			for (auto i : tmp)
				if (des.find(i) == des.end())
				{
					stack.push_back(i);
					des.insert(i);
					dest.push_back(i);
				}
		}
		return dest;
	}

	//״̬��states���ܷ���a�����״̬����
	vector<int> moveToWithinEpsilon(const vector<int>& states, char a)
	{
		set<int> des;
		for (auto& i : states)
		{
			auto fromATo = epsilonClosure(stateSet[i].move(a));
			des.insert(fromATo.begin(), fromATo.end());
		}
		return vector<int>(des.begin(), des.end());
	}

};

