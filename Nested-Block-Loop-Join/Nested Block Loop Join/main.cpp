//����һ�����ݴ���һ��Ԫ�飬һ����������10��Ԫ��
//���û������а˸��飬ÿ����10��Ԫ��
//����R��Ƚ϶�

#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h> 
#include <windows.h>
#include <time.h> 
#include <iostream>
using namespace std;

class NestedBlockLoopJoin {
public:
	NestedBlockLoopJoin(){}
	~NestedBlockLoopJoin(){}
	void RandomlyGeneratedData()
	{
		//�������S��
		cout << "Randomly generate 2,000 digital to represent the table S and write them into the hard disk." << endl;
		int num;
		srand((unsigned)time(NULL));
		ofstream out("Stest.dat", ios::out);
		if (!out)
		{
			cout << "Fail to open the Stest.dat to write." << endl;
			return ;
		}
		else cout << "Success to open the Stest.dat to write." << endl;
		for (int i = 0; i<2000; i++)
		{

			num = 1 + rand() % 1000;//�������ֵ����0С��1000�������
			out << num << " ";
		}
		out.close();
		//�������R��
		cout << "Randomly generate 1,000 digital to represent the table R and write them into the hard disk" << endl;
		//	srand((unsigned)time(NULL)); 
		ofstream outfile("Rtest.dat", ios::binary);
		if (!outfile)
		{
			cout << "Fail to open the Rtest.dat to write." << endl;
			return ;
		}
		else cout << "Success to open the Rtest.dat to write." << endl;
		for (int i = 0; i<1000; i++)
		{
			num = 1 + rand() % 1000;//�������ֵ����0С��1000�������
			outfile << num << " ";
		}
		outfile.close();
	}
	void MatchingData(int k, int  buffer[])
	{
		int bufferNum[70];
		ifstream inSfile("Stest.dat", ios::in);
		if (!inSfile)
		{
			cout << "Fail to open the Stest.dat to read." << endl;
			return ;
		}
		else
			cout << "Success to open the Stest.dat to read." << endl;
		ifstream inRfile("Rtest.dat", ios::in);
		if (!inRfile)
		{
			cout << "Fail to open the Rtest.dat to read." << endl;
			return ;
		}
		else
			cout << "Success to open the Rtest.dat to read." << endl;

		cout << "The matching data are shown bellow:" << endl;
		int i = 0;
		while (!inRfile.eof())
		{
			for (i = 0; i < ((k-1) * 10 ); i++)  
				bufferNum[i] = -1;
			for (i = 0; i <= ((k-1) * 10 ); i++)
			{
				if (i == (k * 10 - 1) || inRfile.eof())//���������Ѿ�����K�飬���߻�����û�д湻k-1�鵫��R�еĿ��Ѿ����꣬��ʼƥ��
				{
					while (!inSfile.eof())
					{
						for (int sj = ((k - 1) * 10); sj<(k * 10); sj++)  
							bufferNum[sj] = -1; 
						//ÿһ�ζ�S�е�һ���������
						for (int sj = ((k - 1) * 10); sj<(k * 10); sj++)
						{
							//in.read((char *)&bufferNum[i],sizeof(bufferNum[i]));
							if (inSfile.eof())
								break;
							inSfile >> bufferNum[sj];
						}
						//�������д��е�R�Ŀ��Ԫ���������д��е�S��Ԫ�����ƥ��
						for (int ri = 0; ri<((k - 1) * 10); ri++)
						{
							if (bufferNum[ri] == -1)
								break;
							for (int sj = ((k - 1) * 10); sj<(k * 10); sj++)//S���������е�bufferNum[k-1][sj]����R�������е�Ԫ��ֱ�ƥ��
							{
								if (bufferNum[sj] == -1)
									break;
								if (bufferNum[ri] == bufferNum[sj])
									cout << setw(5) << bufferNum[ri];
							}
						}
					}
					//Ҫ��Sfile���û��ļ�ͷ
					inSfile.clear();
					inSfile.seekg(0, ios::beg);
				}
				//R�е�ÿ����СΪS���chunk���뻺����
				//inRfile.read((char *)&bufferNum[i],sizeof(bufferNum[i]));
				inRfile >> bufferNum[i];
			}
		}
		inSfile.close();
		inRfile.close();
	}
};

int main()
{
	NestedBlockLoopJoin test;
	bool flag1 = false, flag2 = false;
	int *bufferNum;
	while (11)
	{
		int choose;

		system("cls");
		cout << "Please choose the operation:\n";
		cout << "0. Save and exit\n";
		cout << "1. Using existing data to test\n";
		cout << "2. Generating new test data randomly\n";
		cin >> choose;

		switch (choose)
		{
		case 0:
			flag1 = true;
			break;
		case 1:
			break;
		case 2:
			test.RandomlyGeneratedData();
			break;
		default:
			cout << "Input Error!";
			continue;
		}
		if (flag1)
			break;
		flag2 = false;
		while (22)
		{
			system("cls");
			cout << "Please choose the detail of the operation:\n";
			cout << "0. Save and exit\n";
			cout << "1. Nested block loop join test\n";
			
			cin >> choose;
			switch (choose)
			{
			case 0:
				flag2 = true;
				break;
			case 1:
				int k;
				cout << "There are 10 tuples in every buffer block, please input the number of buffer blocks:";
				cin >> k;
				bufferNum = new int[k * 10];//���û�����k���飬ÿ������10��Ԫ��
				clock_t start, finish;
				double duration;

				start = clock();
				test.MatchingData(k, bufferNum);
				finish = clock();
				duration = (double)(finish - start);
				cout << endl << "The running time of the algorithm is " << duration << "ms." << endl;
				system("pause");
				break;
			default:
				cout << "Input Error!";
				continue;
			}
			if(flag2)
				break;
		}
	}
	
	return 0;
}
