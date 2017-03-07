/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR 
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE 
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF 
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED 
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY 
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/
#include "stdafx.h"
#include "Surface.h"
#include "../Geometry/Transform3D.h"
#include "../Geometry/Line2D.h"
#include "../Geometry/TriTri.h"
#include "../Base/Defs.h"
#include "VolInt.h"
#include <stdio.h>

#ifdef USE_RHINO_IO
bool Surface::loadMesh(const CRhinoMesh &mesh){
  int baseV = vertices.getSize();
  int baseT = triangles.getSize();

  //  load vertices
  const int numVert = mesh.NumVertices();
  vertices.resize(baseV + numVert);
  for (int i = 0; i < numVert; i++){
    double x, y, z;
    mesh.GetVertex(i, x, y, z);

    Point *p = &vertices.index(baseV+i);
    p->p.x = x;
    p->p.y = y;
    p->p.z = z;

    pMin.storeMin(p->p);
    pMax.storeMax(p->p);
    }

  //  load faces (creating vertices)
  Vector3D V1, V2;
  const int numFace = mesh.NumFaces();
  for (i = 0; i < numFace; i++){
    int v0, v1, v2, v3;
    mesh.GetFace(i, v0, v1, v2, v3);
    Triangle *t = &triangles.addItem();
    t->f[0] = t->f[1] = t->f[2] = -1;
    t->v[0] = v0 + baseV;
    t->v[1] = v1 + baseV;
    t->v[2] = v2 + baseV;

    //  triangle normal
    V1.difference(vertices.index(t->v[1]).p, vertices.index(t->v[0]).p);
    V2.difference(vertices.index(t->v[2]).p, vertices.index(t->v[0]).p);
    t->n.cross(V1,V2);
    t->n.norm();

    //  add second triangle
    if (v2 != v3){
      Triangle *t = &triangles.addItem();
      t->f[0] = t->f[1] = t->f[2] = -1;
      t->v[0] = v0 + baseV;
      t->v[1] = v2 + baseV;
      t->v[2] = v3 + baseV;

      V1.difference(vertices.index(t->v[1]).p, vertices.index(t->v[0]).p);
      V2.difference(vertices.index(t->v[2]).p, vertices.index(t->v[0]).p);
      t->n.cross(V1,V2);
      t->n.norm();
      }
    }

  //  connection to other triangles
  setupAdjacent(baseT, triangles.getSize());

  return TRUE;
}
#endif

bool Surface::loadRhinoSurface(const char *fileName, char **err, float boxSize){
#ifdef USE_RHINO_IO
  //  read rhino file
  FILE *f = fopen(fileName, "rb");
  if (!f)
    return FALSE;

  //  initialise bounding box
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  //  load objects
  BOOL ok = TRUE;
  try{
    CRhinoFile RF(f, CRhinoFile::read);
    CRhinoFile::eStatus status = CRhinoFile::good;

    for(;;){
      CRhinoObject *obj = RF.Read(status);
      if (status == CRhinoFile::good){
        //  load object
        if (obj->TypeCode() == TCODE_MESH_OBJECT){
          loadMesh(*(const CRhinoMesh*)obj);
          }

        //rhfree(obj);  --  doesn't work
        delete obj;
        }
      else{
        break;
        }
      }
    }
  catch(CRhinoException e){
    // if something goes wrong, catch the exception here
    if (err){
      switch ( e.m_type ) {
      case CRhinoException::unable_to_write:
        *err = "unable_to_write";
        break;
      case CRhinoException::unable_to_read:
        *err = "unable_to_read";
        break;
      case CRhinoException::unable_to_seek:
        *err = "unable_to_seek";
        break;
      case CRhinoException::unexpected_end_of_file:
        *err = "unexpected_end_of_file";
        break;
      case CRhinoException::unexpected_value:
        *err = "unexpected_value";
        break;
      case CRhinoException::not_supported:
        *err = "not_supported";
        break;
      case CRhinoException::illegal_input:
        *err = "illegal_input";
        break;
      case CRhinoException::out_of_memory:
        *err = "out_of_memory";
        break;
      default:
        *err = "unknow exception";
        break;
        }
      }
    ok = FALSE;
    }
  fclose(f);

  //  fix neighbours
  stitchTriangles();

  //  work out vertex normals
  setupNormals(0, vertices.getSize(), 0, triangles.getSize());

  if (boxSize > 0)
    fitIntoBox(boxSize);

  return ok;
#else
  //  3DM file format disabled
  return FALSE;
#endif
}

float Surface::fitIntoBox(float boxSize){
  //  calculate scale to fit into a 2*2*2 box
  float sX = 	2.0/(pMax.x - pMin.x);
  float sY = 	2.0/(pMax.y - pMin.y);
  float sZ = 	2.0/(pMax.z - pMin.z);
  float scale = sX;
  if (sY < scale)
	  scale = sY;
  if (sZ < scale)
	  scale = sZ;
  scale *= boxSize;

  float cX = (pMax.x + pMin.x) / 2.0f;
  float cY = (pMax.y + pMin.y) / 2.0f;
  float cZ = (pMax.z + pMin.z) / 2.0f;

  int numVert = vertices.getSize();
  for (int i = 0; i < numVert; i++){
    //  pointer to vertex information
    Point*p = &vertices.index(i);
    p->p.x = (p->p.x-cX)*scale;
    p->p.y = (p->p.y-cY)*scale;
    p->p.z = (p->p.z-cZ)*scale;
    }

  //  update bounds
  pMin.x = (pMin.x-cX)*scale;
  pMin.y = (pMin.y-cY)*scale;
  pMin.z = (pMin.z-cZ)*scale;
  pMax.x = (pMax.x-cX)*scale;
  pMax.y = (pMax.y-cY)*scale;
  pMax.z = (pMax.z-cZ)*scale;

  return scale;
}

