#include <iostream>

#include "fawlearn.hpp"
using namespace FawLearn;

using namespace std;

int main(int argc, char* argv[]) {
	//KNN k ({ 
	//	{ "Apple", { 30, 400 } },
	//	{ "Apple", { 35, 380 } },
	//	{ "Banana", { 52, 280 } },
	//	{ "Banana", { 55, 250 } }
	//});
	//cout << "Calc value is: " << k.calc ({ 32, 410 }, 2);

	//DTree t ({          //fly   swim  walk
	//	{ "Bird",       { 0.95, 0,    0.05 } },  //����
	//	{ "Poultry",    { 0.1,  0.45, 0.45 } },  //����
	//	{ "Fish",       { 0,    1,    0    } },  //����
	//	{ "Amphibious", { 0,    0.5,  0.5  } }   //����
	//});
	//cout << "Calc value is: " << t.calc ({ 0.03, 0.97, 0 });//����

	//Bayes b;
	//string s = b.calc (string ("�Ǹ�Ů����Ư��"));
	//cout << "Ŀ����������״̬��" << s << endl;

	Logistic l ({
		{ 0, { 0.,   -1.  } },
		{ 0, { 0.5,  0.   } },
		{ 0, { 1.,   1.   } },
		{ 1, { -1.,  -1.  } },
		{ 1, { -0.5, 0.   } },
		{ 1, { 0.,   1.   } }
	});
	vector<double> v;
	l.calc (v);
	for (double d : v) cout << d << "  ";
	cout << endl;

	return 0;
}
