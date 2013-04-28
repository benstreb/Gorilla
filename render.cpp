#include "glCanvas.h"
#include "mesh.h"
#include "edge.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"

Vec3f floor_color(0.9,0.8,0.7);
Vec3f mesh_color(1,1,1);
Vec3f mirror_color(0.1,0.1,0.2);
Vec3f mirror_tint(0.9,0.9,1.0);

const float PROJECTED_SHADOW_DISTANCE = 10000;

void extendVolume(int n, std::vector<Vertex*> v, int localLight, Vec3f lightPosition, float extendDistance)
{
  Vec3f extendedVertex;
  Vec3f extendDirection;
  int i;
  // If light source is infinite (directional)...
  if (!localLight) {
  // compute direction opposite from light source direction.
    extendDirection.setx(-lightPosition[0]);
    extendDirection.sety(-lightPosition[1]);
    extendDirection.setz(-lightPosition[2]);
  }
  glBegin(GL_QUAD_STRIP);
  // If light source is local (positional)...
  if (localLight) {
    for (i=0; i<n; i++) {
      Vec3f vec = v[i]->getPos();
      double vert[3] = {vec[0], vec[1], vec[2]};	
      GLfloat vert_gl[3] = {(GLfloat)vert[0], (GLfloat)vert[1], (GLfloat)vert[2]};
      glVertex3fv(vert_gl);


      // Compute per-vertex direction from vertex away from the light source.
      extendDirection.setx(vec[0] - lightPosition[0]);
      extendDirection.sety(vec[1] - lightPosition[1]);
      extendDirection.setz(vec[2] - lightPosition[2]);
      // Compute extended vertex.
      extendedVertex.setx(vec[0] + extendDirection[0] * extendDistance);
      extendedVertex.sety(vec[1] + extendDirection[1] * extendDistance);
      extendedVertex.setz(vec[2] + extendDirection[2] * extendDistance);
  
      double ext_vert[3] = {extendedVertex[0], extendedVertex[1], extendedVertex[2]};	
      GLfloat ext_vert_gl[3] = {(GLfloat)ext_vert[0], (GLfloat)ext_vert[1], (GLfloat)ext_vert[2]};
      glVertex3fv(ext_vert_gl);
    }
    // Repeat initial 2 vertices to close the quad strip loop.
    Vec3f vec = v[0]->getPos();
    double vert[3] = {vec[0], vec[1], vec[2]};
    GLfloat vert_gl[3] = {(GLfloat)vert[0], (GLfloat)vert[1], (GLfloat)vert[2]};
    glVertex3fv(vert_gl);
    extendDirection.setx(vec[0] - lightPosition[0]);
    extendDirection.sety(vec[1] - lightPosition[1]);
    extendDirection.setz(vec[2] - lightPosition[2]);
    extendedVertex.setx(vec[0] + extendDirection[0] * extendDistance);
    extendedVertex.sety(vec[1] + extendDirection[1] * extendDistance);
    extendedVertex.setz(vec[2] + extendDirection[2] * extendDistance);
    double ext_vert[3] = {extendedVertex[0], extendedVertex[1], extendedVertex[2]};	
    GLfloat ext_vert_gl[3] = {(GLfloat)ext_vert[0], (GLfloat)ext_vert[1], (GLfloat)ext_vert[2]};
    glVertex3fv(ext_vert_gl);
    // otherwise, light source is infinite (directional)...
  }
  glEnd();
}

// =======================================================================
// =======================================================================

void InsertColor(const Vec3f &v) {
  glColor3f(v.x(),v.y(),v.z());
}

// the light position can be animated
Vec3f Mesh::LightPosition() {
  Vec3f min = bbox.getMin();
  Vec3f max = bbox.getMax();
  Vec3f tmp;
  bbox.getCenter(tmp);
  tmp += Vec3f(0,1.5*(max.y()-min.y()),0);
  tmp += Vec3f(cos(args->timer) * (max.x()-min.x()), 0, 0);
  tmp += Vec3f(0,0,sin(args->timer) * (max.z()-min.z()));
  return tmp;
}

