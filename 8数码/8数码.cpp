#include <iostream>
#include<vector>
#include<fstream>
#include<string>
#include<list>
#include<time.h>
#include<math.h>
using namespace std;

//每个节点/状态
struct State
{
	int index; // 状态号
	int zeroIndex;//0的下标
	vector<int>vec; // 8数码向量
	double func;//估计函数
	int depth;//深度
	int parentIndex;//父节点下标
	int direction;//由父节点如何移动得到：1（左）、2（上）、3（右）、4（下）
};

//=========定义全局变量================
string startFilePath = ".\\start2.txt"; // 默认输入文件路径，1、2、3步数依次增加，4不可达，5的目标状态有变化
string endFilePath = ".\\result.txt";// 默认输出文件路径

const string ALGORITHM = "Algorithm"; // 算法选择
const string HURISTIC_FUNC = "Heuristic function"; // 启发式函数选择
const string EXAMPLE = "Example"; // 测试用例选择

clock_t c_start;//算法开始时间
clock_t c_end;//算法结束时间
vector<int>target; // 目标向量
State s0; // 初始状态
int tag=0;//启发函数的类型,0为宽度优先
list<State>OPEN;//open表
list<State>CLOSED;//closed表
vector<State>allState; // 全局所有的状态
vector<State>pathOfSolution; // 解路径

FILE* file;

int testWeight = 0;

//=========定义接口====================
void showMenu(string const type);//显示菜单
void clearState();//清除状态，只保留初始状态
bool isReachable();//判定是否可达
int getInverseNum(vector<int>vec); // 得到逆序数
void funcValue(State& s);// 得到估计函数值
bool riseCmp(const State& a, const State& b);//按照估计函数值从小到大排序
bool isTarget(vector<int> vec);//是否与目标相同
void extendState(State& s);//扩展节点
void moveZero(State s, int direction);// 移动0，生成下一状态，direction为移动方向
bool isStateExist(vector<int> newVec);//s是否已经存在
void showState(State s);//查看节点信息
void showVec(vector<int>vec);//查看向量
int minMove(int indexA, int indexB);//位置不符的数字移动到目标节点中对应位置的最短距离
int findVecIndex(vector<int>vec,int value);//根据值返回向量中的下标
void analyse();//结果分析
void analyseForFile();//结果分析并保存到文件

void globalOptimalMenu();//全局择优搜索算法菜单控制
void globalOptimal();//全局择优搜索算法实现
void widthFirst();////宽度优先搜索算法实现
double heuristicFunc1(State& s); // 6种启发式函数
double heuristicFunc2(State& s);
double heuristicFunc3(State& s);
double heuristicFunc4(State& s);
double heuristicFunc5(State& s);
double heuristicFunc6(State& s);

double testHeuristicFunc3(State& s, int weight);
void testAnalyse();
void testFuncValue(State& s, int weight);
void testGlobalOptimal(int weight);
void testMain();

//==================函数体===================
int main()
{
	/*test*/
	//testMain();
	//return 0;
	/*test*/
	   
	//若文件已经存在，则清除里面的内容，不存在就创建一个空文件
	freopen_s(&file, endFilePath.c_str(), "w", stdout);
	cout << "";
	//切换为控制台输出
	FILE* afile;
	freopen_s(&afile, "CON", "w", stdout);

	cout << "----------------------------------------" << endl;
	cout << "|        Eight digit problem           |" << endl;
	cout << "----------------------------------------" << endl;

	//判定是否可达
	if (!isReachable()) {
		cout << "Target status not reachable!Exit！" << endl;
		return 0;
	}
	else cout << "Target status is reachable!\n" << endl;

	int algorithmType;
	do {		
		showMenu(ALGORITHM);
		cin >> algorithmType;
		if (algorithmType == 0) {
			s0.vec.clear();

			//清空状态
			clearState();

			allState.clear();

			target.clear();
			break;
		}
		switch (algorithmType)
		{
		case 1: {
			cout << "\nCalculating......\n" << endl;

			//计时
			c_start = clock();
			widthFirst();
			c_end = clock();

			//结果分析
			analyse();

			//清空状态
			clearState();
			continue;
		}
		case 2: {
			globalOptimalMenu();
			continue;
		}
		case 3: {
			showMenu(EXAMPLE);
			int example; cin >> example;
			
			startFilePath = ".\\start" + to_string(example) + ".txt";

			s0.vec.clear();

			//清空状态
			clearState();

			allState.clear();

			target.clear();

			//判定是否可达
			if (!isReachable()) {
				cout << "Target status not reachable!\n" << endl;
			}
			else cout << "Target status is reachable!\n" << endl;
			continue;
		}
		}
	} while (algorithmType!=0);
}

