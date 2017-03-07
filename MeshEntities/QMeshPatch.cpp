// QMeshPatch.cpp: implementation of the QMeshPatch class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <math.h>
#include "QMeshPatch.h"
#include "QMeshEdge.h"
#include "QMeshNode.h"
#include "QMeshFace.h"
#include "QMeshTetra.h"
#include "QMeshHexa.h"
#include "CH_ATTRIB_EDGENODE.h"
#include "CH_ATTRIB_FACENODE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//#include "..\GDBrace\GDBWovenModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QMeshPatch::QMeshPatch()
{
	for(int i=0;i<8;i++) flags[i]=FALSE;

	isVolume=false;
	isTET=true;

	selectedNode = NULL;
}

QMeshPatch::~QMeshPatch()
{
	ClearAll();
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

BOOL QMeshPatch::GetAttribFlag( const int whichBit )
{
	return flags[whichBit];
}

void QMeshPatch::SetAttribFlag( const int whichBit, const BOOL toBe )
{
	flags[whichBit] = toBe;
}

UINT QMeshPatch::GetIndexNo()
{
	return indexno;
}
void QMeshPatch::SetIndexNo( const UINT _index )
{
	indexno = _index;
}
int QMeshPatch::GetTrglFaceNumber() 
{
	return trglFaceList.GetCount();
}

QMeshFace* QMeshPatch::GetTrglFaceRecordPtr(int No)  //from 1 to n
{
	if( (No < 1) || (No > trglFaceList.GetCount()))    return  NULL;
    return (QMeshFace *)trglFaceList.GetAt(trglFaceList.FindIndex(No-1));
}

GLKObList& QMeshPatch::GetTrglFaceList()
{
	return trglFaceList;
}


int QMeshPatch::GetTrglEdgeNumber() 
{
	return trglEdgeList.GetCount();
}

QMeshEdge* QMeshPatch::GetTrglEdgeRecordPtr(int No)  //from 1 to n
{
	if( (No < 1) || (No > trglEdgeList.GetCount()))    return  NULL;
    return (QMeshEdge *)trglEdgeList.GetAt(trglEdgeList.FindIndex(No-1));
}

GLKObList& QMeshPatch::GetTrglEdgeList()
{
	return trglEdgeList;
}

int QMeshPatch::GetTrglNodeNumber() 
{
	return trglNodeList.GetCount();
}

QMeshNode* QMeshPatch::GetTrglNodeRecordPtr(int No)  //from 1 to n
{
	if( (No < 1) || (No > trglNodeList.GetCount()))    return  NULL;
    return (QMeshNode *)trglNodeList.GetAt(trglNodeList.FindIndex(No-1));
}

GLKObList& QMeshPatch::GetTrglNodeList()
{
	return trglNodeList;
}

int QMeshPatch::GetTrglTetraNumber() 
{
	return trglTetraList.GetCount();
}

QMeshTetra* QMeshPatch::GetTrglTetraRecordPtr(int No)  //from 1 to n
{
	if( (No < 1) || (No > trglTetraList.GetCount()))    return  NULL;
    return (QMeshTetra *)trglTetraList.GetAt(trglTetraList.FindIndex(No-1));
}

GLKObList& QMeshPatch::GetTrglTetraList()
{
	return trglTetraList;
}

QMeshHexa* QMeshPatch::GetTrglHexaRecordPtr(int No)  //from 1 to n
{
	if( (No < 1) || (No > trglHexaList.GetCount()))    return  NULL;
    return (QMeshHexa *)trglHexaList.GetAt(trglHexaList.FindIndex(No-1));
}

void QMeshPatch::ClearAll()
{
	GLKPOSITION Pos;

	for(Pos=trglTetraList.GetHeadPosition();Pos!=NULL;)
	{
		QMeshTetra *temp=
			(QMeshTetra *)(trglTetraList.GetNext(Pos));
		delete temp;
	}
	trglTetraList.RemoveAll();

	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;)
	{
		QMeshFace *temp=
			(QMeshFace *)(trglFaceList.GetNext(Pos));
		delete temp;
	}
	trglFaceList.RemoveAll();

	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;)
	{
		QMeshEdge *temp=
			(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		delete temp;
	}
	trglEdgeList.RemoveAll();

	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;)
	{
		QMeshNode *temp=
			(QMeshNode *)(trglNodeList.GetNext(Pos));

		if (temp->boundNodes) 
		{
			GLKPOSITION PosNode;
			for (PosNode=temp->boundNodes->GetHeadPosition();PosNode!=NULL;)
			{
				QMeshNode *temp2= (QMeshNode *)(temp->boundNodes->GetNext(PosNode));
				delete temp2;
			}
			temp->boundNodes->RemoveAll();
	
		delete temp->boundNodes;
		}

		delete temp;
	}
	trglNodeList.RemoveAll();	
}

bool QMeshPatch::inputOFFFile(const char *filename)
{
	FILE *fp;
	char buf[100];
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;
	QMeshEdge *edge;
	QMeshFace *face;
	QMeshNode **nodeArray;
	float xx,yy,zz;
	int faceNum,nodeNum,i;

	fp = fopen(filename, "r");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	fscanf(fp,"%s\n",buf);
	fscanf(fp,"%d %d %d\n",&nodeNum,&faceNum,&i);

	for(i=0;i<nodeNum;++i) {
		fscanf(fp,"%f %f %f\n",&xx,&yy,&zz);
		node=new QMeshNode;
		node->SetMeshSurfacePtr(this);
		node->SetCoord3D(xx,yy,zz);
		node->SetIndexNo(trglNodeList.GetCount()+1);
		trglNodeList.AddTail(node);
	}

	nodeArray=(QMeshNode**)new long[nodeNum];
	i=0;
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;++i) {
		node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		nodeArray[i]=node;
	}

	for(i=0;i<faceNum;++i) {
		int num,nodeIndex, tmp;
		fscanf(fp,"%d ",&num);
//		num=3;
		if (num == 1)
			break;

		if (num>2) {
			face=new QMeshFace;
			face->SetMeshSurfacePtr(this);
			face->SetIndexNo(trglFaceList.GetCount()+1);
			trglFaceList.AddTail(face);

			for(int j=0;j<num;++j) {
				if (j==(num-1)) {
					fscanf(fp,"%d",&nodeIndex);
					fscanf(fp, "%d %d %d %d\n", &tmp, &tmp, &tmp, &tmp);
				}
				else
					fscanf(fp,"%d ",&nodeIndex);

				(face->GetAttachedList()).AddTail(nodeArray[nodeIndex]);
			}
		}
	}

	fclose(fp);

	delete [](QMeshNode**)nodeArray;

	//---------------------------------------------------------------------
	//	Build the topology
	//---------------------------------------------------------------------
	//	Step 1: build the edges
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		face=(QMeshFace*)(trglFaceList.GetNext(Pos));

		int edgeNum=(face->GetAttachedList()).GetCount();
		face->SetEdgeNum(edgeNum);

		nodeArray=(QMeshNode**)new long[edgeNum];
		i=0;
		for(PosNode=(face->GetAttachedList()).GetHeadPosition();PosNode!=NULL;++i) {
			nodeArray[i]=(QMeshNode*)((face->GetAttachedList()).GetNext(PosNode));
			(nodeArray[i]->GetTrglFaceList()).AddTail(face);
		}
		for(i=0;i<edgeNum;i++) {
			edge=NULL;	startNode=nodeArray[i];	endNode=nodeArray[(i+1)%edgeNum];
			bool bDir;
			for(PosNode=(startNode->GetTrglEdgeList()).GetHeadPosition();PosNode!=NULL;) {
				QMeshEdge *temp=(QMeshEdge *)((startNode->GetTrglEdgeList()).GetNext(PosNode));
				if ((temp->GetStartPoint()==startNode) && (temp->GetEndPoint()==endNode)) {
					edge=temp;	bDir=true;
					//face->GetIndexNo();
				}
				else if ((temp->GetStartPoint()==endNode) && (temp->GetEndPoint()==startNode)) {
					edge=temp;	bDir=false;
				}
			}
			if (edge) {
				face->SetEdgeRecordPtr(i+1, edge);
				if (bDir) {
					face->SetDirectionFlag(i+1, true);
					edge->SetLeftFace(face);
				}
				else {
					face->SetDirectionFlag(i+1, false);
					edge->SetRightFace(face);
				}
			}
			else {
				edge=new QMeshEdge;
				edge->SetMeshSurfacePtr(this);
				edge->SetStartPoint(startNode);
				edge->SetEndPoint(endNode);
				edge->SetIndexNo(trglEdgeList.GetCount()+1);
				trglEdgeList.AddTail(edge);

				edge->SetLeftFace(face);
				face->SetEdgeRecordPtr(i+1,edge);
				face->SetDirectionFlag(i+1,true);
				(startNode->GetTrglEdgeList()).AddTail(edge);
				(endNode->GetTrglEdgeList()).AddTail(edge);
			}
		}

		delete [](QMeshNode**)nodeArray;

		face->GetAttachedList().RemoveAll();
	}
	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		face->CalPlaneEquation();
		face->CalCenterPos();
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		node->CalNormal();
	}

	//---------------------------------------------------------------------
	//	Fill in the flags
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		if (edge->GetLeftFace() && edge->GetRightFace()) continue;
		edge->SetAttribFlag(0);
		edge->GetStartPoint()->SetAttribFlag(0);
		edge->GetEndPoint()->SetAttribFlag(0);
	}

	return true;
}