void Mesh::initializeVBOs() {
  glGenBuffers(1,&board_tri_verts_VBO);
  glGenBuffers(1,&piece_tri_verts_VBO);
  //glGenBuffers(1,&reflected_mesh_tri_verts_VBO);
  glGenBuffers(1,&shadow_polygon_quad_verts_VBO);
  glGenBuffers(1,&light_vert_VBO);
  glGenBuffers(1,&mirror_quad_verts_VBO);
  glGenBuffers(1,&floor_quad_verts_VBO);
  glGenBuffers(1,&reflected_floor_quad_verts_VBO);
  glGenBuffers(1,&silhouette_edge_verts_VBO);
  bbox.initializeVBOs();
}

void Mesh::cleanupVBOs() {
  glDeleteBuffers(1,&board_tri_verts_VBO);
  glDeleteBuffers(1,&piece_tri_verts_VBO);
  //glDeleteBuffers(1,&reflected_mesh_tri_verts_VBO);
  glDeleteBuffers(1,&shadow_polygon_quad_verts_VBO);
  glDeleteBuffers(1,&light_vert_VBO);
  glDeleteBuffers(1,&mirror_quad_verts_VBO);
  glDeleteBuffers(1,&floor_quad_verts_VBO);
  glDeleteBuffers(1,&reflected_floor_quad_verts_VBO);
  glDeleteBuffers(1,&silhouette_edge_verts_VBO);
  bbox.cleanupVBOs();
}

// ================================================================================
// ================================================================================

/*Vec3f Mesh::getMirrorCenter() {
  Vec3f diff=bbox.getMax() - bbox.getMin();
  return bbox.getMin() + Vec3f(-0.25*diff.x(), 0.575*diff.y(), 0.5*diff.z());
}

Vec3f planeReflect(Vec3f pos, Vec3f refPoint) {
  pos.setx(2*refPoint.x() - pos.x());
  pos.setz(2*refPoint.z() - pos.z());
  return pos;
}
*/

void Mesh::SetupLight(Vec3f light_position) {
  light_vert.push_back(VBOPos(light_position));
  //light_vert.push_back(VBOPos(planeReflect(light_position, getMirrorCenter())));
  glBindBuffer(GL_ARRAY_BUFFER,light_vert_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPos)*2,&light_vert[0],GL_STATIC_DRAW); 
}

void Mesh::SetupMirror() {
  Vec3f diff = bbox.getMax()-bbox.getMin();
  // create frame vertices just a bit bigger than the bounding box
  Vec3f a = bbox.getMin() + Vec3f(-0.25*diff.x(), 0.1*diff.y(),-0.25*diff.z());
  Vec3f b = bbox.getMin() + Vec3f(-0.25*diff.x(), 0.1*diff.y(), 1.25*diff.z());
  Vec3f c = bbox.getMin() + Vec3f(-0.25*diff.x(), 1.25*diff.y(),-0.25*diff.z());
  Vec3f d = bbox.getMin() + Vec3f(-0.25*diff.x(), 1.25*diff.y(), 1.25*diff.z());
  Vec3f normal = ComputeNormal(a,c,b);
  mirror_quad_verts.push_back(VBOPosNormal(a,normal));
  mirror_quad_verts.push_back(VBOPosNormal(c,normal));
  mirror_quad_verts.push_back(VBOPosNormal(d,normal));
  mirror_quad_verts.push_back(VBOPosNormal(b,normal));
  glBindBuffer(GL_ARRAY_BUFFER,mirror_quad_verts_VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormal)*4,&mirror_quad_verts[0],GL_STATIC_DRAW);
}