void testMain() {
	
	cout << "----------------------------------------" << endl;
	cout << "|        Eight digit problem           |" << endl;
	cout << "----------------------------------------" << endl;

	//判定是否可达
	if (!isReachable()) {
		cout << "Target status not reachable!Exit！" << endl;
		return ;
	}
	else cout << "Target status is reachable!\n" << endl;

	for (int i = 1; i <= 10; i++) {

		testWeight = i;

		//计时
		c_start = clock();
		testGlobalOptimal(i);
		c_end = clock();
		//结果分析
		testAnalyse();

		//还原状态
		clearState();
	}
}

void showMenu(string const type) {
	if (type == ALGORITHM) {
		cout << "----------------------------------------" << endl;
		cout << "-          1.Width first               -" << endl;
		cout << "-          2.Global optimization       -" << endl;
		cout << "-          3.Change example            -" << endl;
		cout << "-          0.Exit                      -" << endl;
		cout << "----------------------------------------" << endl;
		cout << "Please choose: ";
	}
	if (type == HURISTIC_FUNC) {
		cout << "----------------------------------------" << endl;
		cout << "-          1.heuristicFunc1            -" << endl;
		cout << "-          2.heuristicFunc2            -" << endl;
		cout << "-          3.heuristicFunc3            -" << endl;
		cout << "-          4.heuristicFunc4            -" << endl;
		cout << "-          5.heuristicFunc5            -" << endl;
		cout << "-          6.heuristicFunc6            -" << endl;
		cout << "-          0.Exit                      -" << endl;
		cout << "----------------------------------------" << endl;
		cout << "Please choose: ";
	}

	if (type == EXAMPLE) {
		cout << "----------------------------------------" << endl;
		cout << "-  Choose example from [1,2,3,4,5]     -" << endl;
		cout << "----------------------------------------" << endl;
		cout << "Please choose: ";
	}
}

void funcValue(State& s)
{
	double heuristic =0;
	switch (tag)
	{
	case 1:
		heuristic = heuristicFunc1(s); break;
	case 2:
		heuristic = heuristicFunc2(s); break;
	case 3:
		heuristic = heuristicFunc3(s); break;
	case 4:
		heuristic = heuristicFunc4(s); break;
	case 5:
		heuristic = heuristicFunc5(s); break;
	case 6:
		heuristic = heuristicFunc6(s); break;
	default:
		break;
	}
	s.func = s.depth+ heuristic;
}

void testFuncValue(State& s,int weight)
{
	double heuristic = testHeuristicFunc3(s, weight);
	s.func = s.depth + heuristic;
}

void clearState()
{
	//初始化allState
	State start = allState.front();
	start.func = 0;
	allState.clear();
	allState.push_back(s0);

	//初始化OPEN，CLOSED，pathOfSolution
	OPEN.clear();
	CLOSED.clear();
	pathOfSolution.clear();
}

bool isReachable()
{
	//文件读入
	ifstream inputFile(startFilePath);
	if (!inputFile) {
		cerr << "Open file error!" << endl;
	}

	//分析每行的字符串,构建字符表
	string str;
	while (getline(inputFile, str))
	{
		if (s0.vec.empty()) {
			for (int i = 0; i < str.size(); i++)
			{
				if (str[i] != ' ') {
					s0.vec.push_back(str[i] - '0'); // char->int
				}
			}
		}
		else {
			for (int i = 0; i < str.size(); i++)
			{
				if (str[i] != ' ') {
					target.push_back(str[i] - '0'); // char->int
				}
			}
		}
	}
	
	for (int i = 0; i < s0.vec.size(); i++) {
		if (s0.vec[i] == 0) s0.zeroIndex = i;
	}
	s0.index = allState.size();
	s0.depth = 0;
	s0.func = 0;
	allState.push_back(s0);
	inputFile.close();
	//---初始化过程结束

	int startInverseNum = getInverseNum(s0.vec);
	int targetInverseNum = getInverseNum(target);
	if (startInverseNum % 2 == targetInverseNum % 2)	return true;
	else return false;
}