int Surface::findTriangle(int tri, int v1, int v2, int mn, int mx) const{
  CHECK_DEBUG0(mn <= mx);

//  Point3D p1 = vertices.index(v1).p;
//  Point3D p2 = vertices.index(v2).p;

  for (int i = mn; i < mx; i++)
    if (i != tri){
      const Triangle *tr = &triangles.index(i);
      for (int j = 0; j < 3; j++){
        int u1 = j, u2 = (j+1)%3;
//        Point3D q1 = vertices.index(tr->v[u1]).p;
//        Point3D q2 = vertices.index(tr->v[u2]).p;

        if ((tr->v[u1] == v1 && tr->v[u2] == v2) ||
            (tr->v[u1] == v2 && tr->v[u2] == v1)){
          return i;
          }
        }
      }

  return -1;
}

void Surface::setupAdjacent(int mn, int mx){
  CHECK_DEBUG0(mn <= mx);

  for (int i = mn; i < mx; i++){
    Triangle *tr = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      int v1 = j, v2 = (j+1)%3;
      tr->f[j] = findTriangle(i, tr->v[v1], tr->v[v2], mn, mx);
      }
    }
}

//  our own format with edge adjacency
bool Surface::loadSurface(const char *fileName, float boxSize){
  FILE *f = fopen(fileName, "r");
  if (!f)
    return FALSE;

  //  initialise bounding box
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  //  load points
  int numPts;
  fscanf(f, "%d", &numPts);
  vertices.resize(numPts);
  for (int i = 0; i < numPts; i++){
    Point *p = &vertices.index(i);

    float x, y, z;
    fscanf(f, "%f%f%f", &x, &y, &z);
    p->p.x = x;
    p->p.y = y;
    p->p.z = z;

    pMin.storeMin(p->p);
    pMax.storeMax(p->p);

    fscanf(f, "%f%f%f", &x, &y, &z);
    p->n.x = x;
    p->n.y = y;
    p->n.z = z;
    }

  //  load triangle
  int numTri;
  fscanf(f, "%d", &numTri);
  triangles.resize(numTri);
  for (int j = 0; j < numTri; j++){
    Triangle *tri = &triangles.index(j);

    //  vertices
    int a, b, c;
    fscanf(f, "%d%d%d", &a, &b, &c);
    tri->v[0] = a;
    tri->v[1] = b;
    tri->v[2] = c;

    //  neighbouring faces
    fscanf(f, "%d%d%d", &a, &b, &c);
    tri->f[0] = a;
    tri->f[1] = b;
    tri->f[2] = c;

    //  triangle normal
    Vector3D v1, v2;
    v1.difference(vertices.index(tri->v[1]).p, vertices.index(tri->v[0]).p);
    v2.difference(vertices.index(tri->v[2]).p, vertices.index(tri->v[0]).p);
    tri->n.cross(v1,v2);
    tri->n.norm();
    }

  fclose(f);

  if (boxSize > 0)
    fitIntoBox(boxSize);

  return TRUE;
}

bool Surface::loadMinimalSurface(const char *fileName, float boxSize){
  FILE *f = fopen(fileName, "r");
  if (!f)
    return FALSE;

  //  initialise bounding box
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  //  load points
  int numPts;
  fscanf(f, "%d", &numPts);
  vertices.resize(numPts);
  for (int i = 0; i < numPts; i++){
    Point *p = &vertices.index(i);

    //  load position
    float x, y, z;
    fscanf(f, "%f%f%f", &x, &y, &z);
    p->p.x = x;
    p->p.y = y;
    p->p.z = z;

    pMin.storeMin(p->p);
    pMax.storeMax(p->p);

    //  no normal info
    }

  //  load triangle
  int numTri;
  fscanf(f, "%d", &numTri);
  triangles.resize(numTri);
  for (int j = 0; j < numTri; j++){
    Triangle *tri = &triangles.index(j);

    //  load vertices
    int a, b, c;
    fscanf(f, "%d%d%d", &a, &b, &c);
    tri->v[0] = a;
    tri->v[1] = b;
    tri->v[2] = c;

    //  no neighbours
    }
  fclose(f);

  //  work out vertex normals
  setupNormals(0, vertices.getSize(), 0, triangles.getSize());

  //  work out adjacency info
  setupAdjacent(0, triangles.getSize());

  if (boxSize > 0)
    fitIntoBox(boxSize);

  return TRUE;
}

bool Surface::saveSurface(const char *fileName, float scale){
  FILE *f = fopen(fileName, "w");
  if (!f)
    return FALSE;

  //  dump vertices
  int numPoints = vertices.getSize();
  fprintf(f, "%d\n", numPoints);
  for (int i = 0; i < numPoints; i++){
    Point p = vertices.index(i);
    if (!fprintf(f, "%f %f %f %f %f %f\n", p.p.x*scale, p.p.y*scale, p.p.z*scale, p.n.x, p.n.y, p.n.z)){
      fclose(f);
      return FALSE;
      }
    }

  //  dump triangles
  int numTris = triangles.getSize();
  fprintf(f, "%d\n", numTris);
  for (int j = 0; j < numTris; j++){
    Triangle t = triangles.index(j);
    if (!fprintf(f, "%d %d %d %d %d %d\n", t.v[0], t.v[1], t.v[2], t.f[0], t.f[1], t.f[2])){
      fclose(f);
      return FALSE;
      }
    }

  fclose(f);
  return TRUE;
}

