#include "mesh.h"
#include "edge.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"
#include "camera.h"

int Triangle::next_triangle_id = 0;

// =======================================================================
// MESH DESTRUCTOR 
// =======================================================================

Mesh::~Mesh() {
  // delete all the triangles
  std::vector<Triangle*> todo;
  for (auto iter = table.begin(); iter != table.end(); iter++) {
    Triangle *t = iter->second;
    todo.push_back(t);
  }
  int num_triangles = todo.size();
  for (int i = 0; i < num_triangles; i++) {
    removeTriangle(board_edges, todo[i]);
  }
  todo = std::vector<Triangle*>();
  for (auto iter = piece.begin(); iter != piece.end(); iter++) {
    Triangle *t = iter->second;
    todo.push_back(t);
  }
  num_triangles = todo.size();
  for (int i = 0; i < num_triangles; i++) {
    removeTriangle(piece_edges, todo[i]);
  }
  // delete all the vertices
  int num_vertices = numVertices();
  for (int i = 0; i < num_vertices; i++) {
    delete vertices[i];
  }
  cleanupVBOs();
}

// =======================================================================
// MODIFIERS:   ADD & REMOVE
// =======================================================================

Vertex* Mesh::addVertex(const Vec3f &position) {
  int index = numVertices();
  Vertex *v = new Vertex(index, position);
  vertices.push_back(v);
  if (numVertices() == 1)
    bbox = BoundingBox(position,position);
  else 
    bbox.Extend(position);
  return v;
}


void Mesh::addTriangle(triangleshashtype &triSet, edgeshashtype &edgeSet, Vertex *a, Vertex *b, Vertex *c) {
  // create the triangle
  Triangle *t = new Triangle();
  // create the edges
  Edge *ea = new Edge(a,b,t);
  Edge *eb = new Edge(b,c,t);
  Edge *ec = new Edge(c,a,t);
  // point the triangle to one of its edges
  t->setEdge(ea);
  // connect the edges to each other
  ea->setNext(eb);
  eb->setNext(ec);
  ec->setNext(ea);
  // verify these edges aren't already in the mesh 
  // (which would be a bug, or a non-manifold mesh)
  assert (edgeSet.find(std::make_pair(a,b)) == edgeSet.end());
  assert (edgeSet.find(std::make_pair(b,c)) == edgeSet.end());
  assert (edgeSet.find(std::make_pair(c,a)) == edgeSet.end());
  // add the edges to the master list
  edgeSet[std::make_pair(a,b)] = ea;
  edgeSet[std::make_pair(b,c)] = eb;
  edgeSet[std::make_pair(c,a)] = ec;
  // connect up with opposite edges (if they exist)
  edgeshashtype::iterator ea_op = edgeSet.find(std::make_pair(b,a)); 
  edgeshashtype::iterator eb_op = edgeSet.find(std::make_pair(c,b)); 
  edgeshashtype::iterator ec_op = edgeSet.find(std::make_pair(a,c)); 
  if (ea_op != edgeSet.end()) { ea_op->second->setOpposite(ea); }
  if (eb_op != edgeSet.end()) { eb_op->second->setOpposite(eb); }
  if (ec_op != edgeSet.end()) { ec_op->second->setOpposite(ec); }
  // add the triangle to the master list
  assert(triSet.find(t->getID()) == triSet.end());
  triSet[t->getID()] = t;
}


void Mesh::removeTriangle(edgeshashtype &edgeSet, Triangle *t) {
  Edge *ea = t->getEdge();
  Edge *eb = ea->getNext();
  Edge *ec = eb->getNext();
  Vertex *a = ea->getStartVertex();
  Vertex *b = eb->getStartVertex();
  Vertex *c = ec->getStartVertex();
  // remove these elements from master lists
  edgeSet.erase(std::make_pair(a,b)); 
  edgeSet.erase(std::make_pair(b,c)); 
  edgeSet.erase(std::make_pair(c,a)); 
  table.erase(t->getID());
  piece.erase(t->getID());
  // clean up memory
  delete ea;
  delete eb;
  delete ec;
  delete t;
}


// Helper function for accessing data in the hash table
Edge* Mesh::getMeshEdge(edgeshashtype edgeSet, Vertex *a, Vertex *b) const {
  edgeshashtype::const_iterator iter = edgeSet.find(std::make_pair(a,b));
  if (iter == edgeSet.end()) return NULL;
  return iter->second;
}