int getInverseNum(vector<int> vec)
{
	int inverseNum = 0;
	for (int i = 0; i < vec.size()-1; i++) {
		for (int j = i+1; j < vec.size(); j++)
		{
			if (vec[i] > vec[j]&& vec[i]&&vec[j]) inverseNum++;
		}
	}
	return inverseNum;
}

bool riseCmp(const State& a, const State& b) {
	return a.func < b.func;
}

double heuristicFunc1(State& s)
{
	/*
		h(n)定义为当前节点与目标节点差异的度量：即当前节点与目标节点格局相比，
		位置不符的数字个数，0（空格除外）。
		h(n)=位置不符的数字个数（即错位数）
	*/
	int funcValue = 0;
	for (int i = 0; i < target.size(); i++) {
		if (s.vec[i] != target[i]) {
			(target[i] == 0) ? funcValue : ++funcValue;
		}
	}
	return funcValue;
}

double heuristicFunc2(State& s)
{
	/*
		h(n)定义为当前节点与目标节点距离的度量：当前节点与目标节点格局相比，
		位置不符的数字移动到目标节点中对应位置的最短距离之和，0（空格除外）。
		h(n)=位置不符的数字移动到目标节点中对应位置的最短距离之和
	*/
	int distance = 0;//最终距离
	for (int i = 0; i < s.vec.size(); i++) {
		if ((s.vec[i] != target[i]) && (s.vec[i] != 0)) {
			distance += minMove(i, findVecIndex(target, s.vec[i]));
		}
	}
	return distance; // 1216 1926ms
}

double heuristicFunc3(State& s)
{	
	/*
		h(n)定义为每一对逆序数字乘以一个倍数，然后计算该状态与目标状态的差值。
		这个倍数相当于一个权重，一般在2到10之间取值。
	*/
	//参见 testHeuristicFunc3(s, testWeight);

	/*-----------         test1      -------------------------*/
	/*
		这里权重定义为：逆序数之差（如：3 1，其权重即为2）
		h(n)=|该状态逆序数差值权重之和-目标状态逆序数差值权重之和|
	*/
	int inverseNum = 0;
	int targetInverseNum = 0;
	for (int i = 0; i < s.vec.size() - 1; i++) {
		for (int j = i + 1; j < s.vec.size(); j++)
		{
			if (s.vec[i] > s.vec[j] && s.vec[i] && s.vec[j]) inverseNum=inverseNum+ s.vec[i] - s.vec[j];
			if (target[i] > target[j] && target[i] && target[j]) targetInverseNum = targetInverseNum + target[i] - target[j];
		}
	}
	//return abs(inverseNum - targetInverseNum);

	/*-----------         test2      -------------------------*/
	//return 2*abs(inverseNum-targetInverseNum);

	/*-----------         test3      -------------------------*/
	//return 3 * abs(inverseNum - targetInverseNum);

	/*-----------         test4      -------------------------*/
	//return 4 * abs(inverseNum - targetInverseNum);

	/*-----------         test5      -------------------------*/
	//return (s.depth + 1) * abs(inverseNum - targetInverseNum);
	
	/*-----------         test6      -------------------------*/
	//return 0.5*(s.depth + 1) * abs(inverseNum - targetInverseNum);

	/*-----------         test7      -------------------------*/
	return 0.3*(s.depth + 1) * abs(inverseNum - targetInverseNum);
}

double testHeuristicFunc3(State& s,int weight)
{
	return weight*abs(getInverseNum(s.vec) - getInverseNum(target));
}

