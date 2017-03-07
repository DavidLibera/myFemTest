#ifndef _MINHEAP_H_
#define _MINHEAP_H_

#define MAXHEAPSIZE 10000;

template <class Type> class minheap		//����һ��������ʽ����С��
{										//Ҫ��Type֧�� <=��=, == ������ 
public:
	
	//���캯��
	minheap();							
	minheap(unsigned MaxSize);			//MaSize ��С������ĳ���
	minheap(Type arr[],unsigned Size);	//arr:������С��ʱ������; n:arr������Ч���ݵĳ���
	minheap(minheap<Type> &Original);	//�������캯��
	
	//��������
	~minheap();

	//��������
	void MakeEmpty();					//�����С��
	bool RemoveMin(Type *Node=0);		//�Ƴ���С�ѵĶ��㡣�����ö������Node
	bool Insert(const Type Node,bool addnew=false);
										//������Ԫ��
	bool GetMin(Type &Node);			//�õ���Сֵ��������
	
	bool Copy(minheap<Type> &Original);
	
	int GetMaxSize();
	int GetValidSize();
	bool GetNode(Type *Node,int pos);
	bool UpdateNode(Type &Node, int pos);
	int GetPos(Type &Node);
	Type * GetNode(int pos);
public:									
	//��������
	bool IsFull() const	{				//�ж���С���Ƿ�����
		return CurrentSize==MaxHeapSize;
	}
	bool IsEmpty() const{				//�ж���С���Ƿ�Ϊ��
		return CurrentSize==0;
	}
		
protected:

	//������С�Ѻ���
	void FilterUp(unsigned i);			//�ɵ����ϵ�����С��,��i������ϲ��ƶ���һ����ȷλ��
	void FilterDown(unsigned i,unsigned m);	
										//���϶��¸�����С�ѣ���i���²��ƶ���һ��m���ǰ����ȷ��λ��

private:
	//�ڲ�����			
	Type * Heap;						//Ԫ������ָ��		
	unsigned MaxHeapSize;				//���Ԫ�ظ���
	unsigned CurrentSize;				//���鵱ǰ��Ч����
	enum{DefaultSize= 20};				//����Ĭ�ϳ���
};


////////////////////////////////////////////////////////////////////////////////////////////////
/////////////																	////////////////
/////////////				�������ຯ����ʵ��									////////////////
/////////////																	////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
template <class Type> minheap<Type>::minheap()
{
	Heap=new Type[DefaultSize];			//�������½�һ������
	MaxHeapSize=DefaultSize;			//��ʼ���������ֵ
	CurrentSize=0;						//��ʼ�����鵱ǰ����
}

//���캯��  MaSize ��С������ĳ���
template <class Type> minheap<Type>::minheap(unsigned MaxSize)
{
	MaxHeapSize=(DefaultSize>MaxSize)? DefaultSize:MaxSize;	
										//��ʼ���������ֵ				
	Heap=new Type[MaxHeapSize];			//�������½�һ������
	CurrentSize=0;						//��ʼ�����鵱ǰ����
}

//���캯��  arr:������С��ʱ������; n:arr������Ч���ݵĳ���
template <class Type> minheap<Type>::minheap(Type arr[],unsigned Size)
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

//�������캯��
template <class Type> minheap<Type>::minheap(minheap<Type> &Original)
{
	MaxHeapSize=Original.MaxHeapSize;	//��ʼ���������ֵ
	Heap=new Type[MaxHeapSize];			//������һ������ռ䣬��СΪ��Ŀ������ռ���ͬ
	for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
		Heap[CurrentSize]=Original.Heap[CurrentSize];
										//������Ԫ��һһ���ƹ���
}

//��������
template <class Type> minheap<Type>::~minheap()
{
	delete [] Heap;						//�ͷ�����ռ�
}

//�ɵ����ϵ�����С��,��i������ϲ��ƶ���һ����ȷλ��
template <class Type> void minheap<Type>::FilterUp(unsigned i)
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

//���϶��¸�����С�ѣ���i���²��ƶ���һ��m���ǰ����ȷ��λ��
template <class Type> void minheap<Type>::FilterDown(unsigned i,unsigned m)
{
	if(((int)m<0)||m<=i||m>CurrentSize-1||i<0)		//���i,m�ĺϷ��� 
		return;
	unsigned j=2*i+1;					//����jΪi�����ӽ��
	Type temp=Heap[i];					//�ݴ����Ҫ�ƶ���i���
	while(j<=m)							//��j������m���ʱִ��ѭ��
	{
		if(j<m&&Heap[j+1]<=Heap[j])		//j��j+1ͬΪi���ӽ�㣬��jָ��Ƚ�С���ӽ��
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

//�����С��
template <class Type> void minheap<Type>::MakeEmpty()
{
	CurrentSize=0;						//������������˰�...
}	

//�Ƴ���С�ѵĶ��㡣�����ö������Node	
template <class Type> bool minheap<Type>::RemoveMin(Type *Node)
{
	if(IsEmpty())						//���ö��Ƿ�Ϊ�ն�
		return false;
	if(Node)							//����Ƿ���Ҫ����Сֵ����*Node
		*Node=Heap[0];
	Heap[0]=Heap[--CurrentSize];		//�����鳤�ȼ�1���������һ��Ԫ�طŵ��Ѷ���
	FilterDown(0,CurrentSize-1);		//�Զ������ײ������С��
	return true;
}

//������Ԫ��	
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
		
//�õ���Сֵ��������	
template <class Type> bool minheap<Type>::GetMin(Type &Node)
{
	if(IsEmpty())						//���ö��Ƿ�Ϊ�ն�					
		return false;
	else
		Node=Heap[0];					//�ǿն�����СԪ�ظ�ֵ��Node
	return true;
}

//������С��
template <class Type> bool minheap<Type>::Copy(minheap<Type> &Original)
{
	if(MaxHeapSize< Original.GetMaxSize())
		return false;
	else
		for(CurrentSize=0;CurrentSize<Original.CurrentSize;CurrentSize++)
			Heap[CurrentSize]=Original.Heap[CurrentSize];
										//������Ԫ��һһ���ƹ���
}

//ȡ����С�ѵ��������
template <class Type> int minheap<Type>::GetMaxSize()
{
	return MaxHeapSize;
}

//ȡ����С�ѵ�ǰ��ЧԪ�ظ���
template <class Type> int minheap<Type>::GetValidSize()
{
	return CurrentSize;
}

//��ȡ��С����λ��Ϊpos��Ԫ�أ�����Node
template <class Type> bool minheap<Type>::GetNode(Type *Node, int pos)
{
	if(pos > MaxHeapSize)
		return false;
	*Node = Heap[pos];
	return true;

}
//��ȡ��С����λ��Ϊpos��Ԫ�ص�ַ
template <class Type> Type * minheap<Type>::GetNode(int pos)
{
	if(pos > MaxHeapSize)
		return NULL;
	
	return  &Heap[pos];
}
//��ȡ��С���е���Node��Ԫ��λ��
template <class Type> int minheap<Type>::GetPos(Type &Node)
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

//������С����λ��Ϊpos��Ԫ�����ݣ�������Ӧ������С��
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