#include "StdAfx.h"
#include "QMeshHexa.h"
#include "QMeshNode.h"
#include "QMeshEdge.h"
#include "QMeshFace.h"

QMeshHexa::QMeshHexa(void)
{
	type=5;
}

QMeshHexa::~QMeshHexa(void)
{
}

int QMeshHexa::GetFaceIndex(QMeshFace* face){
	for (int i=1; i<=6; i++)
		if (GetFaceRecordPtr(i)==face) return i;
	return -1;
}

int QMeshHexa::GetNodeIndex(QMeshNode* node){
	for (int i=1; i<=8; i++)
		if (GetNodeRecordPtr(i)==node) return i;
	return -1;
}

int QMeshHexa::GetEdgeIndex(QMeshEdge* edge){
	//for (int i=1; i<=12; i++)
	//	if (GetEdgeRecordPtr(i)==edge) return i;
	return -1;
}

QMeshNode * QMeshHexa::GetNodeRecordPtr( UINT whichNode ){
	switch (whichNode){
		case 1: case 2: case 3: case 4:
			if (!isNormal[0]) whichNode=5-whichNode;
			return faces[0]->GetNodeRecordPtr(whichNode);
			break;
		case 5: case 6: case 7: case 8:
			{
				whichNode-=4;
				QMeshNode *node1 = GetNodeRecordPtr(whichNode);
				for (int i=1; i<=4; i++){
					QMeshNode *node = faces[1]->GetNodeRecordPtr(i);
					for (GLKPOSITION Pos = node->GetTrglEdgeList().GetHeadPosition(); Pos!=NULL;){
						QMeshEdge *edge = (QMeshEdge*)node->GetTrglEdgeList().GetNext(Pos);
						QMeshNode *onode = edge->GetStartPoint();
						if (onode==node) onode=edge->GetEndPoint();
						if (onode==node1) return node;
					}
				}
				printf("Hexa get null node!\n");
				return NULL;
			}
			break;
		default:
			printf("Getting %d node from Hexa?\n", whichNode);
			system("pause");
			break;
	}
}