bool QMeshPatch::inputOBJFile(const char *filename)
{
	FILE *fp;
	char linebuf[256], buf[100];
	int nodeNum, i;	float xx, yy, zz;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;
	QMeshNode *node, *startNode, *endNode;
	QMeshEdge *edge;
	QMeshFace *face;
	QMeshNode **nodeArray;

	fp = fopen(filename, "r");
	if (!fp) { AfxMessageBox("Can not open the data file"); return false; }
	ClearAll();

	while (!feof(fp)) {
		sprintf(buf, "");
		sprintf(linebuf, "");
		fgets(linebuf, 255, fp);
		sscanf(linebuf, "%s", buf);

		if ((strlen(buf) == 1) && (buf[0] == 'v'))
		{
			sscanf(linebuf, "%s %f %f %f \n", buf, &xx, &yy, &zz);
			node = new QMeshNode;
			node->SetMeshSurfacePtr(this);
			node->SetCoord3D(xx, yy, zz);
			//			node->SetCoord3D(xx*0.1f,yy*0.1f,zz*0.1f);
			node->SetIndexNo(trglNodeList.GetCount() + 1);
			trglNodeList.AddTail(node);
		}
	}
	fclose(fp);

	nodeNum = trglNodeList.GetCount();
	nodeArray = (QMeshNode**)new long[nodeNum];
	i = 0;
	for (Pos = trglNodeList.GetHeadPosition(); Pos != NULL; i++) {
		node = (QMeshNode*)(trglNodeList.GetNext(Pos));
		nodeArray[i] = node;
	}

	fp = fopen(filename, "r");
	int nodeIndex = 0;	bool bWith2DCoord = false;
	while (!feof(fp)) {
		sprintf(buf, "");
		sprintf(linebuf, "");
		fgets(linebuf, 255, fp);
		sscanf(linebuf, "%s", buf);

		/*if ( (strlen(buf)==2) && (buf[0]=='v') && (buf[1]=='t') )
		{
		sscanf(linebuf, "%s %f %f \n", buf, &xx, &yy);
		nodeArray[nodeIndex]->SetCoord2D(xx,yy);
		bWith2DCoord=true;
		nodeIndex++;
		}*/

		if ((strlen(buf) == 1) && (buf[0] == 'f'))
		{
			char seps[] = " \n";
			char seps2[] = "/";
			char *token;
			char linebuf2[255];

			strcpy(linebuf2, linebuf);

			int num = 0;
			token = strtok(linebuf, seps);
			while (token != NULL) { token = strtok(NULL, seps); num++; }
			num = num - 1;

			//num=3;

			//////////////////////////////////////////////////////////////////////////
			//for polygonal mesh
			face = new QMeshFace;
			face->SetMeshSurfacePtr(this);
			face->SetIndexNo(trglFaceList.GetCount() + 1);
			trglFaceList.AddTail(face);

			char** fields;
			fields = (char**)new long[num];
			for (i = 0; i<num; i++) fields[i] = new char[255];

			token = strtok(linebuf2, seps);
			for (i = 0; i<num; i++) {
				token = strtok(NULL, seps);
				strcpy(fields[i], token);
			}

			for (i = 0; i<num; i++) {
				token = strtok(fields[i], seps2);
				int nodeIndex = atoi(token);
				face->attachedList.AddTail(nodeArray[nodeIndex - 1]);
			}

			for (i = 0; i<num; i++) delete[](char*)(fields[i]);
			delete[](char**)fields;


			//////////////////////////////////////////////////////////////////////////
			//triangulation
			/*
			char** fields;
			fields=(char**)new long[num];
			for(i=0;i<num;i++) fields[i]=new char[255];

			token = strtok( linebuf2, seps );
			for(i=0;i<num;i++)
			{
			token = strtok( NULL, seps );
			strcpy(fields[i],token);
			}

			for(int ControlNum =0;ControlNum<num-2;ControlNum++)

			{
			face=new QMeshFace;
			face->SetMeshSurfacePtr(this);
			face->SetIndexNo(trglFaceList.GetCount()+1);
			trglFaceList.AddTail(face);


			for(i=0;i<3;i++)
			{
			if(i==0)
			{
			token = strtok( fields[i], seps2 );
			int nodeIndex=atoi(token);
			face->GetAttachedList().AddTail(nodeArray[nodeIndex-1]);
			}
			else
			{
			token = strtok( fields[i+ControlNum], seps2 );
			int nodeIndex=atoi(token);
			face->GetAttachedList().AddTail(nodeArray[nodeIndex-1]);
			}

			}
			}
			for(i=0;i<num;i++) delete [](char*)(fields[i]);
			delete [](char**)fields;*/
		}
	}
	fclose(fp);

	delete[](QMeshNode**)nodeArray;

	if (bWith2DCoord) { SetAttribFlag(0);	SetAttribFlag(1); }

	//---------------------------------------------------------------------
	//	Build the topology
	//---------------------------------------------------------------------
	//	Step 1: build the edges
	for (Pos = trglFaceList.GetHeadPosition(); Pos != NULL;) {
		QMeshFace*face = (QMeshFace*)(trglFaceList.GetNext(Pos));

		int edgeNum = (face->attachedList).GetCount();
		face->SetEdgeNum(edgeNum);
		nodeArray = (QMeshNode**)new long[edgeNum];
		i = 0;
		for (PosNode = (face->attachedList).GetHeadPosition(); PosNode != NULL; i++) {
			nodeArray[i] = (QMeshNode*)((face->attachedList).GetNext(PosNode));
			(nodeArray[i]->GetTrglFaceList()).AddTail(face);
		}
		for (i = 0; i<edgeNum; i++) {
			edge = NULL;	startNode = nodeArray[i];	endNode = nodeArray[(i + 1) % edgeNum];
			bool bDir;
			for (PosNode = (startNode->GetTrglEdgeList()).GetHeadPosition(); PosNode != NULL;) {
				QMeshEdge *temp = (QMeshEdge *)((startNode->GetTrglEdgeList()).GetNext(PosNode));
				if ((temp->GetStartPoint() == startNode) && (temp->GetEndPoint() == endNode)) {
					edge = temp;	bDir = true;
				}
				else if ((temp->GetStartPoint() == endNode) && (temp->GetEndPoint() == startNode)) {
					edge = temp;	bDir = false;
				}
			}
			if (edge) {
				face->SetEdgeRecordPtr(i + 1, edge);
				if (bDir) {
					face->SetDirectionFlag(i + 1, true);
					edge->SetLeftFace(face);
				}
				else {
					face->SetDirectionFlag(i + 1, false);
					edge->SetRightFace(face);
				}
			}
			else {
				edge = new QMeshEdge;
				edge->SetMeshSurfacePtr(this);
				edge->SetStartPoint(startNode);
				edge->SetEndPoint(endNode);
				edge->SetIndexNo(trglEdgeList.GetCount() + 1);
				trglEdgeList.AddTail(edge);

				edge->SetLeftFace(face);
				face->SetEdgeRecordPtr(i + 1, edge);
				face->SetDirectionFlag(i + 1, TRUE);
				(startNode->GetTrglEdgeList()).AddTail(edge);
				(endNode->GetTrglEdgeList()).AddTail(edge);
			}
		}

		delete[](QMeshNode**)nodeArray;

		face->attachedList.RemoveAll();
	}
	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for (Pos = trglFaceList.GetHeadPosition(); Pos != NULL;) {
		face = (QMeshFace*)(trglFaceList.GetNext(Pos));
		face->CalPlaneEquation();
		//face->CalCenterPos();
	}
	for (Pos = trglNodeList.GetHeadPosition(); Pos != NULL;) {
		node = (QMeshNode*)(trglNodeList.GetNext(Pos));
		node->CalNormal();
	}
	//---------------------------------------------------------------------
	//	Fill in the flags
	for (Pos = trglEdgeList.GetHeadPosition(); Pos != NULL;) {
		QMeshEdge *edge = (QMeshEdge *)(trglEdgeList.GetNext(Pos));
		if (edge->GetLeftFace() && edge->GetRightFace()) continue;
		edge->SetAttribFlag(0);
		edge->GetStartPoint()->SetAttribFlag(0);
		edge->GetEndPoint()->SetAttribFlag(0);
	}

	return true;
}