double heuristicFunc4(State& s)
{
	double differentNum = heuristicFunc1(s);
	/*-----------         test1      -------------------------*/
	/*
		为克服仅计算数字逆序数字数目策略的局限，启发函数h(n)定义为位置不符数字
		个数的总和与3倍的|该状态数字逆序数目-目标状态数字逆序数目|相加。
		h(n)=错位数+3*|该状态数字逆序数目-目标状态数字逆序数目|
	*/	
	//double result = 3 * abs(getInverseNum(s.vec) - getInverseNum(target)) + differentNum;
	

	/*-----------         test2      -------------------------*/
	//h(n)=2*错位数+3*|该状态数字逆序数目-目标状态数字逆序数目|
	//double result = 3 * abs(getInverseNum(s.vec) - getInverseNum(target)) + 2 * differentNum;

	/*-----------         test3      -------------------------*/
	//h(n) = 3*错位数 + 3 * | 该状态数字逆序数目 - 目标状态数字逆序数目 |
	//double result =3*( abs(getInverseNum(s.vec) - getInverseNum(target)) + differentNum);

	/*-----------         test4      -------------------------*/
	//h(n)=错位数+ |该状态数字逆序数目-目标状态数字逆序数目|
	double result = abs(getInverseNum(s.vec) - getInverseNum(target)) + differentNum;

	return result;
}

double heuristicFunc5(State& s)
{
	/*-----------         test1      -------------------------*/
	/* h(n) = 错位数-该状态与目标状态相同的逆序数的对数 */
	int count = 0;
	for (int i = 0; i < s.vec.size() - 1; i++) {
		for (int j = i + 1; j < s.vec.size(); j++)
		{
			if (s.vec[i] > s.vec[j] && s.vec[i] && s.vec[j]) {
				if (findVecIndex(target, s.vec[i]) < findVecIndex(target, s.vec[j]))
					count++;
			}			
		}
	}
	//return heuristicFunc1(s)- count ;

	/*-----------         test2      -------------------------*/
	/* h(n) = 错位数-2×该状态与目标状态相同的逆序数的对数 */
	//return heuristicFunc1(s)- count * 2 ;

	/*-----------         test3      -------------------------*/
	/* h(n) = 错位数-3×该状态与目标状态相同的逆序数的对数 */
	//return heuristicFunc1(s)- count * 3 ;

	/*-----------         test4      -------------------------*/
	/* h(n) = 错位数-4×该状态与目标状态相同的逆序数的对数 */
	return heuristicFunc1(s)- count * 4 ;
}

double heuristicFunc6(State& s)
{
	/*错位的数字移动到目标位的直线距离*/
	int funcValue = 0;
	for (int i = 0; i < target.size(); i++) {
		if (s.vec[i] != target[i] && s.vec[i]&& target[i]) {
			funcValue += abs(i - findVecIndex(target, s.vec[i]));
		}
	}
	return funcValue;
}

bool isTarget(vector<int> vec)
{
	for (int i = 0; i < target.size(); i++) {
		if (vec[i] != target[i]) return false;
	}
	return true;
}

void extendState(State& s)
{
	int zeroIndex = s.zeroIndex;

	//1（左）、2（上）、3（右）、4（下）

	//0向左移动
	if (zeroIndex - 1 >= 0&&(zeroIndex%3!=0)) {
		//如果父节点不是右移得到的，则子节点才能向左移！！！提升效率明显，减少isExist函数的执行
		if(s.direction!=3) moveZero(s, 1);
	}
	//向上
	if (zeroIndex - 3 >= 0) {
		if (s.direction != 4) moveZero(s, 2);
	}
	//向右
	if (zeroIndex + 1 < s.vec.size() && ((zeroIndex+1) % 3 != 0)) {
		if (s.direction != 1) moveZero(s, 3);
	}
	//向下
	if (zeroIndex + 3 < s.vec.size()) {
		if (s.direction != 2) moveZero(s, 4);
	}
}