bool Surface::saveHavokSurface(const char *fileName){
  FILE *f = fopen(fileName, "w");
  if (!f)
    return FALSE;

  //  dump vertices
  int numPoints = vertices.getSize();
  fprintf(f, "%d\n", numPoints);
  for (int i = 0; i < numPoints; i++){
    Point p = vertices.index(i);
    if (!fprintf(f, "%f %f %f\n", p.p.x, p.p.y, p.p.z)){
      fclose(f);
      return FALSE;
      }
    }

  //  dump triangles
  int numTris = triangles.getSize();
  fprintf(f, "%d\n", numTris);
  for (int j = 0; j < numTris; j++){
    Triangle t = triangles.index(j);
    if (!fprintf(f, "3 %d %d %d\n", t.v[0], t.v[1], t.v[2])){
      fclose(f);
      return FALSE;
      }
    }

  fclose(f);
  return TRUE;
}

void Surface::getTriangleNormal(Vector3D *n, int trI) const
{
  const Triangle *tri = &triangles.index(trI);
  *n = tri->n;
/*
  Point3D pts[3];
  for (int j = 0; j < 3; j++)
    pts[j] = vertices.index(tri->v[j]).p;

  Vector3D v1, v2, vC;
  v1.difference(pts[1], pts[0]);
  v2.difference(pts[2], pts[0]);
  n->cross(v1, v2);
  n->norm();
*/
}

void Surface::getVertexNormal(Vector3D *n, int vertex) const{
  *n = vertices.index(vertex).n;
}

//  get normal of point in triangle using bi-linear interpolation
void Surface::getNormal(Vector3D *n, const Point3D &p, int tri) const{
  //  get points
  const Triangle *triangle = &triangles.index(tri);
  const Point *p0 = &vertices.index(triangle->v[0]);
  const Point *p1 = &vertices.index(triangle->v[1]);
  const Point *p2 = &vertices.index(triangle->v[2]);

  //  get plane transform
  Vector3D v1, v2, vC;
  v1.difference(p1->p, p0->p);
  v2.difference(p2->p, p0->p);
  vC.cross(v1, v2);
  Transform3D tr;
  tr.alignToZ(vC, p0->p);

  //  transform points to plane
  Point2D P0, P1, P2, Pt;
  tr.transformTo2D(&P0, p0->p);
  tr.transformTo2D(&P1, p1->p);
  tr.transformTo2D(&P2, p2->p);
  tr.transformTo2D(&Pt, p);

  //  get intersection point
  Line2D l0(P0, P1), l2(Pt, P2);
  Point2D pI;
  l0.intersect(&pI, l2);

  //  interpolate pI's value between P0 & P1
  float d01 = pI.distance(P0) / P1.distance(P0);
  Vector3D vI;
  vI.x = (1-d01)*p0->n.x + d01*p1->n.x;
  vI.y = (1-d01)*p0->n.y + d01*p1->n.y;
  vI.z = (1-d01)*p0->n.z + d01*p1->n.z;

  //  interpolate Pt's value from P2 and pI
  float d = Pt.distance(P2) / pI.distance(P2);
  n->x = (1-d)*p2->n.x + d*vI.x;
  n->y = (1-d)*p2->n.y + d*vI.y;
  n->z = (1-d)*p2->n.z + d*vI.z;
}

void Surface::getEdgeNormal(Vector3D *n, int tri, int edge) const{
  const Triangle *tr = &triangles.index(tri);

  Vector3D n1, n2;
  getTriangleNormal(&n1, tri);
  getTriangleNormal(&n2, tr->f[edge]);

  n->add(n1, n2);
  n->norm();
}

void Surface::findNeighbours(Array<int> *tris, int vertex, int startTri) const{
  //  find start triangle
  if (startTri < 0){
    int numTri = triangles.getSize();
    for (int i = 0; i < numTri; i++){
      const Triangle *tr = &triangles.index(i);
      if (tr->v[0] == vertex || tr->v[1] == vertex || tr->v[2] == vertex)
        break;
      }
    if (i != numTri)
      startTri = i;
    else
      return;
    }

  //  add start triangle to list
  tris->resize(0);
  tris->addItem() = startTri;

  int l = 0;
  while (l < tris->getSize()){
    int tI = tris->index(l);
    const Triangle *tr = &triangles.index(tI);
    for (int i = 0; i < 3; i++){
      int nI = tr->f[i];
      if (nI >= 0 && !tris->inList(nI)){
        const Triangle *trN = &triangles.index(nI);
        if (trN->v[0] == vertex || trN->v[1] == vertex || trN->v[2] == vertex)
          tris->addItem() = nI;
        }
      }
    l++;
    }
}

void Surface::setupNormals(int mn, int mx, int tn, int tx){
  //  init normals
  for (int i = mn; i < mx; i++){
    Point *p = &vertices.index(i);
    p->n.x = 0;
    p->n.y = 0;
    p->n.z = 0;
    }

  //  sum up normals from faces
  int numTri = triangles.getSize();
  for (i = tn; i < tx; i++){
    Vector3D n;
    getTriangleNormal(&n, i);

    const Triangle *t = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      Point *p = &vertices.index(t->v[j]);
      p->n.x += n.x;
      p->n.y += n.y;
      p->n.z += n.z;
      }
    }

  //  normalise normals
  for (i = mn; i < mx; i++)
    vertices.index(i).n.norm();
}