bool QMeshPatch::inputASCFile(const char *filename)
{
	FILE *fp;
	char linebuf[256];
	int nodeNum,i;	float xx,yy,zz;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;

	fp = fopen(filename, "r");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	while(!feof(fp)) {
		sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		
			sscanf(linebuf, "%f %f %f \n", &xx, &yy, &zz);
			node=new QMeshNode;
			node->SetMeshSurfacePtr(this);
			node->SetCoord3D(xx,yy,zz);
			node->SetIndexNo(trglNodeList.GetCount()+1);
			trglNodeList.AddTail(node);
	}
	fclose(fp);

	return true;
}

bool QMeshPatch::inputBSFFile(const char *filename)
{
	FILE *fp;
	unsigned char buffer[18];
	int data_type;
	float x,y,z;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;

	fp = fopen(filename, "r+b");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	fseek(fp,3,SEEK_SET);
	fread(buffer,sizeof(char),1,fp);
	data_type = buffer[0];
	fseek(fp,256,SEEK_SET);

	while(!feof(fp)) {
		if (data_type == 0) fread(buffer,sizeof(char),12,fp);
		if (data_type == 2) fread(buffer,sizeof(char),18,fp);
		int ix = (signed short) ((buffer[0] << 8) + buffer[1]);
		int iy = (signed short) ((buffer[2] << 8) + buffer[3]);
		int iz = (signed short) ((buffer[4] << 8) + buffer[5]);
		x = (float)ix * 0.0001f;
		y = (float)iy * 0.0001f;
		z = (float)iz * 0.0001f;
	
		node=new QMeshNode;
		node->SetMeshSurfacePtr(this);
		node->SetCoord3D(x,y,z);
		node->SetIndexNo(trglNodeList.GetCount()+1);
		trglNodeList.AddTail(node);
	}
	fclose(fp);

	return true;
}

bool QMeshPatch::inputPWNFile(const char *filename)
{
	FILE *fp;
	int nodeNum;	
	float xx,yy,zz;
	QMeshNode *node;
	
	fp = fopen(filename, "r");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	fscanf(fp,"%d\n",&nodeNum);

	QMeshNode **nodeArray = (QMeshNode**)new long[nodeNum];

	for(int i=0;i<nodeNum;++i) {
		fscanf(fp,"%f %f %f\n",&xx,&yy,&zz);
	
		node=new QMeshNode;
		node->SetMeshSurfacePtr(this);
		node->SetCoord3D(xx,yy,zz);
		node->SetIndexNo(trglNodeList.GetCount()+1);
		trglNodeList.AddTail(node);
		nodeArray[i] = node;
	}

	for(int i=0;i<nodeNum;++i) {
		fscanf(fp,"%f %f %f\n",&xx,&yy,&zz);
		nodeArray[i]->SetNormal(xx, yy, zz);
	}

	fclose(fp);
	delete [](QMeshNode**)nodeArray;

	return true;
}

bool QMeshPatch::inputBinaryFile(const char *filename)
{
	FILE *fp;
	char linebuf[256];
	int nodeNum,i;	float xx,yy,zz;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;

	fp = fopen(filename, "r+b");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	while(!feof(fp)) {
			fread(&xx, sizeof(float), 1, fp);
			fread(&yy, sizeof(float), 1, fp);
			fread(&zz, sizeof(float), 1, fp);
			node=new QMeshNode;
			node->SetMeshSurfacePtr(this);
			node->SetCoord3D(xx,yy,zz);
			node->SetIndexNo(trglNodeList.GetCount()+1);
			trglNodeList.AddTail(node);
	}
	fclose(fp);

	return true;
}


void QMeshPatch::AdjustOrientation(float rx, float ry)
{
	double csy,sny,csx,snx;
	double ratio=3.1451592654/180.0;
	double xx,yy,zz,x1,y1,z1,x2,y2,z2;
	int i;

	csy=cos(ry*ratio);		sny=sin(ry*ratio);
	csx=cos(rx*ratio);		snx=sin(rx*ratio);

	for (GLKPOSITION Pos = GetTrglNodeList().GetHeadPosition(); Pos!=NULL;)
	{
		QMeshNode *node = (QMeshNode*)GetTrglNodeList().GetNext(Pos);
		node->GetCoord3D(xx, yy, zz);

		x1=zz*sny+xx*csy;
		y1=yy;
		z1=zz*csy-xx*sny;

		x2=x1;
		y2=y1*csx-z1*snx;
		z2=y1*snx+z1*csx;

		node->SetCoord3D(x2, y2, z2);
	}

	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(GLKPOSITION Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace *face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		face->CalPlaneEquation();
		face->CalCenterPos();
	}
	for(GLKPOSITION Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		node->CalNormal();
	}
	

}

void QMeshPatch::ScaleToUnitHeight(){
	double max=-1e+8, min=1e+8;
	for (GLKPOSITION Pos = this->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
		QMeshNode *node = (QMeshNode*)this->GetTrglNodeList().GetNext(Pos);
		double x, y, z;
		node->GetCoord3D(x, y, z);
		if (y>max) max=y;
		if (y<min) min=y;
	}
	double height = max-min;

	for (GLKPOSITION Pos = this->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
		QMeshNode *node = (QMeshNode*)this->GetTrglNodeList().GetNext(Pos);
		double x, y, z;
		node->GetCoord3D(x, y, z);
		//y-=min;
		x/=height; y/=height; z/=height;
		node->SetCoord3D(x, y, z);
	}

}