void moveZero(State s, int direction)
{
	int zeroIndex = s.zeroIndex;
	int changeValue = 0; // 下标移动值
	switch (direction)
	{
	case 1:
		changeValue = -1; break;
	case 2:
		changeValue = -3; break;
	case 3:
		changeValue = 1; break;
	case 4:
		changeValue = 3; break;
	default:
		break;
	}
	swap(s.vec[zeroIndex], s.vec[zeroIndex + changeValue]);

	//去掉旧状态判断，虽然时间效率提高很明显，但结点数明显增加，空间效率下降
	if (!isStateExist(s.vec)) { // 如果现有的状态不包含该状态，就添加一个新状态
		State newState;
		newState.zeroIndex = zeroIndex + changeValue;
		newState.depth = s.depth + 1;
		newState.index = allState.size();
		//for (auto x : s.vec) newState.vec.push_back(x);
		newState.vec = s.vec;
		
		funcValue(newState);
		//testFuncValue(newState, testWeight);
		
		newState.parentIndex = s.index;
		newState.direction = direction;
		allState.push_back(newState);
	}
	swap(s.vec[zeroIndex], s.vec[zeroIndex + changeValue]);

}

/*bool isStateExist(vector<int> newVec)
{
	for (auto x : allState) {
		int count = 0;//s.vec与x.vec相等的个数
		for (int i = 0; i < target.size(); i++) {
			if (newVec[i] == x.vec[i]) count++;
		}
		if (count == target.size())return true;
	}
	return false;
}*/
bool isStateExist(vector<int> newVec)
{
	//修改为从allState最后一个往前判断，因为最近加入的子节点与新扩展的节点重合概率更大
	for (int n = allState.size()-1; n > 0; n--) {
		int count = 0;//s.vec与x.vec相等的个数
		for (int i = 0; i < target.size(); i++) {
			if (newVec[i] == allState[n].vec[i]) count++;
		}
		if (count == target.size())return true;
	}
	return false;
}

void showState(State s)
{
	cout << "================================\n" << endl;
	cout << "State" << s.index << endl; 
	cout << "Depth -> " << s.depth << endl;
	cout << "Func(x) -> " << s.func << endl;
	cout << "ParentState -> "<<s.parentIndex<<endl;
	showVec(s.vec);
	
}

void showVec(vector<int> vec)
{
	cout << "-----" << endl;
	for (int i = 0; i < vec.size(); i++) {
		if ((i + 1) % 3 == 0) cout << vec[i] << endl;
		else cout << vec[i] << " ";
	}
	cout << "-----" << endl;
}

int minMove(int indexA, int indexB)
{
	if (indexA > indexB) {
		int temp = indexA;
		indexA = indexB;
		indexB = temp;
	}
	int result = 0;//最终移动步数
	int differenceValue = indexB - indexA; // 下标的差值
	if (differenceValue >= 3) {//如果大于3，即下移（因为一定不在一行）
		result = 1 + minMove(indexA + 3, indexB);// 递归求解！！！！！！！！！！！！！！！！！！！
	}
	else if (differenceValue == 2) result = 2;//差值等于2，一定只需要移动2步
	else if (differenceValue == 0) result = 0;
	else result = (indexA + 1) % 3 == 0 ? 3 : 1;
	return result;
}

int findVecIndex(vector<int> vec, int value)
{
	for (int i = 0; i < vec.size();i++) {
		if (vec[i] == value) return i;
	}
	return -1;
}

