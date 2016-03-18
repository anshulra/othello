#include <fstream>
#include <math.h>
using namespace std;
int main()
{
	ofstream outfile;
	outfile.open("allocfactor.txt"); //store allocation factor
	float val[60];
	int sum[2]={0,0};
	for(int i=0;i<60;i++)
	{	
		val[i] = i+1; //assigning marginal value to each move
		sum[i%2]+= val[i];
			
	}
	for(int i=0;i<60;i++) //normalising values
	{	
		val[i] = val[i]/sum[i%2]; 
		outfile << val[i] <<"\n";
	}
	//moving some values by hand to allowtime for extra moves
	
}