bool QMeshPatch::inputMESHFile(const char *filename){
	FILE *fp = fopen(filename, "r");
	if(!fp) {AfxMessageBox("Can not open the .mesh file"); return false;}
	ClearAll();

	printf("reading MESH file.........");
	isVolume=true;

	char linebuf[256],buf[100];
	int ref;
	//sprintf(buf,""); sprintf(linebuf,"");
	//fgets(linebuf, 255, fp);
	//sscanf(linebuf, "%s %d\n", buf, &ref);
	//if (strcmp(buf, "MeshVersionFormatted")!=0){
	//	printf("First row is not the keyword: MeshVersionFormatted\n");
	//	return false;
	//}

	//sprintf(buf,""); sprintf(linebuf,"");
	//fgets(linebuf, 255, fp);
	//sscanf(linebuf, "%s %d\n", buf, &ref);
	//if (strcmp(buf, "Dimension")!=0){
	//	printf("Second row is not the keyword: Dimension\n");
	//	return false;
	//}

	QMeshNode **nodeArray=NULL;

	while (!feof(fp)){
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		sscanf(linebuf, "%s\n", buf);
		if (buf[0]=='#') continue;
		if (strcmp(buf, "End")==0)
			break;
		if (strcmp(buf, "Vertices")==0){
			sprintf(buf,""); sprintf(linebuf,"");
			fgets(linebuf, 255, fp);
			int nodeNum;
			sscanf(linebuf, "%d\n", &nodeNum);
			printf("%d vertices, ", nodeNum);
			nodeArray=(QMeshNode**)new long[nodeNum];
			for (int i=0; i<nodeNum; i++){
				sprintf(buf,""); sprintf(linebuf,"");
				fgets(linebuf, 255, fp);
				float xx, yy, zz;
				sscanf(linebuf,"%f %f %f %d\n", &xx, &yy, &zz, &ref);
				QMeshNode *node=new QMeshNode;
				node->SetMeshSurfacePtr(this);
				node->SetCoord3D(xx,yy,zz);
				node->SetIndexNo(i+1);
				trglNodeList.AddTail(node);
				nodeArray[i]=node;
			}
			continue;
		}
		if (strcmp(buf, "Tetrahedra")==0){
			sprintf(buf,""); sprintf(linebuf,"");
			fgets(linebuf, 255, fp);
			int tetraNum;
			sscanf(linebuf,"%d \n",&tetraNum);
			printf("%d tetras, ", tetraNum);
			for (int i=0; i<tetraNum; i++){
				int n[4];
				sprintf(buf,""); sprintf(linebuf,"");
				fgets(linebuf, 255, fp);
				sscanf(linebuf,"%d %d %d %d %d \n", &n[0], &n[1], &n[2], &n[3], &ref);

				for (int j=0; j<4; j++)
					n[j]--;

				int n_index[4][3] = {
					{n[0], n[1], n[2]},
					{n[1], n[3], n[2]},
					{n[2], n[3], n[0]},
					{n[3], n[1], n[0]}
				};

				QMeshTetra *Tetra = new QMeshTetra;
				Tetra->SetIndexNo(i+1);
				Tetra->SetMeshSurfacePtr(this);
				trglTetraList.AddTail(Tetra);

				for (int j=0; j<4; j++){
					nodeArray[n[j]]->AddTetra(Tetra);
				}

				//for (int j=0; j<4; j++)
				//	Tetra->nodeindex[j] = n[j];

				QMeshFace *f[4];
				for (int j=1; j<=4; j++){
					bool existed=false;
					for (GLKPOSITION Pos = nodeArray[n[j-1]]->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
						QMeshFace *tmp_face = (QMeshFace*)nodeArray[n[j-1]]->GetTrglFaceList().GetNext(Pos);
						QMeshNode *tmp_node[3];
						int k=0;
						for (GLKPOSITION Pos1 = tmp_face->GetAttachedList().GetHeadPosition(); Pos1!=NULL; k++)
							tmp_node[k] = (QMeshNode*)tmp_face->GetAttachedList().GetNext(Pos1);
						bool same[3]={0, 0, 0};
						for (k=0; k<3; k++){
							for (int m=0; m<3; m++){
								if (tmp_node[k]->GetIndexNo()-1 == n_index[j-1][m]){
									same[k]=true;
									break;
								}
							}
							if (!same[k]) break;
						}
						if (same[0] && same[1] && same[2]){
							f[j-1] = tmp_face;
							if (f[j-1]->GetRightTetra()!=NULL) printf("ERROR: f[%d-1]->GetRightTetra()!=NULL\n", j);
							f[j-1]->SetRightTetra(Tetra);
							Tetra->SetDirectionFlag(j, false);
							existed=true; break;
						}
					}
					if (!existed){
						f[j-1] = new QMeshFace();
						f[j-1]->SetMeshSurfacePtr(this);
						f[j-1]->SetIndexNo(trglFaceList.GetCount()+1);
						trglFaceList.AddTail(f[j-1]);
						for (int k=0; k<3; k++){
							f[j-1]->GetAttachedList().AddTail(nodeArray[n_index[j-1][k]]);
							nodeArray[n_index[j-1][k]]->GetTrglFaceList().AddTail(f[j-1]);
						}
						if (f[j-1]->GetLeftTetra()!=NULL) printf("ERROR: f[%d-1]->GetLeftTetra()!=NULL\n", j);
						f[j-1]->SetLeftTetra(Tetra);
						Tetra->SetDirectionFlag(j);
					}
					Tetra->SetFaceRecordPtr(j, f[j-1]);
				}	
			}
			continue;
		}
	}
	fclose(fp);

	delete [](QMeshNode**)nodeArray;

	SetAttribFlag(1);

	//---------------------------------------------------------------------
	//	Build the topology
	//---------------------------------------------------------------------
	//	Step 1: build the edges
	for(GLKPOSITION Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace*face=(QMeshFace*)(trglFaceList.GetNext(Pos));

		if (face->GetLeftTetra() && face->GetRightTetra()) {face->inner=true;} else face->inner=false;


		int edgeNum=(face->attachedList).GetCount();
		face->SetEdgeNum(edgeNum);
		nodeArray=(QMeshNode**)new long[edgeNum];
		int i=0;
		for(GLKPOSITION PosNode=(face->attachedList).GetHeadPosition();PosNode!=NULL;i++) {
			nodeArray[i]=(QMeshNode*)((face->attachedList).GetNext(PosNode));
			//(nodeArray[i]->GetTrglFaceList()).AddTail(face);
		}
		for(i=0;i<edgeNum;i++) {
			QMeshEdge *edge=NULL;	QMeshNode *startNode=nodeArray[i];	QMeshNode *endNode=nodeArray[(i+1)%edgeNum];
			bool bDir;
			for(GLKPOSITION PosNode=(startNode->GetTrglEdgeList()).GetHeadPosition();PosNode!=NULL;) {
				QMeshEdge *temp=(QMeshEdge *)((startNode->GetTrglEdgeList()).GetNext(PosNode));
				if ((temp->GetStartPoint()==startNode) && (temp->GetEndPoint()==endNode)) {
					edge=temp;	bDir=true;
				}
				else if ((temp->GetStartPoint()==endNode) && (temp->GetEndPoint()==startNode)) {
					edge=temp;	bDir=false;
				}
			}
			if (edge) {
				face->SetEdgeRecordPtr(i+1,edge);
				edge->GetTrglFaceList().AddTail(face);
				if (bDir) {
					face->SetDirectionFlag(i+1,true);
					if (!face->inner) {
						if (edge->GetLeftFace()) {
							printf("edge->GetLeftFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetRightFace()) printf("but right face is null!\n");
							//bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
							//if (t1) deleteTetra(face->GetLeftTetra());
							//if (t2) deleteTetra(face->GetRightTetra());
							continue;
						}
						else edge->SetLeftFace(face);
					}
				}
				else {
					face->SetDirectionFlag(i+1,false);
					if (!face->inner) {
						if (edge->GetRightFace()) {
							printf("edge->GetRightFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetLeftFace()) printf("but left face is null!\n");
							//bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
							//if (t1) deleteTetra(face->GetLeftTetra());
							//if (t2) deleteTetra(face->GetRightTetra());
							continue;
						}
						else edge->SetRightFace(face);
					}
				}
			}
			else {
				edge=new QMeshEdge;
				edge->SetMeshSurfacePtr(this);
				edge->SetStartPoint(startNode);
				edge->SetEndPoint(endNode);
				edge->SetIndexNo(trglEdgeList.GetCount()+1);
				trglEdgeList.AddTail(edge);

				edge->GetTrglFaceList().AddTail(face);
				if (!face->inner) edge->SetLeftFace(face);
				face->SetEdgeRecordPtr(i+1,edge);
				face->SetDirectionFlag(i+1,TRUE);
				(startNode->GetTrglEdgeList()).AddTail(edge);
				(endNode->GetTrglEdgeList()).AddTail(edge);
			}
		}

		delete [](QMeshNode**)nodeArray;

		face->attachedList.RemoveAll();
	}

	//for (Pos=trglTetraList.GetHeadPosition(); Pos!=NULL;){
	//	QMeshTetra *tetra = (QMeshTetra*) trglTetraList.GetNext(Pos);
	//	for (int j=1; j<=4; j++){
	//		bool exist = false;
	//		for (int i=0; i<4; i++){
	//			if (tetra->nodeindex[i]==tetra->GetNodeRecordPtr(j)->GetIndexNo()-1)
	//				exist=true;
	//		}
	//		if (!exist) printf("Not exist: %d tetra, %d node\n", tetra->GetIndexNo(), tetra->GetNodeRecordPtr(j)->GetIndexNo());
	//	}
	//}

	//return true;

	//---------------------------------------------------------------------
	//	Fill in the flags
	for(GLKPOSITION Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {edge->inner=false; break;}
		}
	}
	for(GLKPOSITION Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode *)(trglNodeList.GetNext(Pos));
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {node->inner=false; break;}
		}
	}
	for(GLKPOSITION Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		if (edge->inner) continue;
		if (edge->GetLeftFace() && edge->GetRightFace()) continue;
		edge->SetAttribFlag(0);
		edge->GetStartPoint()->SetAttribFlag(0);
		edge->GetEndPoint()->SetAttribFlag(0);
	}


	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(GLKPOSITION Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace *face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		//if (face->inner) continue;
		face->CalPlaneEquation();
		face->CalCenterPos();
		face->i_inner=face->inner;
	}
	for(GLKPOSITION Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		if (node->inner) continue;
		node->CalNormal();
	}


	return true;
}

