#pragma once

/************************************************************************/
/*                         最小堆结构 函数指针比较 <                    */
/************************************************************************/

template <class Type, class Less> class Heap
{
public:

	Heap(int heapsize)
	{
		Heap_Size = max(heapsize, 1);
		End = -1;
		arrHeap = new Type*[Heap_Size];	if(!arrHeap)	exit(0);
	}

	~Heap()//直接析构堆空间
	{
		if(arrHeap)		{	delete []arrHeap;	arrHeap = NULL;	}	
	}

	void ReInit()//构造更大的堆空间,并保持原有的数据
	{
		if(!arrHeap)	return;
		Heap_Size = Heap_Size * 2;//追加空间
		Type** arrOldHeap = arrHeap;//记录原数组
		arrHeap = new Type*[Heap_Size];	if(!arrHeap)	exit(0);
		for(int i=0; i<=End; i++)//数据存放到第 End 个空间
			arrHeap[i] = arrOldHeap[i];
		delete arrOldHeap;
	}

	void Set_Empty(void)//堆置空
	{	End = -1;	}

	int Is_Empty(void)//判断堆是否为空
	{	return End == -1;	}

	int Is_Full(void)//判断堆是否为满
	{	return End == Heap_Size-1;	}

	/************************************************************************/
	/*                               最小堆操作                             */
	/************************************************************************/

	void FilterDown(int start, int end)//自start到end自顶向下调整成最小堆
	{
		if(!arrHeap)	return;
		Type* temp = arrHeap[start];//哨兵
		int j=2*start+1;//j为左子女位置

		while(j <= end)
		{
			if(j<end && !LessCompare(arrHeap[j],arrHeap[j+1]))	j++;//j指向值较小的子女节点
			if(LessCompare(temp,arrHeap[j]))	break;//节点值小则不做调整	//用距离的绝对值
			arrHeap[start] = arrHeap[j];

			arrHeap[j]->Heap_Index = start;//记录该点在堆数组中的位置
			temp->Heap_Index = j;

			start = j;
			j=start*2+1;
		}
		arrHeap[start] = temp;

		arrHeap[start]->Heap_Index = temp->Heap_Index;//记录该点在堆数组中的位置
	}

	void FilterUp(int start)//自start到0自底向上调整成最小堆
	{
		if(!arrHeap)	return;
		Type* temp = arrHeap[start];//哨兵
		int i=(start-1)/2;//i为左双亲节点的位置

		while(start > 0)//沿双亲节点向上直到根节点			
		{
			if(LessCompare(arrHeap[i],temp))	break;//双亲节点值小则不做调整
			arrHeap[start] = arrHeap[i];

			arrHeap[i]->Heap_Index = start;//记录该点在堆数组中的位置
			temp->Heap_Index = i;

			start = i;
			i=(start-1)/2;
		}
		arrHeap[start] = temp;

		arrHeap[start]->Heap_Index = temp->Heap_Index;//记录该点在堆数组中的位置

	}

	void Insert(Type* &pobject)//将元素插入到最小堆中
	{
		if(!arrHeap)	return;//堆不存在
		if(End == Heap_Size-1)//堆已满
			ReInit();

		End++;
		arrHeap[End] = pobject;

		pobject->Heap_Index = End;//记录该点在堆数组中的位置

		FilterUp(End);//调整该节点位置
	}

	bool Remove(Type* &pobject)//取出最小元素	//堆最前一个元素
	{
		if(End == -1)	return false;//堆空或堆不存在
		pobject = arrHeap[0];//输出第一个元素值

		arrHeap[0] = arrHeap[End];//最后一个元素调整到第一个位置
		End--;//最后一个元素已经调整到第一个位置

		FilterDown(0, End);//调整成最小堆

		return true;
	}

	void GenHeap()//由数组生成最小堆
	{	
		if(!arrHeap)	return;
		int i=(End-1)/2;
		while(i>=0)
		{
			FilterDown(i, End);
			i--;
		}
	}

	int Heap_Size;//堆长度
	int End;//堆最后一个元素在数组中的位置
	Type** arrHeap;//指向最小堆数组的指针
	
	Less LessCompare;//小于比较

};