void Mesh::SetupFloor() {
  Vec3f diff = bbox.getMax()-bbox.getMin();
  // create vertices just a bit bigger than the bounding box
  Vec3f a = bbox.getMin() + Vec3f(-0.75*diff.x(),0,-0.75*diff.z());
  Vec3f b = bbox.getMin() + Vec3f( 1.75*diff.x(),0,-0.75*diff.z());
  Vec3f c = bbox.getMin() + Vec3f(-0.75*diff.x(),0, 1.75*diff.z());
  Vec3f d = bbox.getMin() + Vec3f( 1.75*diff.x(),0, 1.75*diff.z());
  Vec3f normal = ComputeNormal(a,c,d);
  floor_quad_verts.push_back(VBOPosNormal(a,normal));
  floor_quad_verts.push_back(VBOPosNormal(c,normal));
  floor_quad_verts.push_back(VBOPosNormal(d,normal));
  floor_quad_verts.push_back(VBOPosNormal(b,normal));
  glBindBuffer(GL_ARRAY_BUFFER,floor_quad_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormal)*4,&floor_quad_verts[0],GL_STATIC_DRAW); 
}

void Mesh::SetupMesh(triangleshashtype &triSet, GLuint VBO, std::vector<VBOPosNormal> &VBO_verts_vector) {
  for (auto iter = triSet.begin(); iter != triSet.end(); iter++) {
    Triangle *t = iter->second;
    Vec3f a = (*t)[0]->getPos();
    Vec3f b = (*t)[1]->getPos();
    Vec3f c = (*t)[2]->getPos();
    Vec3f na = ComputeNormal(a,b,c);
    Vec3f nb = na;
    Vec3f nc = na;
    if (args->gouraud_normals) {
      na = (*t)[0]->getGouraudNormal();
      nb = (*t)[1]->getGouraudNormal();
      nc = (*t)[2]->getGouraudNormal();
    }
    VBO_verts_vector.push_back(VBOPosNormal(a,na));
    VBO_verts_vector.push_back(VBOPosNormal(b,nb));
    VBO_verts_vector.push_back(VBOPosNormal(c,nc));
  }
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
         sizeof(VBOPosNormal) * numTriangles(triSet) * 3,
         &VBO_verts_vector[0],
         GL_STATIC_DRAW);
}

/*
Vec3f reflect(Vec3f point, Vec3f mirror_center) {
  float distance = mirror_center.x() - point.x();
  point.setx(mirror_center.x() + distance);
  return point;
}

// draw a second copy of the object where it appears to be on the other side of the mirror
void Mesh::SetupReflectedMesh() {
  for (triangleshashtype::iterator iter = triangles.begin();
       iter != triangles.end(); iter++) {
    Triangle *t = iter->second;
    Vec3f a = reflect((*t)[0]->getPos(), getMirrorCenter());
    Vec3f b = reflect((*t)[1]->getPos(), getMirrorCenter());
    Vec3f c = reflect((*t)[2]->getPos(), getMirrorCenter());
    Vec3f na = -ComputeNormal(a,b,c);
    Vec3f nb = na;
    Vec3f nc = na;
    if (args->gouraud_normals) {
      na = (*t)[0]->getGouraudNormal()*Vec3f(-1, 1, 1);
      nb = (*t)[1]->getGouraudNormal()*Vec3f(-1, 1, 1);
      nc = (*t)[2]->getGouraudNormal()*Vec3f(-1, 1, 1);
    }
    reflected_mesh_tri_verts.push_back(VBOPosNormal(c,nc));
    reflected_mesh_tri_verts.push_back(VBOPosNormal(b,nb));
    reflected_mesh_tri_verts.push_back(VBOPosNormal(a,na));
  }
  glBindBuffer(GL_ARRAY_BUFFER,reflected_mesh_tri_verts_VBO);
  glBufferData(GL_ARRAY_BUFFER,
         sizeof(VBOPosNormal) * numTriangles() * 3,
         &reflected_mesh_tri_verts[0],
         GL_STATIC_DRAW);
}


// draw a second copy of the floor where it appears to be on the other side of the mirror
void Mesh::SetupReflectedFloor() {
  float minX = bbox.getMin().x();
  Vec3f diff = bbox.getMax()-bbox.getMin();
  // create vertices just a bit bigger than the bounding box
  Vec3f a = reflect(bbox.getMin() + Vec3f(-0.75*diff.x(),0,-0.75*diff.z()), getMirrorCenter());
  Vec3f b = reflect(bbox.getMin() + Vec3f( 1.75*diff.x(),0,-0.75*diff.z()), getMirrorCenter());
  Vec3f c = reflect(bbox.getMin() + Vec3f(-0.75*diff.x(),0, 1.75*diff.z()), getMirrorCenter());
  Vec3f d = reflect(bbox.getMin() + Vec3f( 1.75*diff.x(),0, 1.75*diff.z()), getMirrorCenter());
  Vec3f normal = ComputeNormal(a,c,d);
  reflected_floor_quad_verts.push_back(VBOPosNormal(c,normal));
  reflected_floor_quad_verts.push_back(VBOPosNormal(a,normal));
  reflected_floor_quad_verts.push_back(VBOPosNormal(b,normal));
  reflected_floor_quad_verts.push_back(VBOPosNormal(d,normal));
  glBindBuffer(GL_ARRAY_BUFFER,reflected_floor_quad_verts_VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormal)*4,&reflected_floor_quad_verts[0],GL_STATIC_DRAW);
}
*/