bool QMeshPatch::inputELEFile(const char *filename){
	FILE *fp;
	char linebuf[256],buf[100];
	int nodeNum=-1,i;	float xx,yy,zz;
	int tetraNum=-1;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;
	QMeshEdge *edge;
	QMeshFace *face;
	QMeshNode **nodeArray;

	CString ELEFile = filename;
	CString NodeFile = filename;
	NodeFile.Delete(NodeFile.GetLength()-3, 3);
	NodeFile.Append("node");

/////////////////////////////////////////////////////////////////////////////////////////
	fp = fopen(NodeFile, "r");
    if(!fp) {AfxMessageBox("Can not open the .node file"); return false;}
	ClearAll();

	printf("reading node file.........");
	isVolume=true;

	int Ncount=0;
	while (!feof(fp)){
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		char c;
		sscanf(linebuf,"%c",&c);
		if (c=='#') 
			continue;
		
		if (nodeNum==-1){
			sscanf(linebuf,"%d \n",&nodeNum);
			printf("%d vertices, ", nodeNum);
			nodeArray=(QMeshNode**)new long[nodeNum];
			continue;
		}

		if (Ncount<nodeNum){
			sscanf(linebuf,"%d %f %f %f \n",&i, &xx, &yy, &zz);
			node=new QMeshNode;
			node->SetMeshSurfacePtr(this);
			node->SetCoord3D(xx,yy,zz);
			node->SetIndexNo(Ncount+1);
			trglNodeList.AddTail(node);
			nodeArray[Ncount]=node;
			Ncount++;
		}
	}
	fclose(fp);
/////////////////////////////////////////////////////////////////////////////////////////
	fp = fopen(ELEFile, "r");
	printf("reading ELE file.........");

	int Ecount=0;
	while (!feof(fp)){
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		char c;
		sscanf(linebuf,"%c",&c);
		if (c=='#') 
			continue;
		
		if (tetraNum==-1){
			sscanf(linebuf,"%d \n",&tetraNum);
			printf("%d tetras, ", tetraNum);
			continue;
		}

		if (Ecount<tetraNum){
			int n[4];
			sscanf(linebuf,"%d %d %d %d %d \n",&i, &n[0], &n[1], &n[2], &n[3]);

			//for (int j=0; j<4; j++)
			//	n[j]--;

			i = Ecount;

			int n_index[4][3] = {
				{n[0], n[1], n[2]},
				{n[1], n[3], n[2]},
				{n[2], n[3], n[0]},
				{n[3], n[1], n[0]}
			};

			QMeshTetra *Tetra = new QMeshTetra;
			Tetra->SetIndexNo(i+1);
			Tetra->SetMeshSurfacePtr(this);
			trglTetraList.AddTail(Tetra);

			for (int j=0; j<4; j++){
				nodeArray[n[j]]->AddTetra(Tetra);
			}

			//for (int j=0; j<4; j++)
			//	Tetra->nodeindex[j] = n[j];

			QMeshFace *f[4];
			for (int j=1; j<=4; j++){
				bool existed=false;
				for (GLKPOSITION Pos = nodeArray[n[j-1]]->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
					QMeshFace *tmp_face = (QMeshFace*)nodeArray[n[j-1]]->GetTrglFaceList().GetNext(Pos);
					QMeshNode *tmp_node[3];
					int k=0;
					for (GLKPOSITION Pos1 = tmp_face->GetAttachedList().GetHeadPosition(); Pos1!=NULL; k++)
						tmp_node[k] = (QMeshNode*)tmp_face->GetAttachedList().GetNext(Pos1);
					bool same[3]={0, 0, 0};
					for (k=0; k<3; k++){
						for (int m=0; m<3; m++){
							if (tmp_node[k]->GetIndexNo()-1 == n_index[j-1][m]){
								same[k]=true;
								break;
							}
						}
						if (!same[k]) break;
					}
					if (same[0] && same[1] && same[2]){
						f[j-1] = tmp_face;
						if (f[j-1]->GetRightTetra()!=NULL) printf("ERROR: f[%d-1]->GetRightTetra()!=NULL\n", j);
						f[j-1]->SetRightTetra(Tetra);
						Tetra->SetDirectionFlag(j, false);
						existed=true; break;
					}
				}
				if (!existed){
					f[j-1] = new QMeshFace();
					f[j-1]->SetMeshSurfacePtr(this);
					f[j-1]->SetIndexNo(trglFaceList.GetCount()+1);
					trglFaceList.AddTail(f[j-1]);
					for (int k=0; k<3; k++){
						f[j-1]->GetAttachedList().AddTail(nodeArray[n_index[j-1][k]]);
						nodeArray[n_index[j-1][k]]->GetTrglFaceList().AddTail(f[j-1]);
					}
					if (f[j-1]->GetLeftTetra()!=NULL) printf("ERROR: f[%d-1]->GetLeftTetra()!=NULL\n", j);
					f[j-1]->SetLeftTetra(Tetra);
					Tetra->SetDirectionFlag(j);
				}
				Tetra->SetFaceRecordPtr(j, f[j-1]);
			}	
			Ecount++;
		}
	}
	fclose(fp);

	delete [](QMeshNode**)nodeArray;

	SetAttribFlag(1);


	//---------------------------------------------------------------------
	//	Build the topology
	//---------------------------------------------------------------------
	//	Step 1: build the edges
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace*face=(QMeshFace*)(trglFaceList.GetNext(Pos));

		if (face->GetLeftTetra() && face->GetRightTetra()) {face->inner=true;} else face->inner=false;


		int edgeNum=(face->attachedList).GetCount();
		face->SetEdgeNum(edgeNum);
		nodeArray=(QMeshNode**)new long[edgeNum];
		i=0;
		for(PosNode=(face->attachedList).GetHeadPosition();PosNode!=NULL;i++) {
			nodeArray[i]=(QMeshNode*)((face->attachedList).GetNext(PosNode));
			//(nodeArray[i]->GetTrglFaceList()).AddTail(face);
		}
		for(i=0;i<edgeNum;i++) {
			edge=NULL;	startNode=nodeArray[i];	endNode=nodeArray[(i+1)%edgeNum];
			bool bDir;
			for(PosNode=(startNode->GetTrglEdgeList()).GetHeadPosition();PosNode!=NULL;) {
				QMeshEdge *temp=(QMeshEdge *)((startNode->GetTrglEdgeList()).GetNext(PosNode));
				if ((temp->GetStartPoint()==startNode) && (temp->GetEndPoint()==endNode)) {
					edge=temp;	bDir=true;
				}
				else if ((temp->GetStartPoint()==endNode) && (temp->GetEndPoint()==startNode)) {
					edge=temp;	bDir=false;
				}
			}
			if (edge) {
				face->SetEdgeRecordPtr(i+1,edge);
				edge->GetTrglFaceList().AddTail(face);
				if (bDir) {
					face->SetDirectionFlag(i+1,true);
					if (!face->inner) {
						if (edge->GetLeftFace()) {
							printf("edge->GetLeftFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetRightFace()) printf("but right face is null!\n");
							//bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
							//if (t1) deleteTetra(face->GetLeftTetra());
							//if (t2) deleteTetra(face->GetRightTetra());
							continue;
						}
						else edge->SetLeftFace(face);
					}
				}
				else {
					face->SetDirectionFlag(i+1,false);
					if (!face->inner) {
						if (edge->GetRightFace()) {
							printf("edge->GetRightFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetLeftFace()) printf("but left face is null!\n");
							//bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
							//if (t1) deleteTetra(face->GetLeftTetra());
							//if (t2) deleteTetra(face->GetRightTetra());
							continue;
						}
						else edge->SetRightFace(face);
					}
				}
			}
			else {
				edge=new QMeshEdge;
				edge->SetMeshSurfacePtr(this);
				edge->SetStartPoint(startNode);
				edge->SetEndPoint(endNode);
				edge->SetIndexNo(trglEdgeList.GetCount()+1);
				trglEdgeList.AddTail(edge);

				edge->GetTrglFaceList().AddTail(face);
				if (!face->inner) edge->SetLeftFace(face);
				face->SetEdgeRecordPtr(i+1,edge);
				face->SetDirectionFlag(i+1,TRUE);
				(startNode->GetTrglEdgeList()).AddTail(edge);
				(endNode->GetTrglEdgeList()).AddTail(edge);
			}
		}

		delete [](QMeshNode**)nodeArray;

		face->attachedList.RemoveAll();
	}

	//for (Pos=trglTetraList.GetHeadPosition(); Pos!=NULL;){
	//	QMeshTetra *tetra = (QMeshTetra*) trglTetraList.GetNext(Pos);
	//	for (int j=1; j<=4; j++){
	//		bool exist = false;
	//		for (int i=0; i<4; i++){
	//			if (tetra->nodeindex[i]==tetra->GetNodeRecordPtr(j)->GetIndexNo()-1)
	//				exist=true;
	//		}
	//		if (!exist) printf("Not exist: %d tetra, %d node\n", tetra->GetIndexNo(), tetra->GetNodeRecordPtr(j)->GetIndexNo());
	//	}
	//}

	//return true;

	//---------------------------------------------------------------------
	//	Fill in the flags
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {edge->inner=false; break;}
		}
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode *)(trglNodeList.GetNext(Pos));
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {node->inner=false; break;}
		}
	}
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		if (edge->inner) continue;
		if (edge->GetLeftFace() && edge->GetRightFace()) continue;
		edge->SetAttribFlag(0);
		edge->GetStartPoint()->SetAttribFlag(0);
		edge->GetEndPoint()->SetAttribFlag(0);
	}


	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		//if (face->inner) continue;
		face->CalPlaneEquation();
		face->CalCenterPos();
		face->i_inner=face->inner;
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		if (node->inner) continue;
		node->CalNormal();
	}


	return true;
}

