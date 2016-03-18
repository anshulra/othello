#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <time.h>
#include <math.h>
using namespace std;
char board[8][8];
int cutoffdepth,ch;
char mychar,opchar;
ifstream infile;
ofstream outfile;
int c_mobility[2],cval[2],pval[2][3],i_stability,p_mobility[2][3];
map<string,int> edge_stability;
struct boardposition
{
 int r;
 int c;
};

struct Move
{
 int r;
 int c;
 int d1;
 int d2;
 int posid;
 };
map<int,boardposition> emptySpace;
list<Move> pmoves;
int ESAC(int movenum)
{
	return 312 + (int)(6.24*movenum);
} 
int CMAC(int movenum)
{
	if(movenum <=25)
		return 50 + 2*movenum;
	return 75 + movenum;
}
bool isValid(int x,int y) //check if position is a valid boardposition
{
 return x>=0 && x<8 && y>=0 && y<8; 
}
bool isStable(int x,int y,bool stable[8][8],char board[8][8]) //check if a piece is stable i.e. cannot be flipped anymore
{
	bool l1 = (!isValid(x+1,y+1))||(!isValid(x-1,y-1))||(isValid(x-1,y-1)&&board[x][y]==board[x-1][y-1]&&stable[x-1][y-1])||(isValid(x+1,y+1)&&board[x][y]==board[x+1][y+1]&&stable[x+1][y+1]);
	bool l2 = (!isValid(x+1,y))||(!isValid(x-1,y))||(isValid(x-1,y)&&board[x][y]==board[x-1][y]&&stable[x-1][y])||(isValid(x+1,y)&&board[x][y]==board[x+1][y]&&stable[x+1][y]);
	bool l3 = (!isValid(x,y+1))||(!isValid(x,y-1))||(isValid(x,y-1)&&board[x][y]==board[x][y-1]&&stable[x][y-1])||(isValid(x,y+1)&&board[x][y]==board[x][y+1]&&stable[x][y+1]);
	bool l4 = (!isValid(x+1,y-1))||(!isValid(x-1,y+1))||(isValid(x-1,y+1)&&board[x][y]==board[x-1][y+1]&&stable[x-1][y+1])||(isValid(x+1,y-1)&&board[x][y]==board[x+1][y-1]&&stable[x+1][y-1]);
	return l1&l2&l3&l4;


}
int internalStability(char board[8][8]) //calculate stability of pieces on board
{ 
	int i=0,j=0;
	list<boardposition> L; //list of stable positions
	bool stable[8][8];
	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
			stable[i][j] = false;
	/*for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
			cout<<stable[i][j]<<"\t";
		cout<<endl;
	}*/
	

	
	if(board[0][0]!='*')
	{
		boardposition temp;
		temp.r = 0;
		temp.c = 0;
		L.push_back(temp);
		stable[0][0]= true;
	}
	if(board[0][7]!='*')
	{
		boardposition temp;
		temp.r = 0;
		temp.c = 7;
		L.push_back(temp);
		stable[0][7]= true;
	}
	if(board[7][0]!='*')
	{
		boardposition temp;
		temp.r = 7;
		temp.c = 0;
		L.push_back(temp);
		stable[7][0]= true;
	}
	if(board[7][7]!='*')
	{
		boardposition temp;
		temp.r = 7;
		temp.c = 7;
		L.push_back(temp);
		stable[7][7] = true;
	}
	while(!L.empty())
	{
		list<boardposition>::iterator it = L.begin();
		int x = it->r;
		int y = it->c;
		for(i=-1;i<2;i++)
			for(j=-1;j<2;j++)
				if((i||j)&isValid(x+i,y+j)&&!stable[x+i][y+j]&&isStable(x+i,y+j,stable,board))
				{
					stable[x+i][y+j] = true;
					boardposition temp;
					temp.r = x+i;
					temp.c = y+j;
					L.push_back(temp);
				}
		L.pop_front();
	}
	int count = 0;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{	
			if(board[i][j]==mychar&&stable[i][j])
				count+=1;
			else if(board[i][j]==opchar&&stable[i][j])
				count-=1;
			//cout<<stable[i][j]<<"\t";
		}
		//cout<<endl;
	}
	return count;
}
bool check(char board[8][8],int r,int c,int d1,int d2,char mychar,char opchar) //check if there is mypiece in direction (d1,d2) starting from (r,c)
{
 int x = r+d1;
 int y = c+d2;
 if(!isValid(x,y)) //invalid board position
	return false;
 if(board[x][y]==mychar||board[x][y]=='*') //adjacent piece is not opponent's 
	return false;
 while(isValid(x+d1,y+d2)) //keep moving in direction [d1,d2] till my piece is encountered or end of board is reached
 {
	if(board[x+d1][y+d2]==mychar)
		return true;
	else if(board[x+d1][y+d2]=='*') //if a blank is encountered then it is not a valid move
		return false;
	x += d1;
	y += d2;
 }
 return false;
}

