class Tensor
{
	private:
		vector<float>Val;//存储张量的元素
		vector<int>Dim;//存储张量的大小
	public:
		Tensor();
		//应对不同情况的构造函数
		Tensor(float);
		Tensor(const vector<float>&);
		Tensor(const vector<int>&,float);
		Tensor(const vector<int>&, const vector<float>&);
		//各种基础功能的实现
		int Rank();
		vector<float> GetVal();
		vector<int> GetDim();
		int GetSize()const;
		bool HaveZero();
		bool HaveLTZero();
		void SetZero();
		bool IsScalar();
		void Export(ofstream&);
		void Print()const;
		void RandomInit();
		void Import(ifstream&);
		void softmax();
		int Pos(int,int)const;
		//张量相关的运算
		friend Tensor operator +(const Tensor&, const Tensor&);
		friend Tensor operator -(const Tensor&, const Tensor&);
		friend Tensor operator *(const Tensor&, const Tensor&);
		friend Tensor operator /(const Tensor&, const Tensor&);
		friend bool operator >=(const Tensor&, const Tensor&);
		friend bool operator <=(const Tensor&, const Tensor&);
		friend bool operator >(const Tensor&, const Tensor&);
		friend bool operator <(const Tensor&, const Tensor&);
		friend bool operator ==(const Tensor&, const Tensor&);
		friend bool operator !=(const Tensor&, const Tensor&);
		friend Tensor MatMulti(const Tensor&, const Tensor&);
		friend Tensor cross_entrophy(const Tensor&, const Tensor&);
		friend Tensor cross_entrophy_gradient(const Tensor&, const Tensor&);
		friend Tensor sigmoid(const Tensor&);
		friend Tensor sin(const Tensor&);
		friend Tensor cos(const Tensor&);
		friend Tensor exp(const Tensor&);
		friend Tensor tanh(const Tensor&);
		friend Tensor cosh(const Tensor&);
		friend Tensor log(const Tensor&);
		friend Tensor inverse(const Tensor&);
		friend Tensor transpose(const Tensor&);
		friend Tensor operator *(const Tensor&, const float&);
		friend Tensor operator *(const float&, const Tensor&);
};
Tensor operator +(const Tensor&, const Tensor&);
Tensor operator -(const Tensor&, const Tensor&);
Tensor operator *(const Tensor&, const Tensor&);
Tensor operator /(const Tensor&, const Tensor&);
Tensor cross_entrophy(const Tensor&, const Tensor&);
Tensor cross_entrophy_gradient(const Tensor&, const Tensor&);
bool operator >=(const Tensor&, const Tensor&);
bool operator <=(const Tensor&, const Tensor&);
bool operator >(const Tensor&, const Tensor&);
bool operator <(const Tensor&, const Tensor&);
bool operator ==(const Tensor&, const Tensor&);
bool operator !=(const Tensor&, const Tensor&);
Tensor MatMulti(const Tensor&, const Tensor&);
Tensor sigmoid(const Tensor&);
Tensor sin(const Tensor&);
Tensor cos(const Tensor&);
Tensor exp(const Tensor&);
Tensor tanh(const Tensor&);
Tensor log(const Tensor&);
Tensor cosh(const Tensor&);
Tensor inverse(const Tensor&);
Tensor transpose(const Tensor&);
Tensor operator *(const Tensor&, const float&);
Tensor operator *(const float&, const Tensor&);