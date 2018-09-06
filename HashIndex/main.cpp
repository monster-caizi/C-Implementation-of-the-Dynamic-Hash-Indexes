
#include<string>
#include<cstdio>
#include<fstream>
#include<stack> 
#include<cstring>
#include<time.h>
#include<iostream>
using namespace std;

#define num  2147483648;
int blockNum = 1;
int BSzie=1;

struct myData
{
	unsigned  Index;

	char Data[124];
}; 

struct myHashBlock
{
	int DataNUM;

	int CheckNUM;

	myData *DataArray;

	int nextBlockNum;
};

struct myHashNode
{
	int BlockNum;

	myHashNode *Parent,*rChild,*lChild;
};

class myHashIndex
{
private:
	myHashNode *root;
	int bufferSize;
public:

	char TreeFile[10],DataFile[10];
	//char *buffer;

	myHashIndex(int bSize)
	{
		root=NULL;
		bufferSize = bSize;
		strcpy(TreeFile ,"sTree.txt");
		strcpy(DataFile , "sData.dat");
	}

	~myHashIndex()
	{
		//saveTree();
		//delete []buffer;
		if(root!=NULL)
			deleteTree(root);
	}

	void deleteTree(myHashNode *current)
	{
		if(current->lChild!=NULL)
			deleteTree(current->lChild);
		if(current->rChild!=NULL)
			deleteTree(current->rChild);
		delete current;
	}
	//�������������
	bool saveTree()
	{
		stack<myHashNode*>s;  //��һ��ջ���ڱ����ҽڵ��ַ
		myHashNode *p=root;
		s.push(NULL);
		ofstream out(TreeFile,ios::out);
		if(!out)
		{
			cout<<"Output file open failed";
			return false;
		}
		while(p!=NULL||!s.empty())
		{
			while(p!=NULL)
			{
				out<<p->BlockNum<<"   ";
				s.push(p);
				p=p->lChild;
			}
			out<<"#";
			if(!s.empty())
			{
				p=s.top();s.pop();
				p=p->rChild;
				while(p==NULL&&!s.empty())
				{
					out<<"#";
					p=s.top();s.pop();
					if(p!=NULL)
						p=p->rChild;
				}
			}
		}
		out<<"#\n";
		out.close();
		return true;
	}
	//��ȡ���������Ķ���������
	bool readTree()
	{
		ifstream inData(TreeFile, ios::in);//���ļ�.txt
		if (!inData)//�жϴ��ļ��Ƿ�ɹ�
			return false;
		readTree(root,NULL,inData);
		inData.close();
		return true;
	}

	bool readTree(myHashNode *&current,myHashNode *parent,ifstream &inData )
	{
		char w;
		int bNum;
		if(!inData.eof())
		{
			inData>>w;
			if(w!='#')
			{
				inData.seekg(-1,ios::cur);
				inData>>bNum;
				current = new myHashNode;
				current->Parent = parent;
				current->BlockNum = bNum;
				readTree(current->lChild,current,inData);
				readTree(current->rChild,current,inData);
			}
			else
			{
				current = NULL;
			}
		}

		return true;

	}
	//�����
	myHashBlock*& readBlock(int bNUM)
	{
		char buffer[1024*2];
		char *p = buffer;
		//�½�Ͱ
		myHashBlock *pB = NULL;
		pB = new myHashBlock;
		pB->CheckNUM = 0; 
		pB->DataNUM = bufferSize;
		pB->DataArray = new myData[bufferSize];
		pB->nextBlockNum = -1;
		//�����Ӧ��
		ifstream in(DataFile,ios::in||ios::binary);
		if(!in)
		{
			cerr<<"Input file open failed";
			return pB;
		}
		in.seekg((bNUM-1)*1024*BSzie,ios::beg);
		in.read(buffer,1024*BSzie);
		in.close();
		//���������ƿ�
		memcpy(&(pB->CheckNUM),p+8,4);
		memcpy(&(pB->DataNUM),p+12,4);
		memcpy(&(pB->nextBlockNum),p+16,4);
		p = p+20;
		int i,max = pB ->CheckNUM; 
		for( i=0;i<max;i++)
		{
			memcpy(&(pB->DataArray[i].Index),p,4);
			memcpy((pB->DataArray[i].Data),p+4,124);
			p = p + 128;
		}
		//���ؽ���������
		return pB;
	}
	//д����
	bool writeBlock(int bNUM,myHashBlock *pB)
	{
		char buffer[1024*2];
		char *P;
		//�������Զ�������ʽд�뻺����
		P = buffer;
		memcpy(P+8,&(pB->CheckNUM),4);
		memcpy(P+12,&(pB->DataNUM),4);
		memcpy(P+16,&(pB->nextBlockNum),4);
		P = P+20;
		int i,max = pB ->CheckNUM; 
		for(i=0;i<max;i++)
		{
			memcpy(P,&(pB->DataArray[i].Index),4);
			memcpy(P+4,(pB->DataArray[i].Data),124);
			P = P + 128;
		}
		//д����
		ofstream out(DataFile,ios::out||ios::binary||ios::ate);
		if(!out)
		{
			cout<<"Output file open failed";
			return false;
		}
		out.seekp((bNUM-1)*1024*BSzie,ios::beg);
		out.write(buffer,1024*BSzie);
		out.close();
		return true;

	}