list<Move> flip(char board[8][8],Move m,char mychar,char opchar) //flip captured pieces
{   
	list<Move> anchors;
	board[m.r][m.c] = mychar;
	for(int d1=-1;d1<=1;d1++) {
		for(int d2=-1;d2<=1;d2++) {	
			int x = m.r;
			int y = m.c;
			if((d1||d2)&&check(board,x,y,d1,d2,mychar,opchar)) {
				x+=d1;
				y+=d2;
				while(board[x][y]!=mychar) {
					board[x][y] = mychar;
					x+=d1;
					y+=d2;
				}
				Move anchor;
				anchor.r = x;
				anchor.c = y;
				anchor.d1 = d1;
				anchor.d2 = d2;
				anchors.push_back(anchor);
			}	
		}
	}
	return anchors;
}

void undoflip(char board[8][8],Move m,list<Move> anchors,char mychar,char opchar) //undo flipping of captured pieces
{
 board[m.r][m.c] = '*';
 for(list<Move>::iterator it = anchors.begin();it!=anchors.end();it++)
 {	int x=m.r;
	int y=m.c;
	int d1 = it->d1;
	int d2 = it->d2;
	x += d1;
	y += d2;
	while(x!=it->r||y!=it->c)
	{
		board[x][y]=opchar;
		x +=d1;
		y +=d2;
	}
 }	
}


list<Move> generateMoves(char board[8][8],char my,char op) //generate a list of moves
{
 list<Move> moves;
 for(map<int,boardposition>::iterator it=emptySpace.begin();it!=emptySpace.end();it++) //loop through empty spaces
 {	int i,j;
    boardposition temp = it->second; 
	int x = temp.r;
	int y = temp.c;
	for(i=-1;i<=1;i++) //check in all eight directions 
	{
		for(j=-1;j<=1;j++)
		{
			if((i||j)&&check(board,x,y,i,j,my,op))
			{
				Move newmove; //if move is valid add it to moves list
				newmove.r = x;
				newmove.c = y;
				newmove.d1 = i;
				newmove.d2 = j;
				newmove.posid = it->first;
				moves.push_back(newmove);
				break;
			}
		}
		if(j<2)
		break;
	}
 }
 return moves;
}

double input2() //read input from text file
{  
	infile>>mychar;
    int count = 0;
	if(mychar=='O')
		opchar = 'X';
	else
		opchar = 'O';
	double timeleft;
	infile >> timeleft;
	for(int i =0;i<8;i++)
		for(int j=0;j<8;j++)
		{
			infile>>board[i][j];
			if(board[i][j]=='*')
			{
				boardposition temp;
				temp.r = i;
				temp.c = j;
				emptySpace[count++] = temp;
			}
		}
	return timeleft;
}
void potential_mobility(char board[8][8],char mychar,char opchar,int index) //compute potential mobility
{
	p_mobility[index][0] = 0;
	p_mobility[index][1] = 0;
	p_mobility[index][2] = 0;
	for(int i=0;i<8;i++)
		for(int j=0;j<=8;j++)
			{
				if(board[i][j] == mychar)
					continue;
				if(board[i][j]=='*')
				{
					for(int k=-1;k<=1;k++)
					{	
						int l;
						for( l=-1;l<=1;l++)
							if((i||j)&&isValid(i+k,j+l)&&board[i+k][j+l]==opchar)
								{
									p_mobility[index][1]++; //how many empty spaces are adjacent to opponent's pieces
									break;
								}
						if(l<=1)
							break;
					}
					for(int k=-1;k<=1;k++)
						for(int l=-1;l<=1;l++)
							if((i||j)&&isValid(i+k,j+l)&&board[i+k][j+l]==opchar) //sum of empty spaces adjacent to opponent's pieces
								{
									p_mobility[index][2]++;
								}
				}
				if(board[i][j]==opchar)
				{
					for(int k=-1;k<=1;k++)
					{
						int l;
						for( l=-1;l<=1;l++)
							if((i||j)&&isValid(i+k,j+l)&&board[i+k][j+l]=='*') //how many opponent pieces are adjacent to empty space
							{
								p_mobility[index][0]++;
								break;
							}
						if(l<=1)
							break;
					
					}
				}
			}
		}
