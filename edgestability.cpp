#include <fstream>
#include <map>
#include <list>
#include <string>
#include <math.h>
#include <iostream>
using namespace std;
map< string,list<int> > edges_table;
int stable[4] = {1000,1000,1200,700};
int semistable = 200;
int unstable[3] = {50,75,-25};
bool checkstable(string s,char mychar,char opchar,int pos)
{
 int j = pos-1;
 while(j>=0)
 {
	if(s[j]!=mychar)
		break;
	j--;
 }
 int k =pos+1;
 while(k<=7)
 {
	if(s[k]!=mychar)
		break;
	k++; 
 }
	return (j<0)||(k>7);
}

bool checkunstable(string s,char mychar,char opchar,int pos)
{
 int j= pos-1;
 int k = pos+1;
 
 while(j>=0&&s[j]==mychar)
	j--;
 while(k<=7&&s[k]==mychar)
	k++;
 return j>=0&&k<=7&&((s[j]=='*'&&s[k]=='O')||(s[j]=='O'&&s[k]=='*'));

}
void flip_B(string s,int pos)
{
 if(pos==8)
 {  
    list<int> l;
	if(s[0]=='X')
		l.push_back(700);
	else if(s[0]=='O')
		l.push_back(-700);
	else
		l.push_back(0);
	for(int i=1;i<7;i++)
	{
		int val = 0;
		int ind = (int)fabs(3.5-(i*1.0));
		if(s[i]=='X')
		{
			if(checkstable(s,'X','O',i))
				val = stable[ind];
			else if(checkunstable(s,'X','O',i))
				val = unstable[ind];
			else
				val = semistable;
		}
		else if(s[i]=='O')
		{
			if(checkstable(s,'O','X',i))
				val = -stable[ind];
			else if(checkunstable(s,'O','X',i))
				val = -unstable[ind];
			else
				val = -semistable;
		}
		else
		{
			string t = s.substr(0,i)+"X"+s.substr(i+1,string::npos);
			list<int> l1 = edges_table[t]; 
			list<int>::iterator it1 = l1.begin();
			for(int j=1;j<=i;j++)
				it1++;
			int val1 = *it1;
			float prob = 0;
			if(s[i-1]=='O'&&s[i+1]=='O')
				prob = 0.75;
			else if(s[i-1]=='X'&&s[i+1]=='X')
				prob = 0.25;
			else if((s[i-1]=='O'&&s[i+1]=='X')||(s[i-1]=='X'&&s[i+1]=='O'))
				prob = 0.33;
			else if((s[i-1]=='O'&&s[i+1]=='*')||(s[i-1]=='*'&&s[i+1]=='O'))
				prob = 0.5;
			else if((s[i-1]=='X'&&s[i+1]=='*')||(s[i-1]=='*'&&s[i+1]=='X'))
				prob = 0;
			val = (int)(prob*val1*1.0);
		
		}
		l.push_back(val);
	}
	if(s[7]=='X')
		l.push_back(700);
	else if(s[7]=='O')
		l.push_back(-700);
	else
		l.push_back(0);
	
	edges_table[s] = l;
 }
 else
 {
	string s1 = s.substr(0,pos)+'*'+s.substr(pos+1,string::npos);
	flip_B(s1,pos+1);
	flip_B(s,pos+1);

 }

}

void flip_O(string s,int pos)
{
 if(pos==8)
 {
	list<int> l;
	int val;
	for(int i=0;i<8;i++)
	{
		int ind = (int)fabs(3.5-i);
		val = stable[ind];
		if(s[i]=='O')
			val*=-1;
		l.push_back(val);
	}
	edges_table[s] = l;
	for(int i=7;i>=0;i--)
	{ 
		string s1 = s.substr(0,i)+'*'+s.substr(i+1,string::npos);
		flip_B(s1,i+1);
	}
 }
 else
 {
	string s1 = s.substr(0,pos)+'O'+s.substr(pos+1,string::npos);
	flip_O(s1,pos+1);
	flip_O(s,pos+1);

 }

}
int main()
{
 ofstream outfile;
 outfile.open("edge_stability.txt");
 string s = "XXXXXXXX";
 flip_O(s,8);
 for(int i=7;i>=0;i--)
 { 
	string s1 = s.substr(0,i)+'O'+s.substr(i+1,string::npos);
	flip_O(s1,i+1);
 } 
 cout << edges_table.size();
 for(map<string,list<int> >:: iterator it = edges_table.begin(); it!= edges_table.end();it++)
 {
	outfile << it->first<<" ";
	list<int>p = it->second;
	int sum = 0 ;
	for(list<int>::iterator it1=p.begin();it1!=p.end();it1++)
		sum += *(it1);
	
	outfile<<sum<<"\n";
 }
 outfile.close();
}