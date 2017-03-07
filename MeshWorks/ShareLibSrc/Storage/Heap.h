#pragma once

/************************************************************************/
/*                         ��С�ѽṹ ����ָ��Ƚ� <                    */
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

	~Heap()//ֱ�������ѿռ�
	{
		if(arrHeap)		{	delete []arrHeap;	arrHeap = NULL;	}	
	}

	void ReInit()//�������Ķѿռ�,������ԭ�е�����
	{
		if(!arrHeap)	return;
		Heap_Size = Heap_Size * 2;//׷�ӿռ�
		Type** arrOldHeap = arrHeap;//��¼ԭ����
		arrHeap = new Type*[Heap_Size];	if(!arrHeap)	exit(0);
		for(int i=0; i<=End; i++)//���ݴ�ŵ��� End ���ռ�
			arrHeap[i] = arrOldHeap[i];
		delete arrOldHeap;
	}

	void Set_Empty(void)//���ÿ�
	{	End = -1;	}

	int Is_Empty(void)//�ж϶��Ƿ�Ϊ��
	{	return End == -1;	}

	int Is_Full(void)//�ж϶��Ƿ�Ϊ��
	{	return End == Heap_Size-1;	}

	/************************************************************************/
	/*                               ��С�Ѳ���                             */
	/************************************************************************/

	void FilterDown(int start, int end)//��start��end�Զ����µ�������С��
	{
		if(!arrHeap)	return;
		Type* temp = arrHeap[start];//�ڱ�
		int j=2*start+1;//jΪ����Ůλ��

		while(j <= end)
		{
			if(j<end && !LessCompare(arrHeap[j],arrHeap[j+1]))	j++;//jָ��ֵ��С����Ů�ڵ�
			if(LessCompare(temp,arrHeap[j]))	break;//�ڵ�ֵС��������	//�þ���ľ���ֵ
			arrHeap[start] = arrHeap[j];

			arrHeap[j]->Heap_Index = start;//��¼�õ��ڶ������е�λ��
			temp->Heap_Index = j;

			start = j;
			j=start*2+1;
		}
		arrHeap[start] = temp;

		arrHeap[start]->Heap_Index = temp->Heap_Index;//��¼�õ��ڶ������е�λ��
	}

	void FilterUp(int start)//��start��0�Ե����ϵ�������С��
	{
		if(!arrHeap)	return;
		Type* temp = arrHeap[start];//�ڱ�
		int i=(start-1)/2;//iΪ��˫�׽ڵ��λ��

		while(start > 0)//��˫�׽ڵ�����ֱ�����ڵ�			
		{
			if(LessCompare(arrHeap[i],temp))	break;//˫�׽ڵ�ֵС��������
			arrHeap[start] = arrHeap[i];

			arrHeap[i]->Heap_Index = start;//��¼�õ��ڶ������е�λ��
			temp->Heap_Index = i;

			start = i;
			i=(start-1)/2;
		}
		arrHeap[start] = temp;

		arrHeap[start]->Heap_Index = temp->Heap_Index;//��¼�õ��ڶ������е�λ��

	}

	void Insert(Type* &pobject)//��Ԫ�ز��뵽��С����
	{
		if(!arrHeap)	return;//�Ѳ�����
		if(End == Heap_Size-1)//������
			ReInit();

		End++;
		arrHeap[End] = pobject;

		pobject->Heap_Index = End;//��¼�õ��ڶ������е�λ��

		FilterUp(End);//�����ýڵ�λ��
	}

	bool Remove(Type* &pobject)//ȡ����СԪ��	//����ǰһ��Ԫ��
	{
		if(End == -1)	return false;//�ѿջ�Ѳ�����
		pobject = arrHeap[0];//�����һ��Ԫ��ֵ

		arrHeap[0] = arrHeap[End];//���һ��Ԫ�ص�������һ��λ��
		End--;//���һ��Ԫ���Ѿ���������һ��λ��

		FilterDown(0, End);//��������С��

		return true;
	}

	void GenHeap()//������������С��
	{	
		if(!arrHeap)	return;
		int i=(End-1)/2;
		while(i>=0)
		{
			FilterDown(i, End);
			i--;
		}
	}

	int Heap_Size;//�ѳ���
	int End;//�����һ��Ԫ���������е�λ��
	Type** arrHeap;//ָ����С�������ָ��
	
	Less LessCompare;//С�ڱȽ�

};
