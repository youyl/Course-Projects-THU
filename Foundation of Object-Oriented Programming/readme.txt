在Windows 10系统中经过测试，可以运行。
g++需要支持c++11，测试时用的是mingw的g++ 4.9.2
在Linux系统下没有测试。理论上可以编译运行。
makefile中的clean需要系统支持del命令。

阅读注释请采用utf-8或者与之兼容的编码

Instruction.md/pdf为说明文档
ExperimentReport.md/pdf为实验报告

在运行MLP相关的内容之前，需要先将mnist目录下的数据集以合适的方式解压
解压的方式参考MLP_train.cpp中的文件读取部分