	bool Insert(myData data)
	{
		myHashBlock *pB;
		myHashNode *current;
		int i;
		//�жϸ��ڵ��Ƿ�Ϊ�գ��½���һ��Hash��
		if(root==0)
		{
			pB = new myHashBlock;
			pB->CheckNUM = 0;
			pB->DataNUM = bufferSize;
			pB->DataArray = new myData[bufferSize];
			pB->nextBlockNum = -1;
			root = new myHashNode;
			root->BlockNum = blockNum;
			blockNum++;
			root->lChild=root->rChild=root->Parent=NULL;
			pB->DataArray[pB->CheckNUM]=data;
			pB->CheckNUM++;
			writeBlock(root->BlockNum,pB);
			delete []pB->DataArray;
			delete pB;
			return true;
		}

		//�����ڵ�ָ��Hash��ʱ���ڸ��ڵ���ָ��Ŀ�����������֪���������½������ҽڵ�
		unsigned index = num;
		if(root->BlockNum >=0 )
		{
			pB =readBlock(root->BlockNum);
			//δ������������
			if(pB->CheckNUM<(pB->DataNUM))
			{
				pB->DataArray[pB->CheckNUM]=data;
				pB->CheckNUM++;
				writeBlock(root->BlockNum,pB);
			}
			else
			{
				//������÷ֲ������½����µ����ҽ��
				bool f1 = setNewNode(root,data,index);
				delete []pB->DataArray;
				delete pB;
				return f1;
			}

			delete []pB->DataArray;
			delete pB;
			return true;
		}

		//�����ڵ㲻ָ��Hashʱ��ɨ��Data��index������root��ָ��������ҵ�Data�Ĺ���Hash��
		current = root;
		
		for(i=0;i<32;i++,index=index>>1)
		{
			if((data.Index & index)==0)
				if(current->lChild!=NULL)
					current = current->lChild;
				else
				{
					//Ϊ�յ�ʱ���½���
					pB = new myHashBlock;
					pB->CheckNUM = 0;
					pB->DataNUM = bufferSize;
					pB->DataArray = new myData[bufferSize];
					pB->nextBlockNum = -1;
					current->lChild = new myHashNode;
					current->lChild->BlockNum = blockNum;
					blockNum++;
					current->lChild->lChild=current->lChild->rChild=NULL;
					current->lChild->Parent = current;
					current = current->lChild;
					break;
				}
			else
				if(current->rChild!=NULL)
					current = current->rChild;
				else
				{
					//Ϊ�յ�ʱ���½���
					pB = new myHashBlock;
					pB->CheckNUM = 0;
					pB->DataNUM = bufferSize;
					pB->DataArray = new myData[bufferSize];
					pB->nextBlockNum = -1;
					current->rChild = new myHashNode;
					current->rChild->BlockNum = blockNum;
					blockNum++;
					current->rChild->lChild=current->rChild->rChild=NULL;
					current->rChild->Parent = current;
					current = current->rChild;
					break;
				}
			if(current->BlockNum>=0)
			{
				//��Ϊ�յ�ʱ������
				pB = readBlock(current->BlockNum);
				break;
			}
		}
		
		if(pB->CheckNUM<(pB->DataNUM))
		{
			pB->DataArray[pB->CheckNUM]=data;
			pB->CheckNUM++;
			writeBlock(current->BlockNum,pB);
		}
		else
		{
			//������÷ֲ������½����µ����ҽ��
			index = index>>1;
			bool f2 = setNewNode(current,data,index);
			delete []pB->DataArray;
			delete pB;
			return f2;
		}
		delete []pB->DataArray;
		delete pB;
		return true;
	
	}