bool QMeshPatch::inputTETFile(const char *filename)
{
	FILE *fp;
	char linebuf[256],buf[100];
	int nodeNum,i;	float xx,yy,zz;
	int tetraNum;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;
	QMeshEdge *edge;
	QMeshFace *face;
	QMeshNode **nodeArray;

	fp = fopen(filename, "r");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	printf("reading tet file.........");
	isVolume=true;

	sprintf(buf,""); sprintf(linebuf,"");
	fgets(linebuf, 255, fp);
	sscanf(linebuf,"%d %s \n",&nodeNum, buf);
	if (strcmp(buf, "vertices")!=0){
		AfxMessageBox("Incorrect Tet format! Missing vertices number.");
		fclose(fp);
		return false;
	}
	printf("%d vertices, ", nodeNum);
	sprintf(buf,""); sprintf(linebuf,"");
	fgets(linebuf, 255, fp);
	sscanf(linebuf,"%d %s \n",&tetraNum, buf);
	if (strcmp(buf, "tets")!=0){
		AfxMessageBox("Incorrect Tet format! Missing Tetra number.");
		fclose(fp);
		return false;
	}
	printf("%d tets\n", tetraNum);

	nodeArray=(QMeshNode**)new long[nodeNum];

	for (i=0; i<nodeNum; i++){
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		sscanf(linebuf,"%f %f %f \n", &xx, &yy, &zz);
		node=new QMeshNode;
		node->SetMeshSurfacePtr(this);
		node->SetCoord3D(xx,yy,zz);
		node->SetIndexNo(i+1);
		trglNodeList.AddTail(node);
		nodeArray[i]=node;
	}

	for (i=0; i<tetraNum; i++){
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		int tet, n[4];
		sscanf(linebuf,"%d %d %d %d %d \n", &tet, &n[0], &n[1], &n[2], &n[3]);
		if (tet!=4){AfxMessageBox("not a tetra?"); fclose(fp); return false;}
		//recall the notation for face 1,2,3,4 - encoding for node index on each face
		int n_index[4][3] = {
			{n[0], n[1], n[2]},
			{n[1], n[3], n[2]},
			{n[2], n[3], n[0]},
			{n[3], n[1], n[0]}
		};

		QMeshTetra *Tetra = new QMeshTetra;
		Tetra->SetIndexNo(i+1);
		Tetra->SetMeshSurfacePtr(this);
		trglTetraList.AddTail(Tetra);

		for (int j=0; j<4; j++){
			nodeArray[n[j]]->AddTetra(Tetra);
		}

		//for (int j=0; j<4; j++)
		//	Tetra->nodeindex[j] = n[j];

		QMeshFace *f[4];
		for (int j=1; j<=4; j++){
			bool existed=false;
			for (GLKPOSITION Pos = nodeArray[n[j-1]]->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
				QMeshFace *tmp_face = (QMeshFace*)nodeArray[n[j-1]]->GetTrglFaceList().GetNext(Pos);
				QMeshNode *tmp_node[3];
				int k=0;
				for (GLKPOSITION Pos1 = tmp_face->GetAttachedList().GetHeadPosition(); Pos1!=NULL; k++)
					tmp_node[k] = (QMeshNode*)tmp_face->GetAttachedList().GetNext(Pos1);
				bool same[3]={0, 0, 0};
				for (k=0; k<3; k++){
					for (int m=0; m<3; m++){
						if (tmp_node[k]->GetIndexNo()-1 == n_index[j-1][m]){
							same[k]=true;
							break;
						}
					}
					if (!same[k]) break;
				}
				if (same[0] && same[1] && same[2]){
					f[j-1] = tmp_face;
					if (f[j-1]->GetRightTetra()!=NULL) printf("ERROR: f[%d-1]->GetRightTetra()!=NULL\n", j);
					f[j-1]->SetRightTetra(Tetra);
					Tetra->SetDirectionFlag(j, false);
					existed=true; break;
				}
			}
			if (!existed){
				f[j-1] = new QMeshFace();
				f[j-1]->SetMeshSurfacePtr(this);
				f[j-1]->SetIndexNo(trglFaceList.GetCount()+1);
				trglFaceList.AddTail(f[j-1]);
				for (int k=0; k<3; k++){
					f[j-1]->GetAttachedList().AddTail(nodeArray[n_index[j-1][k]]);
					nodeArray[n_index[j-1][k]]->GetTrglFaceList().AddTail(f[j-1]);
				}
				if (f[j-1]->GetLeftTetra()!=NULL) printf("ERROR: f[%d-1]->GetLeftTetra()!=NULL\n", j);
				f[j-1]->SetLeftTetra(Tetra);
				Tetra->SetDirectionFlag(j);
			}
			Tetra->SetFaceRecordPtr(j, f[j-1]);
		}
	}


	delete [](QMeshNode**)nodeArray;

	//printf("%d tets\n", trglTetraList.GetCount());

	SetAttribFlag(1);


	//---------------------------------------------------------------------
	//	Build the topology
	//---------------------------------------------------------------------
	//	Step 1: build the edges
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace*face=(QMeshFace*)(trglFaceList.GetNext(Pos));

		if (face->GetLeftTetra() && face->GetRightTetra()) {face->inner=true;} else face->inner=false;


		int edgeNum=(face->attachedList).GetCount();
		face->SetEdgeNum(edgeNum);
		nodeArray=(QMeshNode**)new long[edgeNum];
		i=0;
		for(PosNode=(face->attachedList).GetHeadPosition();PosNode!=NULL;i++) {
			nodeArray[i]=(QMeshNode*)((face->attachedList).GetNext(PosNode));
			//(nodeArray[i]->GetTrglFaceList()).AddTail(face);
		}
		for(i=0;i<edgeNum;i++) {
			edge=NULL;	startNode=nodeArray[i];	endNode=nodeArray[(i+1)%edgeNum];
			bool bDir;
			for(PosNode=(startNode->GetTrglEdgeList()).GetHeadPosition();PosNode!=NULL;) {
				QMeshEdge *temp=(QMeshEdge *)((startNode->GetTrglEdgeList()).GetNext(PosNode));
				if ((temp->GetStartPoint()==startNode) && (temp->GetEndPoint()==endNode)) {
					edge=temp;	bDir=true;
				}
				else if ((temp->GetStartPoint()==endNode) && (temp->GetEndPoint()==startNode)) {
					edge=temp;	bDir=false;
				}
			}
			if (edge) {
				face->SetEdgeRecordPtr(i+1,edge);
				edge->GetTrglFaceList().AddTail(face);
				if (bDir) {
					face->SetDirectionFlag(i+1,true);
					if (!face->inner) {
						if (edge->GetLeftFace()) {
							printf("edge->GetLeftFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetRightFace()) printf("but right face is null!\n");
							//bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
							//if (t1) deleteTetra(face->GetLeftTetra());
							//if (t2) deleteTetra(face->GetRightTetra());
							continue;
						}
						else edge->SetLeftFace(face);
					}
				}
				else {
					face->SetDirectionFlag(i+1,false);
					if (!face->inner) {
						if (edge->GetRightFace()) {
							printf("edge->GetRightFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetLeftFace()) printf("but left face is null!\n");
							//bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
							//if (t1) deleteTetra(face->GetLeftTetra());
							//if (t2) deleteTetra(face->GetRightTetra());
							continue;
						}
						else edge->SetRightFace(face);
					}
				}
			}
			else {
				edge=new QMeshEdge;
				edge->SetMeshSurfacePtr(this);
				edge->SetStartPoint(startNode);
				edge->SetEndPoint(endNode);
				edge->SetIndexNo(trglEdgeList.GetCount()+1);
				trglEdgeList.AddTail(edge);

				edge->GetTrglFaceList().AddTail(face);
				if (!face->inner) edge->SetLeftFace(face);
				face->SetEdgeRecordPtr(i+1,edge);
				face->SetDirectionFlag(i+1,TRUE);
				(startNode->GetTrglEdgeList()).AddTail(edge);
				(endNode->GetTrglEdgeList()).AddTail(edge);
			}
		}

		delete [](QMeshNode**)nodeArray;

		face->attachedList.RemoveAll();
	}

	//for (Pos=trglTetraList.GetHeadPosition(); Pos!=NULL;){
	//	QMeshTetra *tetra = (QMeshTetra*) trglTetraList.GetNext(Pos);
	//	for (int j=1; j<=4; j++){
	//		bool exist = false;
	//		for (int i=0; i<4; i++){
	//			if (tetra->nodeindex[i]==tetra->GetNodeRecordPtr(j)->GetIndexNo()-1)
	//				exist=true;
	//		}
	//		if (!exist) printf("Not exist: %d tetra, %d node\n", tetra->GetIndexNo(), tetra->GetNodeRecordPtr(j)->GetIndexNo());
	//	}
	//}

	//return true;

	//---------------------------------------------------------------------
	//	Fill in the flags
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {edge->inner=false; break;}
		}
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode *)(trglNodeList.GetNext(Pos));
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {node->inner=false; break;}
		}
	}
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		if (edge->inner) continue;
		if (edge->GetLeftFace() && edge->GetRightFace()) continue;
		edge->SetAttribFlag(0);
		edge->GetStartPoint()->SetAttribFlag(0);
		edge->GetEndPoint()->SetAttribFlag(0);
	}


	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		//if (face->inner) continue;
		face->CalPlaneEquation();
		face->CalCenterPos();
		face->i_inner=face->inner;
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		if (node->inner) continue;
		node->CalNormal();
	}
	

	return true;
}