// figure out which edges are the silhouette of the object 
void Mesh::SetupSilhouetteEdges(edgeshashtype &edgeSet, Vec3f light_position) {

  silhouette_edge_verts.clear();
  
  int edge_count = 0;
  
  edgeshashtype::iterator itr;
  for(itr = edgeSet.begin(); itr != edgeSet.end(); ++itr)
  {
    Edge* edge_1 = itr->second;
    Edge* edge_2 = (itr->second)->getOpposite();
    
    Triangle* tri_1 = edge_1->getTriangle();
    Triangle* tri_2 = edge_2->getTriangle();
    
    const Vec3f tri_1_loc = ((*tri_1)[0]->getPos() + (*tri_1)[1]->getPos() + (*tri_1)[2]->getPos())*(1.0/3.0);
    const Vec3f tri_2_loc = ((*tri_2)[0]->getPos() + (*tri_2)[1]->getPos() + (*tri_2)[2]->getPos())*(1.0/3.0);
    
    const Vec3f light_to_tri_1 = tri_1_loc - light_position;
    const Vec3f light_to_tri_2 = tri_2_loc - light_position;
    
    const Vec3f norm_1 = ComputeNormal((*tri_1)[0]->getPos(), (*tri_1)[1]->getPos(), (*tri_1)[2]->getPos());
    const Vec3f norm_2 = ComputeNormal((*tri_2)[0]->getPos(), (*tri_2)[1]->getPos(), (*tri_2)[2]->getPos()); 
      
    const float  norm_1_and_light = norm_1.Dot3(light_to_tri_1);
    const float  norm_2_and_light = norm_2.Dot3(light_to_tri_2);
    
    if(norm_1_and_light * norm_2_and_light < 0)
    {
      VBOPos p1 = edge_1->getStartVertex()->getPos();
      VBOPos p2 = edge_1->getEndVertex()->getPos();
      silhouette_edge_verts.push_back(p1);
      silhouette_edge_verts.push_back(p2);
      edge_count += 1;
    }
    
  }
  
  glBindBuffer(GL_ARRAY_BUFFER,silhouette_edge_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,
         sizeof(VBOPos) * numEdges(edgeSet),
         &silhouette_edge_verts[0],
         GL_STATIC_DRAW); 

}


