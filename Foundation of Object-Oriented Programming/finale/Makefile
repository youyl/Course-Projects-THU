
all: 
	g++ -c opt/Basic.cpp -o OptBasic.o -std=c++11 -O2
	g++ -c opt/Cmp.cpp -o OptCmp.o -std=c++11 -O2
	g++ -c opt/Unary.cpp -o OptUnary.o -std=c++11 -O2
	g++ -c opt/Binary.cpp -o OptBinary.o -std=c++11 -O2
	g++ -c opt/Special.cpp -o OptSpecial.o -std=c++11 -O2
	g++ -c session/Basic.cpp -o SessBasic.o -std=c++11 -O2
	g++ -c session/Input.cpp -o SessInput.o -std=c++11 -O2
	g++ -c session/File.cpp -o SessFile.o -std=c++11 -O2
	g++ -c graph/Basic.cpp -o GraphBasic.o -std=c++11 -O2
	g++ -c graph/NewOpt.cpp -o GraphNewOpt.o -std=c++11 -O2
	g++ -c tensor/Basic.cpp -o TensorBasic.o -std=c++11 -O2
	g++ -c tensor/Calc0.cpp -o TensorCalc0.o -std=c++11 -O2
	g++ -c tensor/Calc1.cpp -o TensorCalc1.o -std=c++11 -O2
	g++ -c tensor/Calc2.cpp -o TensorCalc2.o -std=c++11 -O2
	g++ -c mlp/MLP.cpp -o MLP.o -std=c++11 -O2
	g++ main.cpp MLP.o OptBasic.o OptCmp.o OptUnary.o OptBinary.o OptSpecial.o SessBasic.o SessInput.o SessFile.o GraphBasic.o GraphNewOpt.o TensorBasic.o TensorCalc0.o TensorCalc1.o TensorCalc2.o -o main1 -std=c++11 -O2
	g++ tensor_test.cpp MLP.o OptBasic.o OptCmp.o OptUnary.o OptBinary.o OptSpecial.o SessBasic.o SessInput.o SessFile.o GraphBasic.o GraphNewOpt.o TensorBasic.o TensorCalc0.o TensorCalc1.o TensorCalc2.o -o tensortest -std=c++11 -O2
	g++ main2.cpp MLP.o OptBasic.o OptCmp.o OptUnary.o OptBinary.o OptSpecial.o SessBasic.o SessInput.o SessFile.o GraphBasic.o GraphNewOpt.o TensorBasic.o TensorCalc0.o TensorCalc1.o TensorCalc2.o -o main2 -std=c++11 -O2
	g++ main3.cpp MLP.o OptBasic.o OptCmp.o OptUnary.o OptBinary.o OptSpecial.o SessBasic.o SessInput.o SessFile.o GraphBasic.o GraphNewOpt.o TensorBasic.o TensorCalc0.o TensorCalc1.o TensorCalc2.o -o main3 -std=c++11 -O2
	g++ MLP_train.cpp MLP.o OptBasic.o OptCmp.o OptUnary.o OptBinary.o OptSpecial.o SessBasic.o SessInput.o SessFile.o GraphBasic.o GraphNewOpt.o TensorBasic.o TensorCalc0.o TensorCalc1.o TensorCalc2.o -o train -std=c++11 -O2
	g++ MLP_test.cpp MLP.o OptBasic.o OptCmp.o OptUnary.o OptBinary.o OptSpecial.o SessBasic.o SessInput.o SessFile.o GraphBasic.o GraphNewOpt.o TensorBasic.o TensorCalc0.o TensorCalc1.o TensorCalc2.o -o test -std=c++11 -O2

clean:
	del *.o
	del *.exe