void analyse()
{
	//输出解路径细节
	for (auto x : pathOfSolution) {
		showState(x);
		//cout << heuristicFunc3(x) << endl;
		cout << endl;
	}
	cout << "The length of solution path:  " << pathOfSolution.size()-1 << endl;
	cout << "Solution path:  ";
	for (auto x : pathOfSolution) {
		cout << x.index << " -> ";
	}
	cout << "over!" << endl;
	cout << "Solution direction:  ";
	for (auto x : pathOfSolution) {
		if (x.index == 0) continue;
		switch (x.direction)
		{
		case 1:
			cout << "L -> "; break;
		case 2:
			cout << "U -> "; break;
		case 3:
			cout << "R -> "; break;
		case 4:
			cout << "D -> "; break;
		default:
			break;
		}
	}
	cout << "over!" << endl;
	
	
	if (tag == 0) { // 宽度优先
		cout << "Arithmetic: Width First Arithmetic" << endl;
	}
	else { // 全局择优，启发式函数类型
		cout << "Arithmetic: Global Optimal Arithmetic.";
		cout << "The valuation function is defined as: ";
		switch (tag)
		{
		case 1:
			//cout << "The number of different positions"; break;
			cout << "heuristicFunc1"; break;
		case 2:
			//cout << "The sum of the shortest distance that the number with different positions moves to the corresponding position in the target node"; break;
			cout << "heuristicFunc2"; break;
		case 3:
			cout << "heuristicFunc3"; break;
		case 4:
			cout << "heuristicFunc4"; break;
		case 5:
			cout << "heuristicFunc5"; break;
		case 6:
			cout << "heuristicFunc6"; break;
		default:
			break;
		}
		cout << endl;
	}

	//算法执行时间
	//CLOCKS_PER_SEC展开成 std::clock_t 类型表达式，值等于每秒 std::clock() 所返回的时钟计次数（不必是编译时常量）。
	double duration = (double)(c_end - c_start);
	cout << "Algorithm execution time: " << 1000.0 * duration / CLOCKS_PER_SEC << " ms" << endl;

	//初始状态与目标状态打印
	cout << "Start state:\n";
	showVec(s0.vec);
	cout << "\nTarget state:\n";
	showVec(target);
	cout << endl;

	//同时将结果保存到文件
	analyseForFile();
}

void testAnalyse() {
	//输出解路径细节
	for (auto x : pathOfSolution) {
		showState(x);
		cout << endl;
	}
	cout << "The length of solution path:  " << pathOfSolution.size() - 1 << endl;
	cout << "Solution path:  ";
	for (auto x : pathOfSolution) {
		cout << x.index << " -> ";
	}
	cout << "over!" << endl;
	cout << "Solution direction:  ";
	for (auto x : pathOfSolution) {
		if (x.index == 0) continue;
		switch (x.direction)
		{
		case 1:
			cout << "L -> "; break;
		case 2:
			cout << "U -> "; break;
		case 3:
			cout << "R -> "; break;
		case 4:
			cout << "D -> "; break;
		default:
			break;
		}
	}
	cout << "over!" << endl;

	//算法执行时间
	//CLOCKS_PER_SEC展开成 std::clock_t 类型表达式，值等于每秒 std::clock() 所返回的时钟计次数（不必是编译时常量）。
	double duration = (double)(c_end - c_start);
	cout << "Algorithm execution time: " << 1000.0 * duration / CLOCKS_PER_SEC << " ms" << endl;


	//权重
	cout << "WEIGHT: " << testWeight << endl;

	//初始状态与目标状态打印
	cout << "Start state:\n";
	showVec(s0.vec);
	cout << "\nTarget state:\n";
	showVec(target);
	cout << endl;
}

void analyseForFile()
{
	//输出重定向，将控制台输出到文件中。模式"a+"代表追加文件内容，"w"表示覆盖文件内容
	freopen_s(&file,endFilePath.c_str(), "a+", stdout);

	//输出解路径细节
	for (auto x : pathOfSolution) {
		showState(x);
		//cout << heuristicFunc3(x) << endl;
		cout << endl;
	}
	cout << "The length of solution path:  " << pathOfSolution.size() - 1 << endl;
	cout << "Solution path:  ";
	for (auto x : pathOfSolution) {
		cout << x.index << " -> ";
	}
	cout << "over!" << endl;
	cout << "Solution direction:  ";
	for (auto x : pathOfSolution) {
		if (x.index == 0) continue;
		switch (x.direction)
		{
		case 1:
			cout << "L -> "; break;
		case 2:
			cout << "U -> "; break;
		case 3:
			cout << "R -> "; break;
		case 4:
			cout << "D -> "; break;
		default:
			break;
		}
	}
	cout << "over!" << endl;


	if (tag == 0) { // 宽度优先
		cout << "Arithmetic: Width First Arithmetic";
	}
	else { // 全局择优，启发式函数类型
		cout << "Arithmetic: Global Optimal Arithmetic";
		cout << "The valuation function is defined as: ";
		switch (tag)
		{
		case 1:
			//cout << "The number of different positions"; break;
			cout << "heuristicFunc1"; break;
		case 2:
			//cout << "The sum of the shortest distance that the number with different positions moves to the corresponding position in the target node"; break;
			cout << "heuristicFunc2"; break;
		case 3:
			cout << "heuristicFunc3"; break;
		case 4:
			cout << "heuristicFunc4"; break;
		case 5:
			cout << "heuristicFunc5"; break;
		case 6:
			cout << "heuristicFunc6"; break;
		default:
			break;
		}
		cout << endl;
	}

	//算法执行时间
	//CLOCKS_PER_SEC展开成 std::clock_t 类型表达式，值等于每秒 std::clock() 所返回的时钟计次数（不必是编译时常量）。
	double duration = (double)(c_end - c_start);
	cout << "Algorithm execution time: " << 1000.0 * duration / CLOCKS_PER_SEC << " ms" << endl;

	//初始状态与目标状态打印
	cout << "Start state:\n";
	showVec(s0.vec);
	cout << "\nTarget state:\n";
	showVec(target);
	cout << "***************************************************************************************"<<endl;
	cout << "***************************************************************************************"<<endl;

	FILE* afile;
	freopen_s(&afile, "CON", "w", stdout);
}

