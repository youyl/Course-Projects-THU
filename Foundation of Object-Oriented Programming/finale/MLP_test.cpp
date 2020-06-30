//用于测试神经网络
#include <iostream>
#include <cstring>
#include <sstream>
#include <ctime>
#include <vector>
#include "tt.h"
using namespace std;
const int SampleSize[]={980,1135,1032,1010,982,892,958,1028,974,1009};//测试集中每个数字对应的图片数量
Graph *graph;
MLP *nn;
void print(int n,int m,vector<float> x)//将vector中的数以字符画的形式打印出来，用于显示mnist中的图片
{
	for (int i=0;i<n;i++)
	{
		for (int j=0;j<m;j++)
		{
			float tmp=x[i*m+j];
			if(tmp<0.1)cout<<". ";
			else cout<<"# ";
		}cout<<endl;
	}cout<<endl;
}
void GraphConstruction1()//使用sigmoid做激活函数的神经网络，含有一个300个点的隐藏层
{
	graph=new Graph;
	
	graph->new_PlaceHolder("Input",{784,1});
	graph->new_PlaceHolder("Answer",{10,1});
	graph->new_Variable("Weight1",{300,784});
	graph->new_Variable("Bias1",{300,1});
	graph->new_Variable("Weight2",{10,300});
	graph->new_Variable("Bias2",{10,1});
	
	graph->new_Binary("t11","MATMUL","Weight1","Input");
	graph->new_Binary("t12","+","Bias1","t11");
	graph->new_Unary("x1","SIGMOID","t12");
	
	graph->new_Binary("t21","MATMUL","Weight2","x1");
	graph->new_Binary("t22","+","Bias2","t21");
	graph->new_Unary("Output","SIGMOID","t22");
	
	graph->new_Binary("Loss","LOSS","Output","Answer");
	graph->new_Unary("Grad","GRAD","Loss");
}
void GraphConstruction2()//使用tanh做激活函数的神经网络，含有一个300个点的隐藏层
{
	graph=new Graph;
	
	graph->new_PlaceHolder("Input",{784,1});
	graph->new_PlaceHolder("Answer",{10,1});
	graph->new_Variable("Weight1",{300,784});
	graph->new_Variable("Bias1",{300,1});
	graph->new_Variable("Weight2",{10,300});
	graph->new_Variable("Bias2",{10,1});
	
	graph->new_Binary("t11","MATMUL","Weight1","Input");
	graph->new_Binary("t12","+","Bias1","t11");
	graph->new_Unary("x1","TANH","t12");
	
	graph->new_Binary("t21","MATMUL","Weight2","x1");
	graph->new_Binary("t22","+","Bias2","t21");
	graph->new_Unary("Output","TANH","t22");
	
	graph->new_Binary("Loss","LOSS","Output","Answer");
	graph->new_Unary("Grad","GRAD","Loss");
}
//----------------------------------------------------------------------------------------------------------------------------------------
vector<vector<float> >image;
vector<vector<float> >answer;
const string filename="LR2.000000-1000Batch-BatchSize128-Correct9442-Graph2";//需要读取的参数文件名称
int main()
{
//	GraphConstruction1();
	GraphConstruction2();
	//这里可以选择使用神经网络的种类
	nn=new MLP(graph);
	nn->sess->Import("parameter/Parameter-"+filename+".txt");//从文件读入参数
	vector<float>onehot;
	ios::sync_with_stdio(0);
	for (int i=0;i<10;i++)
	{
		onehot.push_back(0);
	}
	for (int id=0;id<10;id++)
	{
		onehot[id]=1;
		for (int num=0;num<SampleSize[id];num++)
		{
			string filename="mnist/mnist_digits_test_images/"+to_string(id)+"/"+to_string(num)+".txt";//读入测试集
			ifstream fin(filename.c_str());
			vector<float>tmp;
			tmp.resize(784);
			float mxx=0;
			for (int i=0;i<784;i++)
			{
				fin>>tmp[i];
				mxx=max(mxx,tmp[i]);
			}
			for (int i=0;i<784;i++)
			{
				tmp[i]/=mxx;//标准化数据
			}
			image.push_back(tmp);
			answer.push_back(onehot);
			fin.close();
		}
		onehot[id]=0;
	}
	cout<<"Testset Extracted, a total of "<<image.size()<<" images loaded"<<endl;
	int cor=nn->TestBatch(image,answer);
	cout<<"MLP "<<filename<<" has an accuracy of "<<cor*1.0/10000.0<<" on Test Set"<<endl;//输出对测试集的结果
	cout<<"50 Samples:"<<endl;
	srand(time(0));//初始化随机种子，保证真随机
	for (int i=1;i<=50;i++)//随机展示50个测试集中的图片和对应的结果
	{
		int rd=randv(9999);
		cout<<"Image No."<<rd<<endl;
		print(28,28,image[rd]);
		print(1,10,answer[rd]);
		cout<<"MLP "<<filename<<" says it's number "<<nn->SingleTest(image[rd],answer[rd])<<endl;
//		system("pause");//在本地运行的时候建议解除这一行的注释，以方便观察结果
	}
	return 0;
}