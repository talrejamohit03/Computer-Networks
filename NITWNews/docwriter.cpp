#include<iostream>
#include<fstream>

using namespace std;

int main(){
	ofstream fout;
	fout.open("repfile",ios::app);
	string s;
	getline(cin,s);
	fout<<s;
	return 0;
}