	bool DeleteAll(unsigned Index)
	{
		myHashNode *current;
		myHashBlock *pB,*p ;
		unsigned index = num;
		int i;
		//�жϸ��ڵ��Ƿ�Ϊ��
		if(root == NULL)
			return false;
		bool Flag1 =false;
		bool Flag2 = false;
		//�����ڵ�ָ��Hash��ʱ��ɨ��ÿ飬�õ��Ƿ���ɾ������
		if(root->BlockNum >=0 )
		{
			pB = readBlock( root->BlockNum);
			for(i=0;i<=pB->CheckNUM;i++)
				if(pB->DataArray[i].Index == Index)
				{
					deleteData(i,pB);
					writeBlock(root->BlockNum,pB);
					Flag1 = true;
					//return true;
				}
			if(Flag1)
			{
				delete []pB->DataArray;
				delete pB;
				return true;
			}
			else
			{
				delete []pB->DataArray;
				delete pB;
				return false;
			}
			
		}

		//�����ڵ㲻ָ��Hashʱ��ɨ��Data��index������root��ָ��������ҵ�Data�Ĺ���Hash��
		current = root;
		
		for(i=0;i<32;i++,index=index>>1)
		{
			if((Index & index)==0)
				if(current->lChild!=NULL)
					current = current->lChild;
				else
					return false;
			else
				if(current->rChild!=NULL)
					current = current->rChild;
				else
					return false;
			if(current->BlockNum>=0)
				break;
		}
		//����ɨ�����ÿ飬��������飬ɾ������
		pB = readBlock(current->BlockNum) ;
		if(pB->nextBlockNum != -1)
			Flag2 = true;
		while (pB != NULL)
		{
			for(i=0;i<=pB->CheckNUM;i++)
			if(pB->DataArray[i].Index == Index)
			{
				deleteData(i,pB);
				writeBlock(current->BlockNum,pB);
				if(pB->CheckNUM<0)
					current->BlockNum = pB->nextBlockNum;
				if(!Flag2)
					checkCombine(current->Parent);
				delete []pB->DataArray;
				delete pB;
				return true;
			}

			if(pB->nextBlockNum!=-1)
			{
				p = pB;
				pB=readBlock(pB->nextBlockNum);
				delete []p->DataArray;
				delete p;
			}
			else
			{
				delete []pB->DataArray;
				delete pB;
				pB = NULL;
			}
		}

	}

	myData* Check(unsigned Index)
	{
		myHashNode *current;
		myData *pD = new myData;
		myHashBlock *pB ,*p;
		unsigned index = num;
		int i;
		//�жϸ��ڵ��Ƿ�Ϊ��
		if(root == NULL)
			return NULL;
		//�����ڵ�ָ��Hash��ʱ��ɨ��ÿ飬�õ��Ƿ������������
		if(root->BlockNum >=0 )
		{
			pB =readBlock( root->BlockNum );
			for(i=0;i<=pB->CheckNUM;i++)
				if(pB->DataArray[i].Index == Index)
				{
					*pD = pB->DataArray[i];
					delete []pB->DataArray;
					delete pB;
					return pD;
				}
			delete []pB->DataArray;
			delete pB;
			return NULL;
		}

		//�����ڵ㲻ָ��Hashʱ��ɨ��Data��index������root��ָ��������ҵ�Data�Ĺ���Hash��
		current = root;
		
		for(i=0;i<32;i++,index=index>>1)
		{
			if((Index & index)==0)
				if(current->lChild!=NULL)
					current = current->lChild;
				else
					return NULL;
			else
				if(current->rChild!=NULL)
					current = current->rChild;
				else
					return NULL;
				
			if(current->BlockNum>=0)
				break;
		}
		//ɨ�����ÿ飬�ж�index�������ͬ���򷵻ص�һ���ҵ�ֵ
		pB = readBlock(current->BlockNum) ;
		while (pB != NULL)
		{
			for(i=0;i<=pB->CheckNUM;i++)
			if(pB->DataArray[i].Index == Index)
			{
				*pD = pB->DataArray[i];
				delete []pB->DataArray;
				delete pB;
				return pD;
			}
			if(pB->nextBlockNum!=-1)
			{
				p = pB;
				pB=readBlock(pB->nextBlockNum);
				delete []p->DataArray;
				delete p;
			}
			else
			{
				delete []pB->DataArray;
				delete pB;
				pB = NULL;
			}
		}
		return NULL;
	}