int eval1(char board[8][8]) 
{
	int count = 0;
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
			if(board[i][j]==mychar)
				count++;
			else if(board[i][j]==opchar)
				count--;
	if(count > 0)
		return 1;
	else if(count < 0 )
		return -1;
	else
		return 0;

}
int eval2(char board[8][8],int movenum,bool turn,char mychar,char opchar) //eval function
{
 int edge_s = 0; //edge stability
 stringstream e[4];
 char edges[4][8];
 if(mychar=='X')
 {
	for(int j=0;j<8;j++)
		edges[0][j]==board[0][j];
	for(int j=0;j<8;j++)
		edges[1][j]==board[7][j];
	for(int j=0;j<8;j++)
		edges[2][j]==board[j][0];
	for(int j=0;j<8;j++)
		edges[3][j]==board[j][7];
	
 }
 else
 {
	for(int j=0;j<8;j++)
	{	
		if(board[0][j] == '*')
			edges[0][j] = '*';
		else if(board[0][j]=='X')
			edges[0][j] = 'O';
		else
			edges[0][j] = 'X';
	}
	for(int j=0;j<8;j++)
	{	
		if(board[7][j] == '*')
			edges[1][j] = '*';
		else if(board[7][j]=='X')
			edges[1][j] = 'O';
		else
			edges[1][j] = 'X';
	}
	for(int j=0;j<8;j++)
	{	
		if(board[j][0] == '*')
			edges[2][j] = '*';
		else if(board[j][0]=='X')
			edges[2][j] = 'O';
		else
			edges[2][j] = 'X';
	}
	for(int j=0;j<8;j++)
	{	
		if(board[j][7] == '*')
			edges[3][j] = '*';
		else if(board[j][7]=='X')
			edges[3][j] = 'O';
		else
			edges[3][j] = 'X';
	}
 
 }
 for(int i=0;i<4;i++)
 { 	
	for(int j=0;j<8;j++)
		e[i] << edges[i][j];
	edge_s += edge_stability[e[i].str()];	
 }
 int index;
 //cout <<"in eval2"<<endl;
 if(turn)
	index = 1;
 else
	index = 0;
 edge_s *= (int)pow(-1,index);		
 list<Move> moves = generateMoves(board,mychar,opchar);
 c_mobility[index] = moves.size();
 int common = 0;
 for(list<Move>::iterator it = moves.begin();it!=moves.end();it++)
	for(list<Move>::iterator it1 = pmoves.begin();it1!=pmoves.end();it1++)
		{
			if(it1->r==it->r && it1->c == it->c) //finding numer of common moves
			{
				common++;
				break;
			}
		}
 cval[0] = common + 2*(c_mobility[0]-common);
 cval[1] = common + 2*(c_mobility[1]-common);
 potential_mobility(board,mychar,opchar,index);
 potential_mobility(board,opchar,mychar,(index+1)%2);
 int currentmobility = (int)(1000.0*(cval[0]-cval[1])/(cval[0]+cval[1]+2.0));
 int pm = 0; 
 for(int i=0;i<=2;i++)
	pm+= (int)(1000.0*(p_mobility[0][i]-p_mobility[1][i])/(p_mobility[0][i]+p_mobility[1][i]+2.0));
 

 //cout<<movenum<<" "<<edge_s<<" "<<i_stability<<" "<<currentmobility<<" "<<pm<<endl;
 return  (ESAC(movenum)*edge_s+36*i_stability + CMAC(movenum)*currentmobility + 99*pm);
}

