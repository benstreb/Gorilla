#ifndef MESH_H
#define MESH_H

#include "glCanvas.h"
#include <vector>
#include <string>
#include "vectors.h"
#include "hash.h"
#include "boundingbox.h"
#include "vbo_structs.h"

class ArgParser;
class Vertex;
class Edge;
class Triangle;
class Camera;

// ======================================================================
// ======================================================================

class Mesh {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Mesh(ArgParser *_args) { args = _args; }
  ~Mesh();
  int LoadBoard(std::string fileName, int vertexcount);
  int LoadPiece(std::string fileName, int vertexcount);
  int LoadRect(std::string fileName, int vertexcount);
  void ComputeGouraudNormals();

  void initializeVBOs(); 
  void setupVBOs(); 
  void drawVBOs();
  void cleanupVBOs();
    
  // ========
  // VERTICES
  int numVertices() const { return vertices.size(); }
  Vertex* addVertex(const Vec3f &pos);
  // look up vertex by index from original .obj file
  Vertex* getVertex(int i) const {
    assert (i >= 0 && i < numVertices());
    Vertex *v = vertices[i];
    assert (v != NULL);
    return v; }

  // =====
  // EDGES
  int numEdges(edgeshashtype &edgeSet) const { return edgeSet.size(); }
  int boardEdges() const { return board_edges.size(); }
  int pieceEdges() const { return piece_edges.size(); }
  // this efficiently looks for an edge with the given vertices, using a hash table
  Edge* getMeshEdge(edgeshashtype edgeSet, Vertex *a, Vertex *b) const;

  // =========
  // TRIANGLES
  int numTriangles(triangleshashtype &triSet) const { return triSet.size(); }
  void addTriangle(triangleshashtype &triSet, edgeshashtype &edgeSet, Vertex *a, Vertex *b, Vertex *c);
  void removeTriangle(edgeshashtype &edgeSet, Triangle *t);

  // ===============
  // OTHER ACCESSORS
  const BoundingBox& getBoundingBox() const { return bbox; }
  Vec3f LightPosition();
  
  //===Shadow map fun
  void generateShadowFBO();
  
  GLuint shadowMapUniform;
  Camera* camera;
  
  

private:
  int Load(triangleshashtype &triSet, edgeshashtype &edgeSet, std::string fileName, int vertexcount);

  //Vec3f getMirrorCenter();

  // HELPER FUNCTIONS FOR PAINT
  void SetupLight(Vec3f light_position);
  void SetupMirror();
  void SetupFloor();
  void SetupMesh(triangleshashtype &triSet, GLuint VBO, std::vector<VBOPosNormal> &VBO_verts_vector);
  //void SetupReflectedMesh();
  //void SetupReflectedFloor();
  void SetupSilhouetteEdges(edgeshashtype &edgeSet, Vec3f light_position);
  void SetupShadowPolygons(Vec3f light_position);

  void DrawLight();
  void DrawMirror();
  void DrawFloor();
  void DrawControlMap();
  void DrawMesh(triangleshashtype &triSet, GLuint VBO);
  
  //TODO: Un-demo-ify this.
  void DrawPieces(char *flat_board);
  //void DrawReflectedMesh();
  //void DrawReflectedFloor();
  void DrawSilhouetteEdges();
  void DrawShadowPolygons();
  
  //=======Shadow Map Fun
  void setupMatrices(float position_x,float position_y,float position_z,float lookAt_x,float lookAt_y,float lookAt_z);
  void setTextureMatrix(void);
  void startTranslate(float x,float y,float z);
  void endTranslate();

  // ==============
  // REPRESENTATION
  ArgParser *args;
  std::vector<Vertex*> vertices;
  edgeshashtype board_edges;
  edgeshashtype piece_edges;
  edgeshashtype control_map_edges;
  triangleshashtype board;
  triangleshashtype piece;
  triangleshashtype control_map;
  BoundingBox bbox;

  // VBOs
  GLuint board_tri_verts_VBO;
  GLuint piece_tri_verts_VBO;
  GLuint control_map_tri_verts_VBO;
  //GLuint reflected_mesh_tri_verts_VBO;
  GLuint shadow_polygon_quad_verts_VBO;
  GLuint light_vert_VBO;
  GLuint mirror_quad_verts_VBO;
  GLuint floor_quad_verts_VBO;
  GLuint reflected_floor_quad_verts_VBO;
  GLuint silhouette_edge_verts_VBO;
  
  GLuint depthTextureId;
  GLuint fboId;

  std::vector<VBOPosNormal> board_tri_verts;
  std::vector<VBOPosNormal> piece_tri_verts;
  std::vector<VBOPosNormal> control_map_tri_verts;
  //std::vector<VBOPosNormal> reflected_mesh_tri_verts; 
  std::vector<VBOPos> shadow_polygon_quad_verts;
  std::vector<VBOPos> light_vert;
  std::vector<VBOPosNormal> mirror_quad_verts;
  std::vector<VBOPosNormal> floor_quad_verts;
  std::vector<VBOPosNormal> reflected_floor_quad_verts;
  std::vector<VBOPos> silhouette_edge_verts;
};

Vec3f ComputeNormal(const Vec3f &p1, const Vec3f &p2, const Vec3f &p3);

// ======================================================================
// ======================================================================

#endif