void Surface::stitchTriangles(){
  //  fix up triangles
  int numTri = triangles.getSize();
  for (int i = 0; i < numTri; i++){
    Triangle *t = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      if (t->f[j] < 0){
        //  points from unmatched edge
        int v0 = t->v[j], v1 = t->v[(j+1)%3];
        Point3D p0 = vertices.index(v0).p;
        Point3D p1 = vertices.index(v1).p;

        //  search for match
        REAL minD = REAL_MAX;
        int minT = -1, minV1 = -1, minV2 = -1, minE = -1;
        for (int tri = 0; tri < numTri; tri++){
          if (tri == i)
            continue;

          //  candidate match
          Triangle *t1 = &triangles.index(tri);
          for (int e = 0; e < 3; e++){
            if (t1->f[e] >= 0)
              continue;

            //  points from unmatched edge
            Point3D q0 = vertices.index(t1->v[e]).p;
            Point3D q1 = vertices.index(t1->v[(e+1)%3]).p;

            //  forward edge
            float d = p0.distance(q0) + p1.distance(q1);
            if (d < minD){
              minD = d;
              minT = tri;
              minV1 = t1->v[e];
              minV2 = t1->v[(e+1)%3];
              minE = e;
              }

            //  reverse edge
            d = p0.distance(q1) + p1.distance(q0);
            if (d < minD){
              minD = d;
              minT = tri;
              minV2 = t1->v[e];
              minV1 = t1->v[(e+1)%3];
              minE = e;
              }
            }
          }
//      CHECK_DEBUG0(minT >= 0);

        if (minT >= 0){        
          //OUTPUTINFO("minD = %f\n", minD);

          //  link triangles
          t->f[j] = minT;
          triangles.index(minT).f[minE] = i;

          //  merge first vertex
          if (minV1 != v0){
            remapVertex(minV1, v0);

            int lastV = vertices.getSize()-1;
            if (minV1 != lastV){
              vertices.index(minV1) = vertices.index(lastV);
              remapVertex(lastV, minV1);
              if (minV2 == lastV)
                minV2 = minV1;
              if (v1 == lastV)
                v1 = minV1;
              }
            vertices.setSize(lastV);
            }

          if (minV2 != v1){
            remapVertex(minV2, v1);
            int lastV = vertices.getSize()-1;
            if (minV2 != lastV){
              vertices.index(minV2) = vertices.index(lastV);
              remapVertex(lastV, minV2);
              }
            vertices.setSize(lastV);
            }
          }
        }
      }
    }
}

void Surface::remapVertex(int s, int d){
  int numTri = triangles.getSize();
  for (int i = 0; i < numTri; i++){
    Triangle *t = &triangles.index(i);

    for (int j = 0; j < 3; j++){
      if (t->v[j] == s)
        t->v[j] = d;
      }
    }
}

bool Surface::vertexHasRef(int s){
  int numTri = triangles.getSize();
  for (int i = 0; i < numTri; i++){
    Triangle *t = &triangles.index(i);

    for (int j = 0; j < 3; j++){
      if (t->v[j] == s)
        return true;
      }
    }
  return false;
}

void Surface::getBoundingSphere(Sphere *s) const{
  int numPts = vertices.getSize();

  //  first pass - find maxima/minima points for the 3 axes
  Point3D xMin, xMax, yMin, yMax, zMin, zMax;
  xMin.x = yMin.y = zMin.z = REAL_MAX;
  xMax.x = yMax.y = zMax.z = REAL_MIN;
  for (int i = 0; i < numPts; i++){
    Point3D p = vertices.index(i).p;

    if (p.x < xMin.x)
      xMin = p;
    if (p.x > xMax.x)
      xMax = p;

    if (p.y < yMin.y)
      yMin = p;
    if (p.y > yMax.y)
      yMax = p;

    if (p.z < zMin.z)
      zMin = p;
    if (p.z > zMax.z)
      zMax = p; 
    }

  //  x-span is square distance between xmin & xmax
  float xspan = xMin.distanceSQR(xMax);
  float yspan = yMin.distanceSQR(yMax);
  float zspan = zMin.distanceSQR(zMax);

  //  dMin and dMax are the maximally separated pair
  float maxSpan = xspan;
  Point3D dMin = xMin, dMax = xMax;
  if (yspan > maxSpan){
    maxSpan = yspan;
    dMin = yMin;
    dMax = yMax;
    }
  if (zspan > maxSpan){
    maxSpan = zspan;
    dMin = zMin;
    dMax = zMax;
    }

  //  work out center and radius of sphere
  Point3D pC;
  pC.x = (dMin.x + dMax.x)/2.0f;
  pC.y = (dMin.y + dMax.y)/2.0f;
  pC.z = (dMin.z + dMax.z)/2.0f;
  float rCS = pC.distanceSQR(dMax);
  float rC = sqrt(rCS);

  //  second pass - increment current sphere
  for (i = 0; i < numPts; i++){
    Point3D p = vertices.index(i).p;

    float oldToPS = pC.distanceSQR(p);
    if (oldToPS > rCS){
      //  this point is outside the sphere
      float oldToP = sqrt(oldToPS);
      rC = (rC + oldToP)/2.0;
      rCS = rC*rC;
      float oldToNew = oldToP - rC;

      //  new sphere center
      pC.x = (rC*pC.x + oldToNew*p.x)/oldToP;
      pC.y = (rC*pC.y + oldToNew*p.y)/oldToP;
      pC.z = (rC*pC.z + oldToNew*p.z)/oldToP;
      }
    }

  s->c = pC;
  s->r = rC;
}

void Surface::copy(const Surface &other){
  pMin = other.pMin;
  pMax = other.pMax;

  int numVerts = other.vertices.getSize();
  vertices.resize(numVerts);
  vertices.copy(other.vertices);

  int numTris = other.triangles.getSize();
  triangles.resize(numTris);
  triangles.copy(other.triangles);
}

