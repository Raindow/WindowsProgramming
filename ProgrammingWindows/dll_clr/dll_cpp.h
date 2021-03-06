// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DLL_CPP_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DLL_CPP_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DLL_CPP_EXPORTS
#define DLL_CPP_API __declspec(dllexport)
#else
#define DLL_CPP_API __declspec(dllimport)
#endif

// https://blog.csdn.net/liubing8609/article/details/82156067
// https://blog.csdn.net/qq_33757398/article/details/82229325
// 此类是从 dll_cpp.dll 导出的
class DLL_CPP_API Cdll_cpp {
public:
	Cdll_cpp(void);
	// TODO:  在此添加您的方法。

	//加法
	int Add(int a, int b)
	{
		return a + b;
	}

	//斐波那契数列F(1)=1，F(2)=1, F(n)=F(n-1)+F(n-2)
	long Fibonacci(long num)
	{
		if (num == 1 || num == 2)
			return 1;

		return Fibonacci(num - 1) + Fibonacci(num - 2);
	}

	//阶乘
	long Factorial(long num)
	{
		long faresult = 1;
		if (num > 1)
		{
			for (int i = 1; i <= num; i++)
			{
				faresult = faresult * i;
			}
		}
		return (faresult);
	}
};

extern DLL_CPP_API int ndll_cpp;

DLL_CPP_API int fndll_cpp(void);
