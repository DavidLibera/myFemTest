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
	//���캯��
	MinHeap()
	{
		Heap=new Type[DefaultSize];			//�������½�һ������
		MaxHeapSize=DefaultSize;			//��ʼ���������ֵ
		CurrentSize=0;						//��ʼ�����鵱ǰ����
	}
	MinHeap(int MaxSize)			//MaSize ��С������ĳ���
	{
		MaxHeapSize=(DefaultSize>MaxSize)? DefaultSize:MaxSize;	
											//��ʼ���������ֵ				
		Heap=new Type[MaxHeapSize];			//�������½�һ������
		CurrentSize=0;						//��ʼ�����鵱ǰ����
	}
	MinHeap(Type arr[],int Size)	//arr:������С��ʱ������; n:arr������Ч���ݵĳ���
	{
		int CurrentPos;						//����һ���α꣬������С��ʱʹ��
		
		MaxHeapSize=(DefaultSize>Size)? DefaultSize : Size ;						
											//��ʼ���������ֵ����СΪDefaultSize
		Heap=new Type[MaxHeapSize];			//�������½�һ������
		for(CurrentSize=0;CurrentSize<Size;CurrentSize++)
			Heap[CurrentSize]=arr[CurrentSize];
											//�������鸳��ֵ
		CurrentPos=(CurrentSize-1)/2;		//�ӵ����ڶ������һԪ�ؿ�ʼ���϶��µ�������
		while (CurrentPos>=0)				//�ȵ����öѵĵײ㣬��������߲������
			FilterDown(CurrentPos--,CurrentSize-1);	
											//���µ�����һ������λ��ֵ�ϵ�Ԫ���������λ��
	}
	MinHeap(MinHeap<Type> &Original)	//�������캯��
	{
		MaxHeapSize=Original.MaxHeapSize;	//��ʼ���������ֵ
		Heap=new Type[MaxHeapSize];			//������һ������ռ䣬��СΪ��Ŀ������ռ���ͬ
		for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
			Heap[CurrentSize]=Original.Heap[CurrentSize];
										//������Ԫ��һһ���ƹ���
	}
	
	//��������
	virtual ~MinHeap()
	{
		delete [] Heap;						//�ͷ�����ռ�
	}


	//��������
	void MakeEmpty()					//�����С��
	{
		CurrentSize=0;						//������������˰�...
	}

	bool RemoveMin(Type *Node)		//�Ƴ���С�ѵĶ��㡣�����ö������Node
	{
		if(IsEmpty())						//���ö��Ƿ�Ϊ�ն�
			return false;
		*Node=Heap[0];
		Heap[0]=Heap[--CurrentSize];		//�����鳤�ȼ�1���������һ��Ԫ�طŵ��Ѷ���
		FilterDown(0,CurrentSize-1);		//�Զ������ײ������С��
		return true;
	}

	bool Insert(const Type Node,bool addnew=false)	//������Ԫ��
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
	bool GetMin(Type &Node)			//�õ���Сֵ��������
	{
		if(IsEmpty())						//���ö��Ƿ�Ϊ�ն�					
			return false;
		else
			Node=Heap[0];					//�ǿն�����СԪ�ظ�ֵ��Node
		return true;

	}
	bool Copy(MinHeap<Type> &Original)  // ������С��
	{
		if(MaxHeapSize< Original.GetMaxSize())
			return false;
		else
			for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
				Heap[CurrentSize]=Original.Heap[CurrentSize];
											//������Ԫ��һһ���ƹ���
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

		if(IsEmpty())						//���ö��Ƿ�Ϊ�ն�					
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
	
	
	
	int GetMaxSize()  //�õ���С�ѵ����ռ�
	{
		return MaxHeapSize;
	}
	int GetValidSize() //�õ���С�ѵ���Ч���ݸ���
	{	
		return CurrentSize;
	}	
public:									
	//��������
	bool IsFull() const	{				//�ж���С���Ƿ�����
		return CurrentSize==MaxHeapSize;
	}
	bool IsEmpty() const{				//�ж���С���Ƿ�Ϊ��
		return CurrentSize==0;
	}
//protected:

	//������С�Ѻ���
	void FilterUp(int i)			//�ɵ����ϵ�����С��,��i������ϲ��ƶ���һ����ȷλ��
	{
		if(i<1)								//���i�Ƿ�Ϸ�
			return;
		i=(CurrentSize>i)? i:CurrentSize-1;	//���i�Ƿ�������鳤��
		int j=(i-1)/2;					//jΪi�ĸ����
		Type temp=Heap[i];					//��i����ݴ�
		while(i>0)							//��δ����������
		{
			if (Heap[j]<=temp)				//�������С��i����ֵ
				break;						//������С��Ҫ�󣬲��ٵ����÷�֧
			else							//���� 
			{
				Heap[i]=Heap[j];			//��������ֵ���������ӽ��
				i=j;						//�������ϵ�����iָ��������Ԫ�ص�λ��
				j=(i-1)/2;					//jָ��i�ĸ����
			}
		}
		Heap[i]=temp;						//���ݴ�Ľ����뵽iλ��
	}
	
	void FilterDown(int i,int m)//���϶��µ�����С�ѣ���i���²��ƶ���һ��m���ǰ����ȷ��λ��
	{
		if(((int)m<0)||m<=i||m>(CurrentSize-1)||i<0)		//���i,m�ĺϷ��� 
			return;
		unsigned j=2*i+1;					//����jΪi�����ӽ��
		Type temp=Heap[i];					//�ݴ����Ҫ�ƶ���i���
		while(j<=m)							//��j������m���ʱִ��ѭ��
		{
			if(j<m&&Heap[j]<=Heap[j+1])		//j��j+1ͬΪi���ӽ�㣬��jָ��Ƚ�С���ӽ��
				j++;						//����ָ���ҽ�㣬���������������Ķ�һ��������С�ڵ����ҽ��
			if(temp<=Heap[j])				//��:i��㲻���ڽ�С���ӽ��
				break;						//������С���������˳�ѭ��
			else							//����
			{
				Heap[i]=Heap[j];			//����С���ӽ������i���
				i=j;						//����������iָ���������ݵ��ӽ��
				j=2*i+1;					//jָ��i�����ӽ�� 
			}
		}
		Heap[i]=temp;						//�ҵ������i������ȷλ�� ���������ȥ
	}

private:
	//�ڲ�����			
	Type * Heap;						//Ԫ������ָ��		
	int MaxHeapSize;				//���Ԫ�ظ���
	int CurrentSize;				//���鵱ǰ��Ч����
	enum{DefaultSize= 20};				//����Ĭ�ϳ���

};

#endif // !defined(AFX_MINHEAP_H__9403D663_70D4_48D3_9D8E_594D305E15C4__INCLUDED_)