bool Surface::hasEdge(const Triangle *tri, int e[2]){
  bool has[2] = {false, false};
  for (int i = 0; i < 3; i++){
    if (tri->v[i] == e[0])
      has[0] = true;
    if (tri->v[i] == e[1])
      has[1] = true;
    }
  return has[0] && has[1];
}

bool Surface::hasVertex(const Triangle *tri, int v){
  return (tri->v[0] == v || tri->v[1] == v || tri->v[2] == v);
}

void Surface::testSurface(int *badVerts, int *badNeighs, int *openEdges, int *multEdges, int *badFaces) const{
  *badVerts = 0;
  *badNeighs = 0;
  *openEdges = 0;
  *multEdges = 0;
  *badFaces = 0;

  int numTri = triangles.getSize();
  int numVert = vertices.getSize();

  //  check for open edges and bad vertices
  OUTPUTINFO("Testing Open Edges...\n");
  for (int i = 0; i < numTri; i++){
    const Triangle *tri = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      if (tri->v[j] < 0 || tri->v[j] >= numVert)
        (*badVerts)++;
      if (tri->f[j] < 0 || tri->f[j] >= numTri || tri->f[j] == i)
        (*openEdges)++;
      }
    }

  //  check for multiple edges
  OUTPUTINFO("Testing Multi/Bad Edges...\n");
  for (i = 0; i < numTri; i++){
    const Triangle *tri = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      int e[2];
      e[0] = tri->v[j];
      e[1] = tri->v[(j+1)%3];

      //  count triangles with this edge
      int numWithEdge = 0;
      for (int k = 0; k < numTri; k++){
        if (k == i)
          continue;

        const Triangle *tri = &triangles.index(k);
        if (hasEdge(tri, e))
          numWithEdge++;
        }

      //  work out what is wrong
      if (numWithEdge > 1){
        //  too many neighbours
        (*multEdges)++;
        }
      else if (numWithEdge == 1){
        //  check that the neighbour has a reference back to us
        const Triangle *tri1 = &triangles.index(tri->f[j]);
        if ((tri1->f[0] != i) && (tri1->f[1] != i) && (tri1->f[2] != i))
          (*badNeighs)++;
        }
      }
    }

  //  check each triangle for intersections
  OUTPUTINFO("Testing Intersecting Faces...\n");
  for (i = 0; i < numTri; i++){
    const Triangle *tri = &triangles.index(i);
    Point3D t1[3];
    t1[0] = vertices.index(tri->v[0]).p;
    t1[1] = vertices.index(tri->v[1]).p;
    t1[2] = vertices.index(tri->v[2]).p;

    for (int j = i+1; j < numTri; j++){
      const Triangle *tri1 = &triangles.index(j);
      if (hasVertex(tri1, tri->v[0]) || hasVertex(tri1, tri->v[1]) || hasVertex(tri1, tri->v[2]))
        continue;

      Point3D t2[3];
      t2[0] = vertices.index(tri1->v[0]).p;
      t2[1] = vertices.index(tri1->v[1]).p;
      t2[2] = vertices.index(tri1->v[2]).p;

      if (trianglesIntersect(t1, t2))
        (*badFaces)++;
      }
    }
}

//  mass properties - taken from mirtich's code
void Surface::computeMassProperties(REAL *mass, REAL com[3], REAL it[3][3]) const{
  ::computeMassProperties(mass, com, it, *this);
}

//  compute bounding box
void Surface::setupBoundingBox(){
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  int numVert = vertices.getSize();
  for (int i = 0; i < numVert; i++){
    Point3D p = vertices.index(i).p;
    pMin.storeMin(p);
    pMax.storeMax(p);
    }  
}

int Surface::findNearestVertex(Point3D &v)
{
	float dst,mindst=1000;
	Point3D *pV;
	int pMinID;
	int i;
	for(i=0;i<vertices.getSize();i++)
	{
		pV = &vertices.index(i).p;
		dst=v.distanceSQR(*pV);
		if(dst<mindst)
		{
			mindst=dst;
			pMinID=i;
		}
	}
	return pMinID;
}