	bool setNewNode(myHashNode *&node,myData data,unsigned index)
	{
		myHashBlock *pL,*pR,*p;
		if(index!=0)
		{
			//����������Hash��
			pL = new myHashBlock;
			pL->CheckNUM = 0;
			pL->DataNUM = bufferSize;
			pL->DataArray = new myData[bufferSize];
			pL->nextBlockNum = -1;
			pR = new myHashBlock;
			pR->CheckNUM = 0;
			pR->DataNUM = bufferSize;
			pR->DataArray = new myData[bufferSize]();
			pR->nextBlockNum = -1;
			//������Nodeָ����Hash�飬�������ڵ��Hash��ָ����ΪNULL
			node->lChild = new myHashNode;
			node->rChild = new myHashNode;
			node->lChild->BlockNum = node->BlockNum;
			node->rChild->BlockNum = blockNum;
			blockNum++;
			p = readBlock( node->BlockNum);
			node->BlockNum = -1;

			node->lChild->Parent=node->rChild->Parent=node;
			node->lChild->lChild=node->rChild->lChild=node->lChild->rChild=node->rChild->rChild=NULL;
			//���·���ԭHash�����ݣ���pRָ��Ŀ�
			myData *pD;
			
			for(int i=0;i<p->DataNUM;i++)
			{
				pD=&p->DataArray[i];
				//cout<<pD->Index<<"   ";
				if((pD->Index & index)==0)
				{
					pL->DataArray[pL->CheckNUM] = *pD;
					pL->CheckNUM++;
				}
				else
				{
					pR->DataArray[pR->CheckNUM] = *pD;
					pR->CheckNUM++;
				}
			}
			delete []p->DataArray;
			delete p;

			//Ϊ����������������Hash�������룬���ƫ��ĳһ�ߣ���ݹ�ʵ�֣�ֱ�����
			if((data.Index & index)==0)
			{
				if(pL->CheckNUM<pL->DataNUM)
				{
					pL->DataArray[pL->CheckNUM++] = data;
				}
				else
				{
					index = index>>1;
					delete []pR->DataArray;
					delete pR;
					blockNum--;
					delete node->rChild;
					node->rChild = NULL;
					return setNewNode(node->lChild,data,index);
				}

			}
			else
			{
				if(pR->CheckNUM<pR->DataNUM)
				{
					pR->DataArray[pR->CheckNUM++] = data;
				}
				else
				{

					index = index>>1;
					delete []pL->DataArray;
					delete pL;
					node->rChild->BlockNum = node->lChild->BlockNum;
					blockNum--;
					delete node->lChild;
					node->lChild = NULL;
					return setNewNode(node->rChild,data,index);
				}
				
			}
			writeBlock(node->lChild->BlockNum,pL);
			writeBlock(node->rChild->BlockNum,pR);
			delete []pL->DataArray;
			delete pL;
			delete []pR->DataArray;
			delete pR;
		}
		else
		{
			//�������õ�ɢ��λ������N��λ�������������
			p = new myHashBlock;
			p->CheckNUM = 0;
			p->DataNUM = bufferSize;
			p->DataArray = new myData[bufferSize];
			p->nextBlockNum = node->BlockNum;
			node->BlockNum = blockNum;
			blockNum++;
			p->DataArray[p->CheckNUM++] = data;
			writeBlock(node->BlockNum,p);
			delete []p->DataArray;
			delete p;
		}
	return true;
	}
	//ɾ��ĳһ������
	void deleteData(int current,myHashBlock *&pB)
	{
		int i;
		for(i=current;i<pB->CheckNUM;i++)
			if(i<pB->DataNUM-1)
				pB->DataArray[i]= pB->DataArray[i+1];
		pB->CheckNUM--;
		return ;
	}
	//�ж�ɾ��ĳһ���Ժ��丸�ڵ��������ڵ��Ƿ���Ժϲ�
	void checkCombine(myHashNode *parent)
	{
		int i;
		myHashBlock *pBL,*pBR ;
		pBL = readBlock(parent->lChild->BlockNum);
		pBR = readBlock(parent->rChild->BlockNum);
		if((pBL->CheckNUM+pBR->CheckNUM+2)>pBL->DataNUM)
			return ;
		parent->lChild = parent->rChild = NULL;
		parent->BlockNum = parent->lChild->BlockNum;
		for(i=0;i<pBR->CheckNUM;i++)
		{
			pBL->DataArray[pBL->CheckNUM+1] = pBR->DataArray[i];
			pBL->CheckNUM++;
		}
		writeBlock(parent->BlockNum,pBL);
		delete []pBR->DataArray;
		delete pBR;
		return ;
	}
};

