#ifndef _MINHEAP_H_
#define _MINHEAP_H_

#define MAXHEAPSIZE 10000;

template <class Type> class minheap		//这是一个数组形式的最小堆
{										//要求Type支持 <=，=, == 操作符 
public:
	
	//构造函数
	minheap();							
	minheap(unsigned MaxSize);			//MaSize 最小堆数组的长度
	minheap(Type arr[],unsigned Size);	//arr:构造最小堆时的数组; n:arr数组有效内容的长度
	minheap(minheap<Type> &Original);	//拷贝构造函数
	
	//析构函数
	~minheap();

	//其它函数
	void MakeEmpty();					//清空最小堆
	bool RemoveMin(Type *Node=0);		//移除最小堆的顶点。并将该顶点存入Node
	bool Insert(const Type Node,bool addnew=false);
										//插入新元素
	bool GetMin(Type &Node);			//得到最小值，即顶点
	
	bool Copy(minheap<Type> &Original);
	
	int GetMaxSize();
	int GetValidSize();
	bool GetNode(Type *Node,int pos);
	bool UpdateNode(Type &Node, int pos);
	int GetPos(Type &Node);
	Type * GetNode(int pos);
public:									
	//内联函数
	bool IsFull() const	{				//判断最小堆是否已满
		return CurrentSize==MaxHeapSize;
	}
	bool IsEmpty() const{				//判断最小堆是否为空
		return CurrentSize==0;
	}
		
protected:

	//调整最小堆函数
	void FilterUp(unsigned i);			//由底向上调整最小堆,将i结点向上层移动到一个正确位置
	void FilterDown(unsigned i,unsigned m);	
										//从上而下高速最小堆，将i向下层移动至一个m结点前的正确的位置

private:
	//内部数据			
	Type * Heap;						//元素数组指针		
	unsigned MaxHeapSize;				//最大元素个数
	unsigned CurrentSize;				//数组当前有效长度
	enum{DefaultSize= 20};				//数组默认长度
};


////////////////////////////////////////////////////////////////////////////////////////////////
/////////////																	////////////////
/////////////				下面是类函数的实现									////////////////
/////////////																	////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
template <class Type> minheap<Type>::minheap()
{
	Heap=new Type[DefaultSize];			//给该类新建一个数组
	MaxHeapSize=DefaultSize;			//初始化数组最大值
	CurrentSize=0;						//初始化数组当前长度
}

//构造函数  MaSize 最小堆数组的长度
template <class Type> minheap<Type>::minheap(unsigned MaxSize)
{
	MaxHeapSize=(DefaultSize>MaxSize)? DefaultSize:MaxSize;	
										//初始化数组最大值				
	Heap=new Type[MaxHeapSize];			//给该类新建一个数组
	CurrentSize=0;						//初始化数组当前长度
}

//构造函数  arr:构造最小堆时的数组; n:arr数组有效内容的长度
template <class Type> minheap<Type>::minheap(Type arr[],unsigned Size)
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

//拷贝构造函数
template <class Type> minheap<Type>::minheap(minheap<Type> &Original)
{
	MaxHeapSize=Original.MaxHeapSize;	//初始化数组最大值
	Heap=new Type[MaxHeapSize];			//新申请一个数组空间，大小为与目标数组空间相同
	for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
		Heap[CurrentSize]=Original.Heap[CurrentSize];
										//将数组元素一一复制过来
}

//析构函数
template <class Type> minheap<Type>::~minheap()
{
	delete [] Heap;						//释放数组空间
}

//由底向上调整最小堆,将i结点向上层移动到一个正确位置
template <class Type> void minheap<Type>::FilterUp(unsigned i)
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

//从上而下高速最小堆，将i向下层移动至一个m结点前的正确的位置
template <class Type> void minheap<Type>::FilterDown(unsigned i,unsigned m)
{
	if(((int)m<0)||m<=i||m>CurrentSize-1||i<0)		//检查i,m的合法性 
		return;
	unsigned j=2*i+1;					//定义j为i的左子结点
	Type temp=Heap[i];					//暂存可能要移动的i结点
	while(j<=m)							//当j不超过m结点时执行循环
	{
		if(j<m&&Heap[j+1]<=Heap[j])		//j与j+1同为i的子结点，让j指向比较小的子结点
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

//清空最小堆
template <class Type> void minheap<Type>::MakeEmpty()
{
	CurrentSize=0;						//这样就算清空了吧...
}	

//移除最小堆的顶点。并将该顶点存入Node	
template <class Type> bool minheap<Type>::RemoveMin(Type *Node)
{
	if(IsEmpty())						//检查该堆是否为空堆
		return false;
	if(Node)							//检查是否需要将最小值存入*Node
		*Node=Heap[0];
	Heap[0]=Heap[--CurrentSize];		//将数组长度减1，并将最后一个元素放到堆顶点
	FilterDown(0,CurrentSize-1);		//自顶点至底层调整最小堆
	return true;
}

//插入新元素	
template <class Type> bool minheap<Type>::Insert(const Type Node,bool addnew)
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
		
//得到最小值，即顶点	
template <class Type> bool minheap<Type>::GetMin(Type &Node)
{
	if(IsEmpty())						//检查该堆是否为空堆					
		return false;
	else
		Node=Heap[0];					//非空堆则将最小元素赋值给Node
	return true;
}

//拷贝最小堆
template <class Type> bool minheap<Type>::Copy(minheap<Type> &Original)
{
	if(MaxHeapSize< Original.GetMaxSize())
		return false;
	else
		for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
			Heap[CurrentSize]=Original.Heap[CurrentSize];
										//将数组元素一一复制过来
}

//取得最小堆的最大容量
template <class Type> int minheap<Type>::GetMaxSize()
{
	return MaxHeapSize;
}

//取得最小堆当前有效元素个数
template <class Type> int minheap<Type>::GetValidSize()
{
	return CurrentSize;
}

//获取最小堆中位置为pos的元素，放入Node
template <class Type> bool minheap<Type>::GetNode(Type *Node, int pos)
{
	if(pos > MaxHeapSize)
		return false;
	*Node = Heap[pos];
	return true;

}
//获取最小堆中位置为pos的元素地址
template <class Type> Type * minheap<Type>::GetNode(int pos)
{
	if(pos > MaxHeapSize)
		return NULL;
	
	return  &Heap[pos];
}
//获取最小堆中等于Node的元素位置
template <class Type> int minheap<Type>::GetPos(Type &Node)
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

//更新最小堆中位置为pos的元素内容，并且相应更新最小堆
template <class Type> bool minheap<Type>::UpdateNode(Type &Node, int pos)
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

#endif