void Surface::setupMeshGeoInfo()
{
	initialStoreSpace();
	
	int i;
	// setup triangle info around point
	for(i = 0; i< triangles.getSize(); i++)
	{
		Triangle *tri = &triangles.index(i);

		for(int j = 0; j<3; j++)
		{
			Point *p = &vertices.index(tri->v[j]);
			p->addFID(i);
		}
	}
	// setup all edges info 
	for(i = 0; i< triangles.getSize(); i++)
	{
		Triangle *tri = &triangles.index(i);

		MESH_EDGE e;
		e.f[0] = i;
		for(int j = 0; j< 3; j++)  // for each edge in the triangle
		{
			e.v[0] = tri->v[j]; e.v[1] = tri->v[(j+1)%3];

			int eid = isInEdges(&e);
			if(eid<0)  // generate new a MESH_EDGE
			{
				e.f[1] = -1;
				MESH_EDGE *newe = &edges.addItem();
				newe->f[0] = e.f[0];  newe->f[1] = e.f[1];
				newe->v[0] = e.v[0];  newe->v[1] = e.v[1];
				tri->e[j] = edges.getSize() -1 ;
			}
			else
			{
				tri->e[j] = eid;
			}
		}
	}
	// setup the surface info connected triangle
	for(i = 0; i< triangles.getSize(); i++)
	{
		Triangle *tri = &triangles.index(i);
		for(int j = 0; j<3; j++)
		{
			MESH_EDGE *e = &edges.index(tri->e[j]);
			tri->f[j] = (i == e->f[0] ? e->f[1] : e->f[0]);
		}
	}
	// setup the edges info connected vertex
	for(i = 0; i< edges.getSize(); i++)
	{
		MESH_EDGE *edge = &edges.index(i);

		Point *p1 = &vertices.index(edge->v[0]);
		p1->addEID(i);
		Point *p2 = &vertices.index(edge->v[1]);
		p2->addEID(i);

		edge->cost = edge->length = p1->p.distance(p2->p);
	}
	/*
//	Debug  vertices
	OUTPUTINFO("*****************Vertices******************\n");
	for(i = 0; i< vertices.getSize();i++)
	{
		OUTPUTINFO("%d vertex:\n",i);
		Point *pt = &vertices.index(i);
		OUTPUTINFO("f: ");
		for(int j = 0; j< pt->fNum; j++)
		{
			OUTPUTINFO("%d ",pt->fids.index(j));
		}
		OUTPUTINFO("\ne: ");
		for(j = 0; j< pt->eNum; j++)
		{
			OUTPUTINFO("%d ",pt->eids.index(j));
		}
		OUTPUTINFO("\n");
	}
//
//	Debug	triangles
	OUTPUTINFO("*****************Triangles******************\n");
	for(i=0; i< triangles.getSize(); i++)
	{
		OUTPUTINFO("%d triangle:\n",i);
		Triangle *tri = &triangles.index(i);
		OUTPUTINFO("v: ");
		for(int j = 0; j<3; j++)
		{
			OUTPUTINFO("%d ",tri->v[j]);
		}
		OUTPUTINFO("\nf: ");
		for(j = 0; j<3; j++)
		{
			OUTPUTINFO("%d ",tri->f[j]);
		}
		OUTPUTINFO("\ne: ");
		for(j = 0; j<3; j++)
		{
			OUTPUTINFO("%d ", tri->e[j]);
		}
		OUTPUTINFO("\n");
	}

//	Debug edges
	OUTPUTINFO("*****************Edges******************\n");
	for(i = 0; i<edges.getSize(); i++ )
	{
		OUTPUTINFO("%d Edge:\n",i);
		MESH_EDGE *edge = &edges.index(i);
		OUTPUTINFO("v: ");
		for(int j = 0; j< 2; j++)
		{
			OUTPUTINFO("%d ",edge->v[j]);
		}
		OUTPUTINFO("\nf: ");
		for(j = 0; j<2; j++)
		{
			OUTPUTINFO("%d ",edge->f[j]);
		}
		OUTPUTINFO("\n");

	}
	*/
}

void Surface::initialStoreSpace()
{
	if(edges.getSize()>0)
	{
		edges.free();
	}
	edges.setSize(0);

	int i;
	for(i = 0; i<vertices.getSize(); i++)
	{
		Point *p = &vertices.index(i);
		if(p->fids.getSize()>0) p->fids.free(); 
		p->fids.setSize(0);	p->fNum = 0;
		if(p->eids.getSize()>0) p->eids.free(); 
		p->eids.setSize(0); p->eNum = 0;
	}

	for(i = 0; i< triangles.getSize(); i++)
	{
		Triangle *tri = &triangles.index(i);
		for(int j = 0; j<3; j++)
		{
			tri->e[j] = -1;
			tri->f[j] = -1;
		}
	}
}

int Surface::isInEdges(MESH_EDGE *e)
{
	int eid = -1;
	// for the first vertex of the edge
	Point *p = &vertices.index(e->v[0]);  
	for(int k = 0; k < p->fNum ; k++) // for each triangle around of the vertex
	{
		int fid = p->fids.index(k);
		if(fid != e->f[0])  // the triangle is not the current one
		{
			Triangle *tri1 = &triangles.index(fid);
			
			for(int h = 0 ; h<3; h++)
			{
				if(tri1->v[h] == e->v[1]) // if the other vertex of the edge is one of the triangle vertices
				{
					e->f[1] = fid;
					if(fid < e->f[0])  // the edge has been processed.
					{
						for(int i = 0; i<3; i++)
						{
							MESH_EDGE *edge = &edges.index(tri1->e[i]);
							if((edge->v[0] == e->v[1] && edge->v[1] == e->v[0]) ||
								(edge->v[0] == e->v[0] && edge->v[1] == e->v[1]))
							{
								edge->f[1] = e->f[0];
								eid = tri1->e[i];
								return eid;
							}
						}
					}
				}
			}
		}
	}
	return eid;
}

Surface::Surface(void)
{
}
Surface::~Surface(void)
{
	if(triangles.getSize()>0)
	{
		triangles.clear();
	}
	if(vertices.getSize()>0)
	{
		for(int i = 0; i<vertices.getSize(); i++)
		{
			if(vertices.index(i).fids.getSize()>0) 
				vertices.index(i).fids.clear();
			if(vertices.index(i).eids.getSize()>0)
				vertices.index(i).eids.clear();
		}
		vertices.clear();
	}
	if(edges.getSize()>0)
	{
		edges.clear();
	}
	if(e_ex_properties.getSize()>0)
		e_ex_properties.free();
	if(v_ex_properties.getSize()>0)
	{
		for(int i = 0; i< v_ex_properties.getSize(); i++)
		{
			if(v_ex_properties.index(i).f_e3_ids.getSize()>0)
			{
				v_ex_properties.index(i).f_e3_ids.free();
				v_ex_properties.index(i).f_v_distance.free();
				v_ex_properties.index(i).f_v_id.free();
			}
		}
		v_ex_properties.free();
	}
}