// project the silhouette edges away from the light source
void Mesh::SetupShadowPolygons(Vec3f light_position) {

  // ASSIGNMENT: WRITE THIS FUNCTION
  assert(silhouette_edge_verts.size()%2 == 0);
  int shadow_poly_count = 0;
  
  for(unsigned int i = 0; i < silhouette_edge_verts.size()-1; i+=2)
  {
    const Vec3f vert_1 = Vec3f(silhouette_edge_verts[i].x, silhouette_edge_verts[i].y,silhouette_edge_verts[i].z);
    const Vec3f vert_2 = Vec3f(silhouette_edge_verts[i+1].x, silhouette_edge_verts[i+1].y,silhouette_edge_verts[i+1].z);
    
    VBOPos p1 = vert_1;
    VBOPos p2 = vert_2;
    

      
    const Vec3f light_to_vert_1 = vert_1 - light_position;
    const Vec3f light_to_vert_2 = vert_2 - light_position;
    
    const Vec3f shadow_end_1 = light_position + light_to_vert_1*PROJECTED_SHADOW_DISTANCE;
    const Vec3f shadow_end_2 = light_position + light_to_vert_2*PROJECTED_SHADOW_DISTANCE;
    
    VBOPos p3 = shadow_end_1;
    VBOPos p4 = shadow_end_2;
    
    shadow_polygon_quad_verts.push_back(p3);
    shadow_polygon_quad_verts.push_back(p4);
    shadow_polygon_quad_verts.push_back(p1);
    shadow_polygon_quad_verts.push_back(p2);
    
    shadow_poly_count+=1;
  }
 
   glBindBuffer(GL_ARRAY_BUFFER,shadow_polygon_quad_verts_VBO); 
   glBufferData(GL_ARRAY_BUFFER,
         sizeof(VBOPos) * shadow_poly_count*4,
         &shadow_polygon_quad_verts[0],
         GL_STATIC_DRAW); 

}

// ================================================================================
// ================================================================================

void Mesh::DrawLight() {
  glPointSize(10);
  glDisable(GL_LIGHTING);
  glBindBuffer(GL_ARRAY_BUFFER, light_vert_VBO);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VBOPos), BUFFER_OFFSET(0));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_vert_VBO);
  glDrawArrays(GL_POINTS,0,1);
  glDisableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LIGHTING);
}

void Mesh::DrawMirror() {
  glBindBuffer(GL_ARRAY_BUFFER, mirror_quad_verts_VBO);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(0));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(12));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mirror_quad_verts_VBO);
  glDrawArrays(GL_QUADS,0,4);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::DrawFloor() {
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.1f,4.0f);
  glBindBuffer(GL_ARRAY_BUFFER, floor_quad_verts_VBO);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(0));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(12));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_quad_verts_VBO);
  glDrawArrays(GL_QUADS,0,4);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_POLYGON_OFFSET_FILL);
}

void Mesh::DrawMesh(triangleshashtype &triSet, GLuint VBO) {
  //assert ((int)mesh_tri_verts.size() == numTriangles()*3);
  if (args->silhouette_edges) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1f,4.0f);
  }
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(0));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(12));
  glDrawArrays(GL_TRIANGLES,0,numTriangles(triSet)*3);
  
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  if (args->silhouette_edges) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }
}

/*
void Mesh::DrawReflectedMesh() {
  if (reflected_mesh_tri_verts.size() > 0) {
    assert ((int)reflected_mesh_tri_verts.size() == numTriangles()*3);
    glBindBuffer(GL_ARRAY_BUFFER, reflected_mesh_tri_verts_VBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(0));
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(12));
    glDrawArrays(GL_TRIANGLES,0,numTriangles()*3);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
}

void Mesh::DrawReflectedFloor() {
  if (reflected_floor_quad_verts.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, reflected_floor_quad_verts_VBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(0));
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(12));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reflected_floor_quad_verts_VBO);
    glDrawArrays(GL_QUADS,0,4);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
}
*/