void QMeshPatch::deleteFace(QMeshFace *face){
	trglFaceList.RemoveAt(face->Pos);
	for (int j=1; j<=3; j++){
		QMeshEdge * edge = face->GetEdgeRecordPtr(j);
		if (!edge) continue;
		//will be an unreference edge
		if (edge->GetTrglFaceList().GetCount()<=1){
			trglEdgeList.RemoveAt(edge->Pos);
			for (int k=0; k<2; k++){
				QMeshNode *node = edge->GetStartPoint();
				if (k==1) node = edge->GetEndPoint();
				//will be an unreference node
				if (node->GetTrglEdgeNumber()<=1){
					trglNodeList.RemoveAt(node->Pos);
					delete node;
				} else { //undate the node
					node->GetTrglEdgeList().Remove(edge);
					node->GetTrglFaceList().Remove(face);
					//node->inner=false;
				}
			}
			delete edge;
		} else {//update the edge
			edge->GetTrglFaceList().Remove(face); //edge->inner=false;
			//if (edge->GetLeftFace()==face) {edge->SetLeftFace(); edge->inner=false;}
			//else if (edge->GetRightFace()==face) {edge->SetRightFace(); edge->inner=false;}
			//else printf("[QMeshPatch::deleteFace]ERROR: unknown face!\n");
		}
	}
	delete face;
}

void QMeshPatch::deleteTetra(QMeshTetra *tetra){
	trglTetraList.RemoveAt(tetra->Pos);
	for (int i=1; i<=4; i++) {
		QMeshFace * face = tetra->GetFaceRecordPtr(i);
		if (!face) continue;
		//will be an unreferenced face
		if (!face->GetLeftTetra() || !face->GetRightTetra()){
			deleteFace(face);
		} else { //update the face
			if (face->GetLeftTetra()==tetra) face->SetLeftTetra();
			else if (face->GetRightTetra()==tetra) face->SetRightTetra();
			else printf("[QMeshPatch::deleteTetra]ERROR: unknown tetra!\n");
			face->i_inner=face->inner=false; //face->CalPlaneEquation(); face->CalCenterPos();
			//for (int m=1; m<=3; m++){
			//	QMeshNode * n = face->GetNodeRecordPtr(m);
			//	n->inner=false;
			//	QMeshEdge *edge = face->GetEdgeRecordPtr(m);
			//	edge->inner=false;
			//	//n->CalNormal();
			//}
		}
	}
	delete tetra;
}

void QMeshPatch::RecomputeAllNormals(){
	//---------------------------------------------------------------------
	//	Fill in the flags
	GLKPOSITION Pos=NULL;
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace*face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		if (face->GetLeftTetra() && face->GetRightTetra()) {face->inner=true;} else face->inner=false;
	}
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {edge->inner=false; break;}
		}
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode *)(trglNodeList.GetNext(Pos));
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {node->inner=false; break;}
		}
	}


	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace *face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		face->i_inner=face->inner;
		if (face->inner) continue;
		QMeshTetra *tetra = face->GetLeftTetra();
		if (!tetra) tetra=face->GetRightTetra();
		int idx = tetra->GetFaceIndex(face);

		if (!tetra->IsNormalDirection(idx)){
			QMeshEdge* edge1 = face->GetEdgeRecordPtr(1);
			QMeshEdge* edge3 = face->GetEdgeRecordPtr(3);
			face->SetEdgeRecordPtr(1, edge3);
			face->SetEdgeRecordPtr(3, edge1);
			BOOL n1 = face->IsNormalDirection(1);
			BOOL n3 = face->IsNormalDirection(3);
			face->SetDirectionFlag(1, n3);
			face->SetDirectionFlag(3, n1);
			for (int i=1; i<=3; i++){
				face->SetDirectionFlag(i, !face->IsNormalDirection(i));
			}
			tetra->SetDirectionFlag(idx);
		}

		face->CalPlaneEquation();
		face->CalCenterPos();
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		node->CalNormal();
	}
}