void Surface::outputMeshInfo(int pID, int pType)
{
	switch(pType)
	{
	case 1:
		{
			if(pID>=0 && pID<vertices.getSize())
			{
				OUTPUTINFO("****%d vertex****\n",pID);
				Point *pt = &vertices.index(pID);
				OUTPUTINFO("f: ");
				for(int i = 0; i<pt->fNum; i++)
				{
					OUTPUTINFO("%d ", pt->fids.index(i));
				}
				OUTPUTINFO("\ne: ");
				for(i = 0; i<pt->eNum; i++)
				{
					OUTPUTINFO("%d ", pt->eids.index(i));
				}

				OUTPUTINFO("\nf_e3_ids: ");
				VERTEX_EXTRA_PROPERTY *v_pt = &v_ex_properties.index(pID);
				for(i = 0; i<pt->fNum; i++)
				{
					OUTPUTINFO("%d ", v_pt->f_e3_ids.index(i));
				}
				
				OUTPUTINFO("\np: %f %f %f", pt->p.x, pt->p.y, pt->p.z);
				OUTPUTINFO("\nvIndex: %d \n", pt->vIndex);
			}
			else
			{
				OUTPUTINFO("\nError!\n");
			}
		}
		break;
	case 2:
		{
			if(pID>=0 && pID<edges.getSize())
			{
				OUTPUTINFO("****%d edge****\n",pID);
				MESH_EDGE *eg = &edges.index(pID);
				OUTPUTINFO("f: %d %d\n", eg->f[0],eg->f[1]);
				OUTPUTINFO("v: %d %d\n", eg->v[0],eg->v[1]);

				EDGE_EXTRA_PROPERTY *e_pt = &e_ex_properties.index(pID);
				OUTPUTINFO("f_v3_id: %d %d\n", e_pt->f_v3_id[0], e_pt->f_v3_id[1]);

				OUTPUTINFO("f_v3_2D[0]: %f %f\n", e_pt->f_v3_2D[0].x, e_pt->f_v3_2D[0].y);
				OUTPUTINFO("f_v3_2D[1]: %f %f\n", e_pt->f_v3_2D[1].x, e_pt->f_v3_2D[1].y);

				OUTPUTINFO("f_v_eid[0]: %d %d\n", e_pt->f_v_eid[0][0], e_pt->f_v_eid[0][1]);
				OUTPUTINFO("f_v_eid[1]: %f %f\n", e_pt->f_v_eid[1][0], e_pt->f_v_eid[1][1]);
			}
			else
			{
				OUTPUTINFO("\nError!\n");
			}
		}
		break;
	case 3:
		{
			if(pID>=0 && pID<triangles.getSize())
			{
				OUTPUTINFO("**********%d triangle*********\n",pID);
				Triangle *tri = &triangles.index(pID);
				OUTPUTINFO("f: %d %d %d\n", tri->f[0],tri->f[1],tri->f[2]);
				OUTPUTINFO("e: %d %d %d\n", tri->e[0],tri->e[1],tri->e[2]);
				OUTPUTINFO("v: %d %d %d\n", tri->v[0],tri->v[1],tri->v[2]);
			}
			else
			{
				OUTPUTINFO("\nError!\n");
			}
		}
		break;
	}
}


int Surface::findEdge(int id1, int id2, int flag)
{
	//  flag == 0   根据两个顶点序号查找其所在边序号
	//  flag == 1   根据两个三角形序号查找其公共边序号

	if(flag == 1)  //  三角形
	{
	//  At first, if the id1,id2 are valid
		if(!(id1 >=0 && id1<triangles.getSize() && id2 >=0 && id2<triangles.getSize()))
		{
			OUTPUTINFO("Error!");
			return -1;
		}
		Triangle *tri1,*tri2;
		tri1 = &triangles.index(id1);
		tri2 = &triangles.index(id2);
		for(int i = 0; i< 3; i++)
		{
			for(int j = 0; j< 3; j++)
			{
				if(tri1->e[i] == tri2->e[j])
					return tri1->e[i];
			}
		}
	}
	if(flag == 0)  //  顶点
	{
	//  At first, if the id1,id2 are valid
		if(!(id1 >=0 && id1<vertices.getSize() && id2 >=0 && id2<vertices.getSize()))
		{
			OUTPUTINFO("Error!");
			return -1;
		}
		Point *pt1, *pt2;
		pt1 = &vertices.index(id1);
		pt2 = &vertices.index(id2);
		for(int i = 0; i< pt1->eNum; i++)
		{
			int eid = pt1->eids.index(i);
			for(int j = 0; j< pt2->eNum; j++)
			{
				if(eid == pt2->eids.index(j))
					return eid;
			}
		}
	}

	return -1;
}

bool Surface::transform3Dto2D(int triID, int eID, double tv1, double tv2, int vtID, Point2D *vt)
{
	double r1,r2,v2x;
	double r12,r22,v2x2;
	double A,B;

	if(eID >= edges.getSize() || eID < 0)
		return false;

	MESH_EDGE *e = &edges.index(eID);

	if(triID >= 0)  // 将三角形的第三个顶点转化成二维坐标的点
	{
		if(triID >= triangles.getSize())
			return false;
		Triangle *tri = &triangles.index(triID);
		
		int v1id = e->v[0];
		int v2id = e->v[1];

//		for(int i = 0; i<3; i++)
//		{
//			if(tri->v[i] != v1id && tri->v[i] != v2id)
//			{
//				vtID = tri->v[i];
//				break;
//			}
//		}

		int e1id, e2id;
		for(int i = 0; i<3; i++)
		{
			int eid = tri->e[i];

			if(eid != eID )
			{
				MESH_EDGE *tempE = &edges.index(eid);
				if(tempE->v[0] == v1id || tempE->v[1] == v1id)
					e1id = eid;
				if(tempE->v[0] == v2id || tempE->v[1] == v2id)
					e2id = eid;
			}
		}

		r1 = edges.index(e1id).length;	r2 = edges.index(e2id).length;
		v2x = e->length;		
	}
	else  // 计算虚拟顶点（根据一条边的两个顶点的权值）
	{
		r1 = tv1;	r2 = tv2;
		v2x = e->length;

		if((r1+r2)<v2x)
			return false;
	}

	r12 = r1*r1; r22 = r2*r2; v2x2 = v2x*v2x;

	A = 2*r12*v2x2-v2x2*v2x2+2*r22*v2x2;
	B = (r12-r22)*(r12-r22);

	vt->x = 0.5*(v2x2+r12-r22)/v2x;
	vt->y = 0.5*sqrt(A-B)/v2x;	

	return true;
}