void globalOptimalMenu()
{	
	int choose=1;
	while (true)
	{
		//选择启发式函数
		showMenu(HURISTIC_FUNC);
		cin >> choose;
		if (choose == 0) {
			clearState();
			tag = 0;
			return;
		}
		else {
			tag = choose;
			cout << "\nCalculating......\n" << endl;

			//计时
			c_start = clock();
			globalOptimal();
			c_end = clock();
			//结果分析
			analyse();

			//还原状态
			clearState();
		}

	}
}

void globalOptimal()
{
	/*
	(1) 把初始节点S0放入OPEN表，f(S0)。
	(2) 如果OPEN表为空，则问题无解，退出。
	(3) 把OPEN表的第一个节点(记为节点n)取出放入CLOSED表。
	(4) 考察节点n是否为目标节点。若是，则求得问题的解，退出。
	(5) 若节点n不可扩展，则转第(2)步。
	(6) 扩展节点n，用估计函数f(x)计算每个子节点的估价值，并为每个子节点配置指向父节点的指针，
		把这些子节点都送入OPEN表中，然后对OPEN表中的全部节点按估价值从小到大的顺序进行排序，
		然后转第(2)步
	*/
	funcValue(s0);//计算s0的启发式函数值

	//（1）
	OPEN.push_back(s0);

	//（2）
	while (!OPEN.empty()) {
		//（3）
		State maxFunc = OPEN.front();
		OPEN.pop_front();
		CLOSED.push_back(maxFunc);

		//（4）
		if (isTarget(maxFunc.vec)) {
			//保存解路径
			int pathIndex = maxFunc.index;
			do {
				pathOfSolution.insert(pathOfSolution.begin(), allState[pathIndex]);
				pathIndex = allState[pathIndex].parentIndex;
			} while (pathIndex != 0);
			pathOfSolution.insert(pathOfSolution.begin(), allState[0]);
			return;
		}

		//（5）
		int nowStateNum = allState.size();
		extendState(maxFunc);
		if (nowStateNum == allState.size()) { // 如果扩展后allState数量不变，则无法扩展
			continue;
		}

		//（6）
		for (int i = nowStateNum; i < allState.size(); i++) {
			OPEN.push_back(allState[i]);
		}

		OPEN.sort(riseCmp);		
		/*
		//不排序，只寻找估价函数最小值放到OPEN头，貌似还没有sort函数速度快
		int minValue = INT_MAX;
		auto minIndex=OPEN.begin();// 最小值的链表下标
		for (auto it = OPEN.begin(); it != OPEN.end(); ++it) { //it为iterator迭代器
			if ((*it).func < minValue) {
				minValue = (*it).func;
				minIndex = it;
			}
		}
		State minState = *minIndex;
		OPEN.erase(minIndex);
		OPEN.insert(OPEN.begin(), minState);
		*/
	}
	return;
}

