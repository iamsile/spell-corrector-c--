//
//  main.cpp
//  spell corrector
//
//  Created by taowei on 13-11-2.
//  Copyright (c) 2013年 taowei. All rights reserved.
//
/*
 spell-word
 倒入单词库，把单词全部转换为小写，去除单词中间的特殊符号；
 接着训练一个概率模型，统计单词出现的次数；
 给丁一个单词w，怎么能够枚举所有可能的正确的拼写呢？利用距离编辑的概念。这2个次之间的编辑距离定义为使用了几次插入（在词中插入一个单字母），删除（删除一个单字母），交换（交换相邻两个字母），替换（把一个字母替换成另一个）的操作从一个词变到另一个词
 误差模型P(w|c)
 建立误差模型是关键
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

class spell {
public:
	int fileLoad(const string& fileName);//读取文件
	string wrongWord(string& word);//纠正错单词

private:
	static map<string, int> dic;
	static char toLow(char str); //把大写改成小写
	static void combinationDistance(const string& word, vector<string>& wordDic);
	static void deleteNonword(vector<string>& wordDic);
};

map<string, int> spell::dic;

char spell::toLow(char str) {
	if (isalpha(str)) {
		return ::tolower(str);
	}
	return ' ';
}

void spell::combinationDistance(const string& word, vector<string>& wordDic) {
	//delete
	for (string::size_type i = 0; i < word.size(); i++)
		wordDic.push_back(word.substr(0, i) + word.substr(i + 1));
	//transposition
	for (string::size_type i = 0; i < word.size(); i++)
		wordDic.push_back(word.substr(0, i) + word.at(i+1) + word.at(i) + word.substr(i + 2));

	for (char i = 'a'; i <= 'z'; i++) {
		//alteration
		for (string::size_type j = 0; j < word.size(); j++)
			wordDic.push_back(word.substr(0, j) + i + word.substr(j + 1));
		//insertion
		for (string::size_type j = 0; j < word.size() + 1; j++)
			wordDic.push_back(word.substr(0, j) + i + word.substr(j));
	}
}

void spell::deleteNonword(vector<string>& wordDic) {
	for (vector<string>::iterator it = wordDic.begin(); it != wordDic.end(); ) {
		if (spell::dic.find(*it) == spell::dic.end())
			it = wordDic.erase(it);
		else it++;
	}
}

int spell::fileLoad(const string& fileName) {
	ifstream file(fileName.c_str(), ios::binary | ios::in);

	if (!file)
		return -1;
	string content(istreambuf_iterator<char> (file), (istreambuf_iterator<char>()));
	transform(content.begin(), content.end(), content.begin(), spell::toLow);

	string::size_type i = 0;
	//切割单词
	while (i < content.size() - 1) {
		while (!isalpha(content.at(i)))
			i++;
		string::size_type start = i;
		while (isalpha(content.at(i)))
			i++;
		string::size_type end = i;
		spell::dic[content.substr(start, end - start)]++;
	}
	file.close();
	return 0;
}

string spell::wrongWord(string& word) {
	vector<string> wordcnt;
	map<string, int>wordhash;

	if (spell::dic.find(word) != spell::dic.end())
		return word;

	combinationDistance(word, wordcnt);
	deleteNonword(wordcnt);

	if (word.size() > 0) {
		for (string::size_type i = 0; i < wordcnt.size(); i++)
			wordhash[wordcnt.at(i)] = spell::dic[wordcnt.at(i)];
		string correct;
		int Max = 0;
		for (map<string, int>::const_iterator it = wordhash.begin(); it != wordhash.end(); it++)
			if (it->second > Max) {
				Max = it->second;
				correct = it->first;
			}
		return correct;
	}
	return " ";
}

int main(int argc, const char * argv[])
{

	spell input;
	input.fileLoad("big.txt");
	string word;
	while ((cout << "Input word: ") && (cin >> word)) {
		string output = input.wrongWord(word);
		if (output != " ")
			cout << "Did you mean: " << output << endl;
		else cout << "no word" << endl;
	}
    return 0;
}