int alphabetavalue(char board[8][8],Move* m,bool turn,char mychar,char opchar,int alpha,int beta,int depth)
{
 int val;
 list<Move> anchors;
 //cout <<"inalphabetavalue"<<endl;
 //set the move name
 if(m!=NULL) //move is not pass
 {
	anchors = flip(board,*m,opchar,mychar);
	//cout <<(char)(y+97)<<x<<endl;
 }
 
 //depth check
 
 if(depth==cutoffdepth) //depth limit reached
 {
	//cout<<"depth = cutoff"<<endl;
	switch(ch)
	{
		case 1: val = eval1(board); break;
		case 2: val = eval2(board,60-emptySpace.size()+1,turn,mychar,opchar);break;
	}
 }
 else //depth limit not reached
 {  
	int index;
	if(m!=NULL) //move is not pass
		emptySpace.erase(emptySpace.find(m->posid)); //remove move from list of emptySpaces
	if(turn) //min node initial value is infinity
	{	
		val = 2000000000;
		index = 1;
	}
	else //max node initial value is -infinity
	{	
		val = -2000000000;
		index = 0;
	}
	list<Move> moves = generateMoves(board,mychar,opchar);
	if(depth+1 == cutoffdepth)
	{
		i_stability = internalStability(board);
		pmoves = moves;
		c_mobility[index] = moves.size();
	}
	if(!moves.empty()) //if a move is possible
	{	
		for(list<Move>:: iterator it = moves.begin();it!=moves.end();it++)
		{   
			int v = alphabetavalue(board,&(*it),!turn,opchar,mychar,alpha,beta,depth+1);
			if(turn && v < val) //min node check if successor value is smaller
			{
				val = v;
				if(val <= alpha)
					break;
			}
			else if(!turn && v > val) //max node check if successor value is larger
				{
				val = v;
				if(beta <= val)
					break;
				}
			
		}
	}
	else //no further move possible
	{
		if(!generateMoves(board,opchar,mychar).empty()) //not endgame condition
		{
			val = alphabetavalue(board,NULL,!turn,opchar,mychar,alpha,beta,depth+1);
		}
		else //game end condition i.e. current move was pass and no further moves are possible
		{
			switch(ch)
			{
			case 1: val = eval1(board);break;
			case 2: 
					i_stability = internalStability(board);
					pmoves = moves;
					c_mobility[(index+1)%2] = moves.size();
					potential_mobility(board,mychar,opchar,(index+1)%2);
					val = eval2(board,60-emptySpace.size()+1,turn,mychar,opchar);break;
			}
		}
	}
 }
 	
 if(m!=NULL) //if move is not null reset board and insert move in emptySpace
 {
	boardposition temp;
	temp.r = m->r;
	temp.c = m->c;
	emptySpace[m->posid] = temp;
	undoflip(board,*m,anchors,opchar,mychar);
 }
 return val;
}
string alphabeta()
{
 //cout <<"inalphabeta"<<endl;
 stringstream m;
 int r,c;
 list<Move> moves = generateMoves(board,mychar,opchar);
 int alpha = -2000000000,beta = 2000000000;
 list<Move>::iterator bestmove ;
 int v;
 for(list<Move>::iterator it=moves.begin();it!=moves.end();it++)
	{
		v = alphabetavalue(board,&(*it),true,opchar,mychar,alpha,beta,1);
		if( v > alpha)
		{
			alpha = v;
			r = it->r;
			c = it->c; 
		}
  }
  m<<(char)(97+c);
  m<<(r)+1;
  //cout <<v<<endl;
 return m.str();
}

void competition()
{
	double t_remaining = input2();
	//cout <<t_remaining<<endl;
	ifstream infile2;
	infile2.open("allocfactor.txt");
	float timefrac[60];
	for(int i=0;i<=59;i++)
	{
		infile2>>timefrac[i];
	}
	infile2.close();
	infile2.open("edge_stability.txt");
	for(int i=0;i<6561;i++)
	{
		string temp;
		infile2 >> temp;
		int val;
		infile2 >>val;
		edge_stability[temp] = val;
	
	}
	infile2.close();
	int l = emptySpace.size();
	t_remaining*=timefrac[60-l];
	//cout <<currentmobility(board)<<" "<<internalStability(board);
	clock_t start,end;
	cutoffdepth = 2;
	bool flag = true;
	int b = generateMoves(board,mychar,opchar).size();
	int nodes = b;
	ch = 2;
	start = clock();
	string m = alphabeta();
	end = clock();
	float time_elapsed = ((float)(end-start))/CLOCKS_PER_SEC;
	//t_remaining-= time_elapsed;
	nodes = b*nodes;
	cutoffdepth+=1;	
	while(true)
	{
		if(t_remaining < time_elapsed)
			break;
		
		start = clock();
		m = alphabeta();
		end = clock();
		float time_iter = ((float)(end-start))/CLOCKS_PER_SEC;
		time_elapsed+= time_iter;
		//nodes = b*nodes;
		cutoffdepth+=1;	
		//cout<<"Time elapsed: "<<time_elapsed<<endl;//<<" b: "<<b<<endl;
	    //cout<<"Remaining time: "<<t_remaining<<endl;
		
	}
	//cout <<time_elapsed<<endl;
	outfile << m;
}
int main()
{   
	clock_t start,end;
	//start = clock();
	infile.open("input.txt");
	outfile.open("output.txt");
	char task;
	infile>> task;
	switch(task)
	{
		case '4': competition();
				  break;
	
	}
	infile.close();
	outfile.close();
	//end=  clock();
	//cout << ((float)end-start)/CLOCKS_PER_SEC;
}