int main()
{
	int choose,buSize=8;
	unsigned i,randNum,delIndex,cheIndex,j;
	string randData,s1,s2;
	bool flag,flag1 = false,flag2 = false;
	myData data1,data2,data3,*data4;
	myHashIndex myHash(buSize);
	BSzie = 2;
	ofstream out;
	char buf[10];
	clock_t start,finish;
	double duration;
	while(11)
	{
		system("cls");
		cout<<"Please choose the operation of the HashIndex:\n";
		cout<<"1.Set small number data test from newly created data\n";
		cout<<"2.Set small number data test from exit data\n";
		cout<<"3.Set great number data test from newly created data\n";
		cout<<"4.Set great number data test from exit data\n";
		cout<<"5.Save tree and Exit\n";
		cin>>choose;
		
		switch(choose)
		{
		case 1: 
			out.open("sData.dat",ios::out);
			if(!out)
			{
				cout<<"Output file open failed";
			
			}
			out.close();
			srand( (unsigned)time(NULL));
			myHash.TreeFile[0] = 's';
			myHash.DataFile[0] = 's';
			for(i=1,j=-1;i<500;i++,j--)
			{
				randNum = rand();
				data1.Index = randNum;
				sprintf(buf,"%d",randNum);
				strcpy(data1.Data ,"cai+"); 
				strcat(data1.Data,buf);
				myHash.Insert(data1);
			}
			break;
		case 2:
			myHash.TreeFile[0] = 's';
			myHash.DataFile[0] = 's';
			myHash.readTree();
			break;
		case 3:
			out.open("gData.dat",ios::out);
			if(!out)
			{
				cout<<"Output file open failed";
			
			}
			out.close();
			srand( (unsigned)time(NULL));
			myHash.TreeFile[0] = 'g';
			myHash.DataFile[0] = 'g';
			for(i=1;i<50000;i++)
			{
				randNum = i;
				data1.Index = randNum;
				sprintf(buf,"%d",randNum);
				strcpy(data1.Data ,"cai+"); 
				strcat(data1.Data,buf);
				myHash.Insert(data1);
			}
			break;
		case 4:
			myHash.TreeFile[0] = 'g';
			myHash.DataFile[0] = 'g';
			myHash.readTree();
			break;
		case 5:
			flag1 = true ;
			break;
		default:
			cout<<"Input Error!";
			continue;
		}
		if(flag1)
			break;
		flag2 = false;
		while(22)
		{
			system("cls");
			cout<<"Please choose the detail of the operation:\n";
			cout<<"1.Insert data\n";
			cout<<"2.Delete data\n";
			cout<<"3.Check data\n";
			cout<<"4.Save and exit\n";
			cin>>choose;
			switch (choose)
			{
			case 1:
				cout<<"Please input the index and the data:\n";
				cin>>data3.Index>>data3.Data;
				start=clock();
				flag = myHash.Insert(data3);
				if(flag)
					cout<<"Insert successed!\n";
				else 
					cout<<"Insert failed!\n";
				finish=clock();
				break;
			case 2:
				cout<<"Please input the index of the data which will be deleted:\n";
				cin>>delIndex;
				start=clock();
				flag = myHash.DeleteAll(delIndex);
				if(flag)
					cout<<"Delete successed!\n";
				else 
					cout<<"Delete failed!\n";
				finish=clock();
				break;
			case 3:
				cout<<"Please input the index of the data which will be checked:\n";
				cin>>cheIndex;
				start=clock();
				data4 = myHash.Check(cheIndex);
				if(data4!=NULL)
				{
					cout<<"The detail data of the checked is :\n";
					cout<<"Index is "<<data4->Index<<", data is "<<data4->Data<<endl;
				}
				else
				{
					cout<<"Check failed!\n";
				}
				finish=clock();
				break;
			case 4:
				start=clock();
				myHash.saveTree();
				flag2 = true;
				finish=clock();
				break;
			default:
				cout<<"Input Error!\n";
				break;
			}
			if(flag2)
				break;
			duration=(double)(finish-start);
			cout<<"The used of the time is "<<duration <<" ms!"<<endl;
			system("pause");
		}
	}
	return 0;
}