void Mesh::DrawSilhouetteEdges() {
  if (silhouette_edge_verts.size() > 0) {
    glLineWidth(4);
    glDisable(GL_LIGHTING);
    glBindBuffer(GL_ARRAY_BUFFER, silhouette_edge_verts_VBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(VBOPos), BUFFER_OFFSET(0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, silhouette_edge_verts_VBO);
    glDrawArrays(GL_LINES,0,silhouette_edge_verts.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_LIGHTING);
  }
}

void Mesh::DrawShadowPolygons() {
  if (shadow_polygon_quad_verts.size() > 0) {
    glDisable(GL_LIGHTING);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_polygon_quad_verts_VBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(VBOPos), BUFFER_OFFSET(0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadow_polygon_quad_verts_VBO);
    glDrawArrays(GL_QUADS,0,shadow_polygon_quad_verts.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_LIGHTING);
  }
}

//TODO: REALLY, REALLY Un-demo-ify this.
char *demoBoard;
bool initialized = false;
char *makeDemoBoard() {
  if (!initialized) {
    initialized = true;
    demoBoard = (char*) malloc(81);
    for (int x = 0; x < 9; x++) {
      for (int y = 0; y < 9; y++) {
        demoBoard[x*9+y] = (x+y)%3-1;
      }
    }
  }
  return demoBoard;
}


//TODO: Un-demo-ify this.
void Mesh::DrawPieces(char *flat_board) {
  glTranslatef(0.005f, 1, 0.005f);
  glScalef(0.1f, 0.03f, 0.1f);
  for (int x = 0; x < 9; x++) {
    for (int y = 0; y < 9; y++) {
      char space = flat_board[x*9+y];
      if (space != 0) {
        float color = (space + 1) / 2 + 0.2;
        glColor3f(color, color, color);
        DrawMesh(piece, piece_tri_verts_VBO);
      }
      glTranslatef(0, 0, 10.0f/9);
    }
    glTranslatef(0, 0, -10);
    glTranslatef(10.0f/9, 0, 0);
  }
  glTranslatef(-10, 0, 0);
  glScalef(10.0f, 1/0.03f, 10.0f);
  glTranslatef(-0.005f, -1, -0.005f);
}

// ======================================================================================
// ======================================================================================

void Mesh::setupVBOs() {
  // delete all the old geometry
  board_tri_verts.clear();
  piece_tri_verts.clear();
  //reflected_mesh_tri_verts.clear(); 
  shadow_polygon_quad_verts.clear();
  light_vert.clear();
  mirror_quad_verts.clear();
  floor_quad_verts.clear();
  reflected_floor_quad_verts.clear();
  silhouette_edge_verts.clear();
  cleanupVBOs();
  // setup the new geometry
  Vec3f light_position = LightPosition();
  SetupLight(light_position);
  SetupMirror();
  SetupFloor();
  SetupMesh(board, board_tri_verts_VBO, board_tri_verts);
  SetupMesh(piece, piece_tri_verts_VBO, piece_tri_verts);
  //SetupReflectedMesh();
  //SetupReflectedFloor();
  SetupSilhouetteEdges(board_edges, light_position);
  SetupSilhouetteEdges(piece_edges, light_position);
  SetupShadowPolygons(light_position);
  bbox.setupVBOs();
}

void Mesh::drawVBOs() {
  // scale it so it fits in the window
  Vec3f center; bbox.getCenter(center);
  float s = 1/bbox.maxDim();
  glScalef(s,s,s);
  glTranslatef(-center.x(),-center.y(),-center.z());

  // setup the light
  Vec3f light_position = LightPosition();
  GLfloat position[4] = { float(light_position.x()),float(light_position.y()),float(light_position.z()),1 };
  glLightfv(GL_LIGHT1, GL_POSITION, position);

  InsertColor(floor_color);
  DrawFloor();
  if (args->geometry) {
    InsertColor(mesh_color);
    if (args->glsl_enabled) {
        glUseProgramObjectARB(GLCanvas::program);
    }
    glColor3b(GLbyte(240-127), GLbyte(184-127), GLbyte(0-127));
    DrawMesh(board, board_tri_verts_VBO);
    DrawPieces(makeDemoBoard());
    if (args->glsl_enabled) {
      glUseProgramObjectARB(0);
    }
  }

  // -------------------------
  // ADDITIONAL VISUALIZATIONS (for debugging)
  glColor3f(1,1,0);
  DrawLight();
  /*if (args->reflected_geometry) {
    glColor3f(0,0,1);
    DrawReflectedMesh();
    DrawReflectedFloor();
  }*/
  if (args->bounding_box) {
    glColor3f(0,0,0);
    bbox.drawVBOs();
  }
  if (args->silhouette_edges) {
    glColor3f(1,0,0);
    DrawSilhouetteEdges();
  }
  if (args->shadow_polygons) {
    glDisable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glColor4f(0.0f,1.0f,0.5f,0.2f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    DrawShadowPolygons();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
  }
    
  HandleGLError(); 
}

// =================================================================
