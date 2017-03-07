// GLKObList.h: interface for the GLKObList class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _CW_GLKObject
#define _CW_GLKObject

#define NULL	0
#include "../GLKernel/GLEntity.h"
class GLKObject
{
public:
	GLKObject () {};
	int type; //0 for QMeshNode
			  //1 for QMeshEdge
			  //2 for QMeshFace
			  //3 for CH_ATTRIB_EDGENODE
			  //4 for QMeshTetra
			  //5 for QMeshHexa
};

#endif



#ifndef _CW_GLKObNode
#define _CW_GLKObNode
//#define GLKPOSITION		GLKObNode*


class GLKObNode
{
public:
	GLKObNode(GLKObNode* ptrprev=NULL,GLKObNode* ptrnext=NULL)	{next=ptrnext;prev=ptrprev;};

	void InsertAfter(GLKObNode *p) {
		GLKObNode *oldNextNode=next;
		next=p;p->prev=this;
		if (oldNextNode) {oldNextNode->prev=p;p->next=oldNextNode;}
	};
	GLKObNode *DeleteAfter() {
		GLKObNode *tempObj=next;
		if (next==NULL) return NULL;
		next=tempObj->next;
		next->prev=this;
		return tempObj;
	};

	void InsertBefore(GLKObNode *p) {
		GLKObNode *oldPrevNode=prev;
		prev=p;p->next=this;
		if (oldPrevNode) {oldPrevNode->next=p;p->prev=oldPrevNode;}
	};
	GLKObNode *DeleteBefore() {
		GLKObNode *tempObj=prev;
		if (prev==NULL) return NULL;
		prev=tempObj->prev;
		prev->next=this;
		return tempObj;
	};

	GLKObNode *next;
	GLKObNode *prev;
	GLKObject *data;
};
typedef GLKObNode* GLKPOSITION;
#endif



#ifndef _CW_GLKObList
#define _CW_GLKObList

class GLKObList  
{
public:
	GLKObList();
	virtual ~GLKObList();

	GLKObject* GetHead();
	GLKObject* GetTail();
	GLKPOSITION GetHeadPosition() {return headPos;};
	GLKPOSITION GetTailPosition() {return tailPos;};
	GLKPOSITION FindIndex(int index);
	GLKPOSITION Find(GLKObject* element);

	GLKPOSITION AddHead( GLKObject* newElement );
	void AddHead( GLKObList* pNewList );
	GLKPOSITION AddTail( GLKObject* newElement );
	void AddTail( GLKObList* pNewList );
	GLKObject* RemoveHead();
	GLKObject* RemoveTail();
	GLKObject* RemoveAt(GLKPOSITION rPosition);
	void Remove(GLKObject* element);
	void RemoveAll();

	GLKObject* GetNext( GLKPOSITION& rPosition );
	GLKObject* GetPrev( GLKPOSITION& rPosition );
	GLKObject* GetNext( GLKPOSITION& rPosition, bool direction);

	GLKObject* GetAt( GLKPOSITION rPosition );

	GLKPOSITION InsertBefore( GLKPOSITION rPosition, GLKObject* newElement );
	GLKPOSITION InsertAfter( GLKPOSITION rPosition, GLKObject* newElement );

	int GetCount() {return nCount;};

	bool IsEmpty() {return ((nCount==0)?true:false);};

	void Inverse();

	

private:
	GLKPOSITION headPos;
	GLKPOSITION tailPos;

	int nCount;
};

#endif




#ifndef _CW_GLKARRAY
#define _CW_GLKARRAY

class GLKArray  
{
public:
	GLKArray(int sx=50, int increaseStep=50, int type=0);	//	Type:	0 - (void*)
															//			1 - int
															//			2 - float 
															//			3 - double
	~GLKArray();

	int GetSize();

	void* RemoveAt(int i);
	void RemoveAll();
	
	void Add(void* data);
	void Add(int data);
	void Add(float data);
	void Add(double data);

	void SetAt(int i, void* data);
	void SetAt(int i, int data);
	void SetAt(int i, float data);

	void* GetAt(int i);
	int GetIntAt(int i);
	float GetFloatAt(int i);
	double GetDoubleAt(int i);

private:
	void** listType0;
	int* listType1;
	float* listType2;
	double* listType3;

	int size;
	int arraySize;

	int step;
	int m_type;
};

#endif