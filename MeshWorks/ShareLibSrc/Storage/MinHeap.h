// MinHeap.h: interface for the MinHeap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINHEAP_H__9403D663_70D4_48D3_9D8E_594D305E15C4__INCLUDED_)
#define AFX_MINHEAP_H__9403D663_70D4_48D3_9D8E_594D305E15C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXHEAPSIZE 1000;

template <class Type> class MinHeap  
{
public:
	//构造函数
	MinHeap()
	{
		Heap=new Type[DefaultSize];			//给该类新建一个数组
		MaxHeapSize=DefaultSize;			//初始化数组最大值
		CurrentSize=0;						//初始化数组当前长度
	}
	MinHeap(int MaxSize)			//MaSize 最小堆数组的长度
	{
		MaxHeapSize=(DefaultSize>MaxSize)? DefaultSize:MaxSize;	
											//初始化数组最大值				
		Heap=new Type[MaxHeapSize];			//给该类新建一个数组
		CurrentSize=0;						//初始化数组当前长度
	}
	MinHeap(Type arr[],int Size)	//arr:构造最小堆时的数组; n:arr数组有效内容的长度
	{
		int CurrentPos;						//定义一个游标，调整最小堆时使用
		
		MaxHeapSize=(DefaultSize>Size)? DefaultSize : Size ;						
											//初始化数组最大值，最小为DefaultSize
		Heap=new Type[MaxHeapSize];			//给该类新建一个数组
		for(CurrentSize=0;CurrentSize<Size;CurrentSize++)
			Heap[CurrentSize]=arr[CurrentSize];
											//给新数组赋初值
		CurrentPos=(CurrentSize-1)/2;		//从倒数第二层最后一元素开始自上而下调整数组
		while (CurrentPos>=0)				//先调整好堆的底层，再慢慢向高层调调整
			FilterDown(CurrentPos--,CurrentSize-1);	
											//往下调整第一个参数位置值上的元素至合理的位置
	}
	MinHeap(MinHeap<Type> &Original)	//拷贝构造函数
	{
		MaxHeapSize=Original.MaxHeapSize;	//初始化数组最大值
		Heap=new Type[MaxHeapSize];			//新申请一个数组空间，大小为与目标数组空间相同
		for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
			Heap[CurrentSize]=Original.Heap[CurrentSize];
										//将数组元素一一复制过来
	}
	
	//析构函数
	virtual ~MinHeap()
	{
		delete [] Heap;						//释放数组空间
	}


	//其它函数
	void MakeEmpty()					//清空最小堆
	{
		CurrentSize=0;						//这样就算清空了吧...
	}

	bool RemoveMin(Type *Node)		//移除最小堆的顶点。并将该顶点存入Node
	{
		if(IsEmpty())						//检查该堆是否为空堆
			return false;
		*Node=Heap[0];
		Heap[0]=Heap[--CurrentSize];		//将数组长度减1，并将最后一个元素放到堆顶点
		FilterDown(0,CurrentSize-1);		//自顶点至底层调整最小堆
		return true;
	}

	bool Insert(const Type Node,bool addnew=false)	//插入新元素
	{
		if (IsFull()&&!addnew)
			return (false);	
		if(IsFull())
		{
			MaxHeapSize+=DefaultSize;
			Type *temp=new Type[MaxHeapSize];
			for(unsigned x=0;x<CurrentSize;x++)
				temp[x]=Heap[x];
			delete []Heap;
			Heap=temp;
		}
		Heap[CurrentSize]=Node;
		FilterUp(CurrentSize++);
		return true;

	}
	bool GetMin(Type &Node)			//得到最小值，即顶点
	{
		if(IsEmpty())						//检查该堆是否为空堆					
			return false;
		else
			Node=Heap[0];					//非空堆则将最小元素赋值给Node
		return true;

	}
	bool Copy(MinHeap<Type> &Original)  // 拷贝最小堆
	{
		if(MaxHeapSize< Original.GetMaxSize())
			return false;
		else
			for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
				Heap[CurrentSize]=Original.Heap[CurrentSize];
											//将数组元素一一复制过来
	}

	bool GetNode(Type *Node,int pos)
	{
		if(pos > MaxHeapSize)
			return false;
		*Node = Heap[pos];
		return true;

	}
	bool UpdateNode(Type &Node, int pos)
	{
		if(pos > CurrentSize)
			return false;
		Type temp;
		temp = Heap[pos];

		Heap[pos]=Node;
		if(!(Heap[pos]<=temp))
		{
			FilterDown(pos,CurrentSize-1);
		}
		if(!(temp<=Heap[pos]))
		{
			FilterUp(pos);
		}
		return true;
	}
	int GetPos(Type &Node)
	{
		int pos=-1;

		if(IsEmpty())						//检查该堆是否为空堆					
			return pos;

		for(pos=0;pos<CurrentSize;pos++)
		{
			if(Node == Heap[pos])
				break;
		}

		return pos;
	}
	Type * GetNode(int pos)
	{
		if(pos > MaxHeapSize)
			return NULL;
		
		return  &Heap[pos];
	}
	
	
	
	int GetMaxSize()  //得到最小堆的最大空间
	{
		return MaxHeapSize;
	}
	int GetValidSize() //得到最小堆的有效数据个数
	{	
		return CurrentSize;
	}	
public:									
	//内联函数
	bool IsFull() const	{				//判断最小堆是否已满
		return CurrentSize==MaxHeapSize;
	}
	bool IsEmpty() const{				//判断最小堆是否为空
		return CurrentSize==0;
	}
//protected:

	//调整最小堆函数
	void FilterUp(int i)			//由底向上调整最小堆,将i结点向上层移动到一个正确位置
	{
		if(i<1)								//检查i是否合法
			return;
		i=(CurrentSize>i)? i:CurrentSize-1;	//检查i是否大于数组长度
		int j=(i-1)/2;					//j为i的父结点
		Type temp=Heap[i];					//将i结点暂存
		while(i>0)							//若未调整至顶点
		{
			if (Heap[j]<=temp)				//若父结点小于i结点的值
				break;						//符合最小堆要求，不再调整该分支
			else							//否则 
			{
				Heap[i]=Heap[j];			//将父结点的值向下移至子结点
				i=j;						//继续向上调整，i指向已移走元素的位置
				j=(i-1)/2;					//j指向i的父结点
			}
		}
		Heap[i]=temp;						//将暂存的结点插入到i位置
	}
	
	void FilterDown(int i,int m)//从上而下调整最小堆，将i向下层移动至一个m结点前的正确的位置
	{
		if(((int)m<0)||m<=i||m>(CurrentSize-1)||i<0)		//检查i,m的合法性 
			return;
		unsigned j=2*i+1;					//定义j为i的左子结点
		Type temp=Heap[i];					//暂存可能要移动的i结点
		while(j<=m)							//当j不超过m结点时执行循环
		{
			if(j<m&&Heap[j]<=Heap[j+1])		//j与j+1同为i的子结点，让j指向比较小的子结点
				j++;						//优先指向右结点，这样，调整出来的堆一般是左结点小于等于右结点
			if(temp<=Heap[j])				//若:i结点不大于较小的子结点
				break;						//符合最小堆条件，退出循环
			else							//否则
			{
				Heap[i]=Heap[j];			//将较小的子结点移至i结点
				i=j;						//继续调整，i指向被移走数据的子结点
				j=2*i+1;					//j指向i的左子结点 
			}
		}
		Heap[i]=temp;						//找到了最初i结点的正确位置 ，将它插进去
	}

private:
	//内部数据			
	Type * Heap;						//元素数组指针		
	int MaxHeapSize;				//最大元素个数
	int CurrentSize;				//数组当前有效长度
	enum{DefaultSize= 20};				//数组默认长度

};

#endif // !defined(AFX_MINHEAP_H__9403D663_70D4_48D3_9D8E_594D305E15C4__INCLUDED_)