bool QMeshPatch::inputHEXFile(const char *filename)
{
	FILE *fp;
	char linebuf[256],buf[100];
	int nodeNum,i;	float xx,yy,zz;
	int hexNum;
	GLKPOSITION Pos;
	GLKPOSITION PosNode;	
	QMeshNode *node,*startNode,*endNode;
	QMeshEdge *edge;
	QMeshFace *face;
	QMeshNode **nodeArray;

	fp = fopen(filename, "r");
    if(!fp) {AfxMessageBox("Can not open the data file"); return false;}
	ClearAll();

	printf("reading hex file.........");
	isVolume=true; isTET=false;

	sprintf(buf,""); sprintf(linebuf,"");
	fgets(linebuf, 255, fp);
	sscanf(linebuf,"%d %s \n",&nodeNum, buf);
	if (strcmp(buf, "vertices")!=0){
		AfxMessageBox("Incorrect Tet format! Missing vertices number.");
		fclose(fp);
		return false;
	}
	printf("%d vertices, ", nodeNum);
	sprintf(buf,""); sprintf(linebuf,"");
	fgets(linebuf, 255, fp);
	sscanf(linebuf,"%d %s \n",&hexNum, buf);
	if (strcmp(buf, "hexs")!=0){
		AfxMessageBox("Incorrect Hex format! Missing Hexa number.");
		fclose(fp);
		return false;
	}
	printf("%d hexs\n", hexNum);

	nodeArray=(QMeshNode**)new long[nodeNum];

	for (i=0; i<nodeNum; i++){
		char iden;
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		sscanf(linebuf,"%c %f %f %f \n", &iden, &xx, &yy, &zz);
		if (iden!='v') {AfxMessageBox("vertex is not successfully read!"); return false;}
		node=new QMeshNode;
		node->SetMeshSurfacePtr(this);
		node->SetCoord3D(xx,yy,zz);
		node->SetIndexNo(i+1);
		trglNodeList.AddTail(node);
		nodeArray[i]=node;
	}

	for (i=0; i<hexNum; i++){
		char iden;
		sprintf(buf,""); sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		int n[8];
		sscanf(linebuf,"%c %d %d %d %d %d %d %d %d \n", &iden, &n[0], &n[1], &n[2], &n[3], &n[4], &n[5], &n[6], &n[7]);
		if (iden!='h'){AfxMessageBox("Hexahedron is not successfully read!"); fclose(fp); return false;}
		//recall the notation for face 1,2,3,4,5,6 - encoding for node index on each face
		//	f1 = 1234
		//	f2 = 5876
		//	f3 = 1562
		//	f4 = 2673
		//	f5 = 3784
		//	f6 = 4851
		int n_index[6][4] = {
			{n[0], n[1], n[2], n[3]},
			{n[4], n[7], n[6], n[5]},
			{n[0], n[4], n[5], n[1]},
			{n[1], n[5], n[6], n[2]},
			{n[2], n[6], n[7], n[3]},
			{n[3], n[7], n[4], n[0]}
		};

		QMeshHexa *Hexa = new QMeshHexa;
		Hexa->SetIndexNo(i+1);
		Hexa->SetMeshSurfacePtr(this);
		trglHexaList.AddTail(Hexa);

		for (int j=0; j<8; j++){
			nodeArray[n[j]]->AddHexa(Hexa);
		}

		//for (int j=0; j<4; j++)
		//	Tetra->nodeindex[j] = n[j];

		QMeshFace *f[6];
		for (int j=1; j<=6; j++){
			bool existed=false;
			for (GLKPOSITION Pos = nodeArray[n_index[j-1][0]]->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
				QMeshFace *tmp_face = (QMeshFace*)nodeArray[n_index[j-1][0]]->GetTrglFaceList().GetNext(Pos);
				QMeshNode *tmp_node[4];
				int k=0;
				for (GLKPOSITION Pos1 = tmp_face->GetAttachedList().GetHeadPosition(); Pos1!=NULL; k++)
					tmp_node[k] = (QMeshNode*)tmp_face->GetAttachedList().GetNext(Pos1);
				bool same[4]={0, 0, 0,0};
				for (k=0; k<4; k++){
					for (int m=0; m<4; m++){
						if (tmp_node[k]->GetIndexNo()-1 == n_index[j-1][m]){
							same[k]=true;
							break;
						}
					}
					if (!same[k]) break;
				}
				if (same[0] && same[1] && same[2] && same[3]){
					f[j-1] = tmp_face;
					if (f[j-1]->GetRightHexa()!=NULL) printf("ERROR: f[%d-1]->GetRightHexa()!=NULL\n", j);
					f[j-1]->SetRightHexa(Hexa);
					Hexa->SetDirectionFlag(j, false);
					existed=true; break;
				}
			}
			if (!existed){
				f[j-1] = new QMeshFace();
				f[j-1]->SetMeshSurfacePtr(this);
				f[j-1]->SetIndexNo(trglFaceList.GetCount()+1);
				trglFaceList.AddTail(f[j-1]);
				for (int k=0; k<4; k++){
					f[j-1]->GetAttachedList().AddTail(nodeArray[n_index[j-1][k]]);
					nodeArray[n_index[j-1][k]]->GetTrglFaceList().AddTail(f[j-1]);
				}
				if (f[j-1]->GetLeftHexa()!=NULL) printf("ERROR: f[%d-1]->GetLeftHexa()!=NULL\n", j);
				f[j-1]->SetLeftHexa(Hexa);
				Hexa->SetDirectionFlag(j);
			}
			Hexa->SetFaceRecordPtr(j, f[j-1]);
		}
	}


	delete [](QMeshNode**)nodeArray;

	//printf("%d tets\n", trglTetraList.GetCount());

	SetAttribFlag(1);


	//---------------------------------------------------------------------
	//	Build the topology
	//---------------------------------------------------------------------
	//	Step 1: build the edges
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		QMeshFace*face=(QMeshFace*)(trglFaceList.GetNext(Pos));

		if (face->GetLeftHexa() && face->GetRightHexa()) {face->inner=true;} else face->inner=false;


		int edgeNum=(face->attachedList).GetCount();
		face->SetEdgeNum(edgeNum);
		nodeArray=(QMeshNode**)new long[edgeNum];
		i=0;
		for(PosNode=(face->attachedList).GetHeadPosition();PosNode!=NULL;i++) {
			nodeArray[i]=(QMeshNode*)((face->attachedList).GetNext(PosNode));
			//(nodeArray[i]->GetTrglFaceList()).AddTail(face);
		}
		for(i=0;i<edgeNum;i++) {
			edge=NULL;	startNode=nodeArray[i];	endNode=nodeArray[(i+1)%edgeNum];
			bool bDir;
			for(PosNode=(startNode->GetTrglEdgeList()).GetHeadPosition();PosNode!=NULL;) {
				QMeshEdge *temp=(QMeshEdge *)((startNode->GetTrglEdgeList()).GetNext(PosNode));
				if ((temp->GetStartPoint()==startNode) && (temp->GetEndPoint()==endNode)) {
					edge=temp;	bDir=true;
				}
				else if ((temp->GetStartPoint()==endNode) && (temp->GetEndPoint()==startNode)) {
					edge=temp;	bDir=false;
				}
			}
			if (edge) {
				face->SetEdgeRecordPtr(i+1,edge);
				edge->GetTrglFaceList().AddTail(face);
				if (bDir) {
					face->SetDirectionFlag(i+1,true);
					if (!face->inner) {
						if (edge->GetLeftFace()) {
							printf("edge->GetLeftFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetRightFace()) printf("but right face is null!\n");
							//bool t1 = face->GetLeftHexa(), t2 = face->GetRightHexa();
							//if (t1) deleteHexa(face->GetLeftHexa());
							//if (t2) deleteHexa(face->GetRightHexa());
							continue;
						}
						else edge->SetLeftFace(face);
					}
				}
				else {
					face->SetDirectionFlag(i+1,false);
					if (!face->inner) {
						if (edge->GetRightFace()) {
							printf("edge->GetRightFace()!=NULL\n"); 
							face->m_nIdentifiedPatchIndex=2;
							if (!edge->GetLeftFace()) printf("but left face is null!\n");
							//bool t1 = face->GetLeftHexa(), t2 = face->GetRightHexa();
							//if (t1) deleteHexa(face->GetLeftHexa());
							//if (t2) deleteHexa(face->GetRightHexa());
							continue;
						}
						else edge->SetRightFace(face);
					}
				}
			}
			else {
				edge=new QMeshEdge;
				edge->SetMeshSurfacePtr(this);
				edge->SetStartPoint(startNode);
				edge->SetEndPoint(endNode);
				edge->SetIndexNo(trglEdgeList.GetCount()+1);
				trglEdgeList.AddTail(edge);

				edge->GetTrglFaceList().AddTail(face);
				if (!face->inner) edge->SetLeftFace(face);
				face->SetEdgeRecordPtr(i+1,edge);
				face->SetDirectionFlag(i+1,TRUE);
				(startNode->GetTrglEdgeList()).AddTail(edge);
				(endNode->GetTrglEdgeList()).AddTail(edge);
			}
		}

		delete [](QMeshNode**)nodeArray;

		face->attachedList.RemoveAll();
	}

	//for (Pos=trglHexaList.GetHeadPosition(); Pos!=NULL;){
	//	QMeshHexa *tetra = (QMeshHexa*) trglHexaList.GetNext(Pos);
	//	for (int j=1; j<=4; j++){
	//		bool exist = false;
	//		for (int i=0; i<4; i++){
	//			if (tetra->nodeindex[i]==tetra->GetNodeRecordPtr(j)->GetIndexNo()-1)
	//				exist=true;
	//		}
	//		if (!exist) printf("Not exist: %d tetra, %d node\n", tetra->GetIndexNo(), tetra->GetNodeRecordPtr(j)->GetIndexNo());
	//	}
	//}

	//return true;

	//---------------------------------------------------------------------
	//	Fill in the flags
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {edge->inner=false; break;}
		}
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode *)(trglNodeList.GetNext(Pos));
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {node->inner=false; break;}
		}
	}
	for(Pos=trglEdgeList.GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(trglEdgeList.GetNext(Pos));
		if (edge->inner) continue;
		if (edge->GetLeftFace() && edge->GetRightFace()) continue;
		edge->SetAttribFlag(0);
		edge->GetStartPoint()->SetAttribFlag(0);
		edge->GetEndPoint()->SetAttribFlag(0);
	}


	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;) {
		face=(QMeshFace*)(trglFaceList.GetNext(Pos));
		//if (face->inner) continue;
		face->CalPlaneEquation();
		face->CalCenterPos();
		face->i_inner=face->inner;
	}
	for(Pos=trglNodeList.GetHeadPosition();Pos!=NULL;) {
		node=(QMeshNode*)(trglNodeList.GetNext(Pos));
		if (node->inner) continue;
		node->CalNormal();
	}
	

	return true;
}