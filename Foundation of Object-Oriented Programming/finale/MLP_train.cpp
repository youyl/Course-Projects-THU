//用于训练神经网络
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include "tt.h"
using namespace std;
const int BatchSize=128;//minibatch中每个batch的大小
const int SampleSize[]={5444,6179,5470,5638,5307,4987,5417,5715,5389,5454};//训练集中每个数字对应的图片数量
const int SampleSizeT[]={980,1135,1032,1010,982,892,958,1028,974,1009};//测试集中每个数字对应的图片数量
const float Lrate=2;//学习率
const int Rounds=1000;//训练batch数量
const int Total=55000;
//----------------------------------------------------------------------------------------------------------------------------------------
Graph *graph;
MLP *nn;
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
//----------------------------------------------------------------------------------------------------------------------------------------
vector<pair<vector<float>,vector<float> > >dataset;
int gp;
int main()
{
//	GraphConstruction1();gp=1;
	GraphConstruction2();gp=2;
	nn=new MLP(graph);
	nn->Init();
	vector<float>onehot;
	
	for (int i=0;i<10;i++)
	{
		onehot.push_back(0);
	}
	for (int id=0;id<10;id++)
	{
		onehot[id]=1;
		for (int num=0;num<SampleSize[id];num++)
		{
			string filename="mnist/mnist_digits_training_images/"+to_string(id)+"/"+to_string(num)+".txt";//从训练集中读取图片
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
			dataset.push_back(make_pair(tmp,onehot));
			fin.close();
		}
		cout<<"Currently "<<dataset.size()<<" images loaded"<<endl;//显示读入了多少图片
		onehot[id]=0;
	}
	cout<<"Trainingset Extracted, a total of "<<dataset.size()<<" images loaded"<<endl;
	
	float lrate=Lrate;
	for (int rd=0;rd<Rounds;rd++)
	{
		vector<vector<float> >image;
		vector<vector<float> >answer;
		for (int cnt=0;cnt<BatchSize;cnt++)
		{
			int num=(randv(27500)+27500*(rand()%2))%55000;//随机加入图片到batch
			image.push_back(dataset[num].first);
			answer.push_back(dataset[num].second);
		}
		nn->SetLearningRate(lrate);
		cout<<"Batch No."<<rd<<" Lrate is "<<lrate<<endl;//输出当前轮数和学习率
		nn->MiniBatch(image,answer);
	}
	
	vector<vector<float> >image;
	vector<vector<float> >answer;
	for (int id=0;id<10;id++)
	{
		onehot[id]=1;
		for (int num=0;num<SampleSizeT[id];num++)
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
				tmp[i]/=mxx;
			}
			image.push_back(tmp);
			answer.push_back(onehot);
			fin.close();
		}
		cout<<"Currently "<<image.size()<<" images loaded"<<endl;
		onehot[id]=0;
	}
	cout<<"Testset Extracted, a total of "<<image.size()<<" images loaded"<<endl;
	int correct=nn->TestBatch(image,answer);
	cout<<"This MLP has an accuracy of "<<correct*1.0/10000.0<<" on Test Set"<<endl;//输出训练好的神经网络对于测试集的结果
	nn->sess->Export("parameter/Parameter-LR"+to_string(Lrate)+"-"+to_string(Rounds)+"Batch-BatchSize"+to_string(BatchSize)+"-Correct"+to_string(correct)+"-Graph"+to_string(gp)+".txt");
	//将参数导出到文件
	return 0;
}