void testGlobalOptimal(int weight)
{
	/*
	(1) 把初始节点S0放入OPEN表，f(S0)。
	(2) 如果OPEN表为空，则问题无解，退出。
	(3) 把OPEN表的第一个节点(记为节点n)取出放入CLOSED表。
	(4) 考察节点n是否为目标节点。若是，则求得问题的解，退出。
	(5) 若节点n不可扩展，则转第(2)步。
	(6) 扩展节点n，用估计函数f(x)计算每个子节点的估价值，并为每个子节点配置指向父节点的指针，
		把这些子节点都送入OPEN表中，然后对OPEN表中的全部节点按估价值从小到大的顺序进行排序，
		然后转第(2)步
	*/
	testFuncValue(s0,weight);//计算s0的启发式函数值

	//（1）
	OPEN.push_back(s0);

	//（2）
	while (!OPEN.empty()) {
		//（3）
		State maxFunc = OPEN.front();
		OPEN.pop_front();
		CLOSED.push_back(maxFunc);

		//（4）
		if (isTarget(maxFunc.vec)) {
			//保存解路径
			int pathIndex = maxFunc.index;
			do {
				pathOfSolution.insert(pathOfSolution.begin(), allState[pathIndex]);
				pathIndex = allState[pathIndex].parentIndex;
			} while (pathIndex != 0);
			pathOfSolution.insert(pathOfSolution.begin(), allState[0]);
			return;
		}

		//（5）
		int nowStateNum = allState.size();
		extendState(maxFunc);
		if (nowStateNum == allState.size()) { // 如果扩展后allState数量不变，则无法扩展
			continue;
		}

		//（6）
		for (int i = nowStateNum; i < allState.size(); i++) {
			OPEN.push_back(allState[i]);
		}

		OPEN.sort(riseCmp);
		/*
		//不排序，只寻找估价函数最小值放到OPEN头，貌似还没有sort函数速度快
		int minValue = INT_MAX;
		auto minIndex=OPEN.begin();// 最小值的链表下标
		for (auto it = OPEN.begin(); it != OPEN.end(); ++it) { //it为iterator迭代器
			if ((*it).func < minValue) {
				minValue = (*it).func;
				minIndex = it;
			}
		}
		State minState = *minIndex;
		OPEN.erase(minIndex);
		OPEN.insert(OPEN.begin(), minState);
		*/
	}
	return;
}

void widthFirst()
{
	/*
	(1) 把初始节点S0放入OPEN表
	(2) 初始节点是目标节点？若是，退出；若不是，下一步
	(3) OPEN表为空？若空，失败退出
	(4) 把OPEN表的第一个节点(记为节点n)取出放入CLOSED表。
	(5) 扩展节点n，为每个子节点配置指向父节点的指针，把这些子节点都送入OPEN表中
	(6) 有子节点是目标节点？若有，成功退出；若没有，转(3)
	*/

	//（1）
	OPEN.push_back(s0);
	//（2）
	if (isTarget(s0.vec)) {
		//保存解路径
		int pathIndex = s0.index;
		do {
			pathOfSolution.insert(pathOfSolution.begin(), allState[pathIndex]);
			pathIndex = allState[pathIndex].parentIndex;
		} while (pathIndex != 0);
		pathOfSolution.insert(pathOfSolution.begin(), allState[0]);
		return;
	}
	else {
		do {
			if (OPEN.empty()) { // (3)
				cout << "With First Search Error!" << endl;
				return;
			}
			else {
				//（4）
				State maxFunc = OPEN.front();
				OPEN.pop_front();
				CLOSED.push_back(maxFunc);

				//（5）
				int nowStateNum = allState.size();
				extendState(maxFunc);
				if (nowStateNum == allState.size()) { // 如果扩展后allState数量不变，则无法扩展
					continue;
				}
				//（6）
				for (int i = nowStateNum; i < allState.size(); i++) {
					if (isTarget(allState[i].vec)) {
						//保存解路径
						int pathIndex = allState[i].index;
						do {
							pathOfSolution.insert(pathOfSolution.begin(), allState[pathIndex]);
							pathIndex = allState[pathIndex].parentIndex;
						} while (pathIndex != 0);
						pathOfSolution.insert(pathOfSolution.begin(), allState[0]);
						return;
					}
					OPEN.push_back(allState[i]);
				}
			}
		} while (!OPEN.empty()); 		
	}
}