// =======================================================================
// the load function parses very simple .obj files
// =======================================================================

int Mesh::LoadBoard(std::string input_file, int prior_vert_count) {
  return Mesh::Load(table, board_edges, input_file, prior_vert_count);
}
int Mesh::LoadPiece(std::string input_file, int prior_vert_count) {
  return Mesh::Load(piece, piece_edges, input_file, prior_vert_count);
}
int Mesh::LoadRect(std::string input_file, int prior_vert_count) {
  return Mesh::Load(control_map, control_map_edges, input_file, prior_vert_count);
}

int Mesh::Load(triangleshashtype &triSet, edgeshashtype &edgeSet, std::string input_file, int prior_vert_count) {
  std::cout<<prior_vert_count<<", "<<numVertices()<<"\n";
  //std::string input_file = args->input_file;
  
  FILE *objfile = fopen(input_file.c_str(),"r");
  if (objfile == NULL) {
    std::cout << "ERROR! CANNOT OPEN '" << input_file << "'\n";
    return prior_vert_count;
  }

  char line[200] = "";
  char token[100] = "";
  char atoken[100] = "";
  char btoken[100] = "";
  char ctoken[100] = "";
  float x,y,z;
  int a,b,c,d,e;
  
  int index = 0;
  int vert_count = 0;
  int vert_index = 1;
  
  while (fgets(line, 200, objfile)) {
    int token_count = sscanf (line, "%s\n",token);
    if (token_count == -1) continue;
    a = b = c = d = e = -1;
    if (!strcmp(token,"usemtl") ||
	!strcmp(token,"g")) {
      vert_index = 1; 
      index++;
    } else if (!strcmp(token,"v")) {
      vert_count++;
      sscanf (line, "%s %f %f %f\n",token,&x,&y,&z);
      addVertex(Vec3f(x,y,z));
    } else if (!strcmp(token,"f")) {
      int num = sscanf (line, "%s %s %s %s\n",token,
			atoken,btoken,ctoken);
      sscanf (atoken,"%d",&a);
      sscanf (btoken,"%d",&b);
      sscanf (ctoken,"%d",&c);
      assert (num == 4);
      a += prior_vert_count;
      b += prior_vert_count;
      c += prior_vert_count;
      a -= vert_index;
      b -= vert_index;
      c -= vert_index;
      assert (a >= 0 && a < numVertices());
      assert (b >= 0 && b < numVertices());
      assert (c >= 0 && c < numVertices());
      addTriangle(triSet, edgeSet, getVertex(a),getVertex(b),getVertex(c));
    } else if (!strcmp(token,"vt")) {
    } else if (!strcmp(token,"vn")) {
    } else if (token[0] == '#') {
    } else {
      printf ("LINE: '%s'",line);
    }
  }
  fclose(objfile);
  return vert_count;
}

// =======================================================================

// calculate the normal of a triangle
Vec3f ComputeNormal(const Vec3f &p1, const Vec3f &p2, const Vec3f &p3) {
  Vec3f v12 = p2;
  v12 -= p1;
  Vec3f v23 = p3;
  v23 -= p2;
  Vec3f normal;
  Vec3f::Cross3(normal,v12,v23);
  normal.Normalize();
  return normal;
}

void doGouraud(Triangle *t) {
  Vec3f n = ComputeNormal((*t)[0]->getPos(),
			  (*t)[1]->getPos(),
			  (*t)[2]->getPos());
  (*t)[0]->incrGouraudNormal(n);
  (*t)[1]->incrGouraudNormal(n);
  (*t)[2]->incrGouraudNormal(n);
}

// compute the gouraud normals of all vertices of the mesh and store at each vertex
void Mesh::ComputeGouraudNormals() {
  int i;
  // clear the normals
  for (i = 0; i < numVertices(); i++) {
    getVertex(i)->clearGouraudNormal();
  }
  // loop through all the triangles incrementing the normal at each vertex
  for (auto iter = table.begin(); iter != table.end(); iter++) {
    Triangle *t = iter->second;
    doGouraud(t);
  }
  for (auto iter = piece.begin(); iter != piece.end(); iter++) {
    Triangle *t = iter->second;
    doGouraud(t);
  }
  // finally, normalize the sum at each vertex
  for (i = 0; i < numVertices(); i++) {
    getVertex(i)->normalizeGouraudNormal();
  }
}

// =================================================================