bool Surface::setupExtraProperty(int flag)
{
	//　flag = 1 (vertex); 2 (edge) ; 3 (vertex and edge)
	int i;
	// 初始化
	int vertNum = vertices.getSize();
	int edgeNum = edges.getSize();

	if(e_ex_properties.getSize()>0)
	{
		e_ex_properties.free();
	}
	e_ex_properties.setSize(0);

	for(i = 0; i<v_ex_properties.getSize(); i++)
	{
		VERTEX_EXTRA_PROPERTY *v_pt = &v_ex_properties.index(i);
		if(v_pt->f_e3_ids.getSize()>0) 
		{
			v_pt->f_e3_ids.free(); 
			v_pt->f_v_distance.free();
			v_pt->f_v_id.free();
		}
	}
	v_ex_properties.setSize(0);

	v_ex_properties.resize(vertNum);
	e_ex_properties.resize(edgeNum);

	//　vertex
	if(flag == 1 || flag == 3)
	for(i = 0; i<vertNum; i++ )
	{
		Point *v = &vertices.index(i);

		VERTEX_EXTRA_PROPERTY *v_pt = &v_ex_properties.index(i);
		v_pt->f_e3_ids.resize(v->fNum);
		v_pt->f_v_distance.resize(v->fNum);
		v_pt->f_v_id.resize(v->fNum);

		for(int j = 0; j< v->fNum; j++)
		{
			int fid = v->fids.index(j);
			Triangle *tri = &triangles.index(fid);

			v_pt->f_v_distance.index(j) = -1.0;
			v_pt->f_v_id.index(j) = -1;
			
			for(int k = 0; k < 3; k++)
			{
				int eid = tri->e[k];
				MESH_EDGE *edge = &edges.index(eid);
				if(edge->v[0] != i && edge->v[1] != i)
				{
					v_pt->f_e3_ids.index(j) = eid;
					break;
				}
			}
		}
	}
	// edge
	if(flag == 2 || flag == 3)
	for(i = 0; i< edgeNum; i++)
	{
		MESH_EDGE *edge = &edges.index(i);
		EDGE_EXTRA_PROPERTY *e_pt = &e_ex_properties.index(i);
		
		for(int j = 0; j< 2; j++)
		{
			int fid = edge->f[j];
			e_pt->f_v3_id[j] = -1;
			e_pt->f_v_eid[j][0] = -1; e_pt->f_v_eid[j][1] = -1;

			if(fid < 0)
				continue;
			Triangle *tri = &triangles.index(fid);
			//　for f_v3_id
			for(int k = 0; k< 3; k++)
			{
				int vid = tri->v[k];

				if(vid != edge->v[0] && vid != edge->v[1])
				{
					e_pt->f_v3_id[j] = vid;
					break;
				}
			}

			//　for f_v_eid
			double tv1,tv2;
			for(k = 0; k<3; k++)
			{
				int eid = tri->e[k];
				MESH_EDGE *e = &edges.index(eid);
				if(i != eid)
				{
					if(e->v[0] == edge->v[0] || e->v[1] == edge->v[0])
					{
						e_pt->f_v_eid[j][0] = eid;
						tv1 = e->length;
					}
					if(e->v[0] == edge->v[1] || e->v[1] == edge->v[1])
					{
						e_pt->f_v_eid[j][1] = eid;
						tv2 = e->length;
					}

				}
			}

			// for f_v3_2D
			if(!transform3Dto2D(-1,i, tv1,tv2, 0, &(e_pt->f_v3_2D[j])))
				return false;
		}
	}
	
	// vertex
	if(flag == 1 || flag == 3)
	for(i = 0; i<vertNum; i++ )
	{
		Point *v = &vertices.index(i);

		VERTEX_EXTRA_PROPERTY *v_pt = &v_ex_properties.index(i);
		
		for(int j = 0; j< v->fNum; j++)
		{
			int eid = v_pt->f_e3_ids.index(j);

			EDGE_EXTRA_PROPERTY *e_pt = &e_ex_properties.index(eid);
			if(e_pt->f_v3_id[0]>=0 && e_pt->f_v3_id[1]>=0 )
			{
				int v3_id;
				v3_id = (i == e_pt->f_v3_id[0] ? e_pt->f_v3_id[1] : e_pt->f_v3_id[0]);

//				VERTEX_EXTRA_PROPERTY *v3_pt;
//				v3_pt = &v_ex_properties.index(v3_id);
				Point2D temp_p;
				temp_p.assign(e_pt->f_v3_2D[1]);
				temp_p.y = -temp_p.y;
				double dist = e_pt->f_v3_2D[0].distance(temp_p);
				
				v_pt->f_v_distance.index(j) = dist;
				v_pt->f_v_id.index(j) = v3_id;
			}
		}
	}
	return true;
}
