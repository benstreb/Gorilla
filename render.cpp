#include "glCanvas.h"
#include "mesh.h"
#include "edge.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"
#include "camera.h"

Vec3f floor_color(0.9,0.8,0.7);
Vec3f mesh_color(1,1,1);
Vec3f mirror_color(0.1,0.1,0.2);
Vec3f mirror_tint(0.9,0.9,1.0);

const float PROJECTED_SHADOW_DISTANCE = 10000;
const int SHADOW_MAP_RATIO = 1.0;

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
  glGenBuffers(1,&light_vert_VBO);
  glGenBuffers(1,&floor_quad_verts_VBO);
  bbox.initializeVBOs();
}

void Mesh::cleanupVBOs() {
  glDeleteBuffers(1,&board_tri_verts_VBO);
  glDeleteBuffers(1,&piece_tri_verts_VBO);
  //glDeleteBuffers(1,&reflected_mesh_tri_verts_VBO);
  glDeleteBuffers(1,&light_vert_VBO);
  glDeleteBuffers(1,&floor_quad_verts_VBO);
  bbox.cleanupVBOs();
}

// ================================================================================
// ================================================================================

void Mesh::SetupLight(Vec3f light_position) {
  light_vert.push_back(VBOPos(light_position));
  //light_vert.push_back(VBOPos(planeReflect(light_position, getMirrorCenter())));
  glBindBuffer(GL_ARRAY_BUFFER,light_vert_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPos)*2,&light_vert[0],GL_STATIC_DRAW); 
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

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(0));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(VBOPosNormal), BUFFER_OFFSET(12));
  glDrawArrays(GL_TRIANGLES,0,numTriangles(triSet)*3);
  
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

}


//TODO: REALLY, REALLY Un-demo-ify this.
GoBoard *demoBoard;
bool initialized = false;
GoBoard &makeDemoBoard() {
  if (!initialized) {
    initialized = true;
    demoBoard = new GoBoard();
    for (int x = 0; x < 9; x++) {
      for (int y = 0; y < 9; y++) {
        demoBoard->placePiece(x, y, (x+y)%3-1);
      }
    }
  }
  return *demoBoard;
}


//TODO: Un-demo-ify this.
void Mesh::DrawPieces(const GoBoard &board) {
  glTranslatef(PIECE_OFFSET, BOARD_HEIGHT, PIECE_OFFSET);
  glScalef(PIECE_X_SCALE, PIECE_Y_SCALE, PIECE_Z_SCALE);
  for (int x = 0; x < 9; x++) {
    for (int y = 0; y < 9; y++) {
      char space = board.getPiece(x, y);
      if (space != 0) {
        float color = (space + 1) / 2 + 0.2;
        glColor3f(color, color, color);
        DrawMesh(piece, piece_tri_verts_VBO);
      }
      glTranslatef(0, 0, BOARD_GRID_SPACING);
    }
    glTranslatef(0, 0, -BOARD_MAX/PIECE_Z_SCALE);
    glTranslatef(BOARD_GRID_SPACING, 0, 0);
  }
  glTranslatef(-BOARD_MAX, 0, 0);
  glScalef(1/PIECE_X_SCALE, 1/PIECE_Y_SCALE, 1/PIECE_Z_SCALE);
  glTranslatef(-PIECE_OFFSET, -BOARD_HEIGHT, -PIECE_OFFSET);
  
  /*
  startTranslate(0.005f, 1, 0.005f);
	glColor3f(.7, .7, 7.);
	glScalef(0.1f, 0.03f, 0.1f);
	DrawMesh(piece, piece_tri_verts_VBO);
  endTranslate();
  */
 
}

// ======================================================================================
// ======================================================================================

void Mesh::setupVBOs() {
  // delete all the old geometry
  board_tri_verts.clear();
  piece_tri_verts.clear();
  light_vert.clear();
  floor_quad_verts.clear();
  cleanupVBOs();
  // setup the new geometry
  Vec3f light_position = LightPosition();
  //SetupLight(light_position);
  SetupFloor();
  SetupMesh(board, board_tri_verts_VBO, board_tri_verts);
  SetupMesh(piece, piece_tri_verts_VBO, piece_tri_verts);
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
  
  if (args->glsl_enabled) {
  
  
	  glEnable(GL_DEPTH_TEST);
	  glClearColor(0,0,0,1.0f);
	  glEnable(GL_CULL_FACE);
	  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	  
	  //BEGIN SHADOW FUN/////////////===================================
	  
	  //First step: Render from the light POV to a FBO, story depth values only
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);	//Rendering offscreen
		
		//Using the fixed pipeline to render to the depthbuffer
		glUseProgramObjectARB(0);
			
		
		// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
		glViewport(0,0,args->width * SHADOW_MAP_RATIO,args->height* SHADOW_MAP_RATIO);
		
		// Clear previous frame values
		glClear( GL_DEPTH_BUFFER_BIT);
		
		//Disable color rendering, we only want to write to the Z-Buffer
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
		
		
		
		//LIGHT POSITION==========================================================================================
		/*setupMatrices(float(light_position.x()),float(light_position.y()),float(light_position.z()),
						10,10,1);*/
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45,args->width/args->height,1.0f,1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt((float)light_position.x(),(float)light_position.y(),(float)light_position.z(),0,0,0,0,1,0);
						
		
		// Culling switching, rendering only backface, this is done to avoid self-shadowing
		glCullFace(GL_FRONT);
		 
		
		//===========Draw the Things===================================================================================
		InsertColor(floor_color);
		DrawFloor();
		startTranslate(0,1,0);
			glutSolidCube(1);
		endTranslate();
		/*
		InsertColor(mesh_color);
		glUseProgramObjectARB(GLCanvas::program);
		glColor3b(GLbyte(240-127), GLbyte(184-127), GLbyte(0-127));
		DrawMesh(board, board_tri_verts_VBO);
		DrawPieces(makeDemoBoard());
		*/
		glUseProgramObjectARB(0);
		
		//=============================================================================================================
		
		//Save modelview/projection matrice into texture7, also add a biais
		setTextureMatrix();
		
		
		// Now rendering from the camera POV, using the FBO to generate shadows
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		
		glViewport(0,0,args->width,args->height);
		
		//Enabling color write (previously disabled for light POV z-buffer rendering)
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
		
		// Clear previous frame values
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  
		//Using the shadow shader
		glUseProgramObjectARB(GLCanvas::program);

		glUniform1iARB(shadowMapUniform,7);
		glActiveTextureARB(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D,depthTextureId);
		
		Vec3f cam_position = camera->camera_position;
		Vec3f interest = camera->point_of_interest;
		
		//CAMERA MATRIX=======================================================================================================
		//setupMatrices(cam_pos[0],cam_pos[1],cam_pos[2],interest[0],interest[1],interest[2]);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45,args->width/args->height,1.0f,1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt((float)cam_position.x(),(float)cam_position.y(),(float)cam_position.z(),(float)interest.x(),(float)interest.y(),(float)interest.z(),0,1,0);
		
		glCullFace(GL_BACK);
		//============================Draw the things
		InsertColor(floor_color);
		DrawFloor();
		startTranslate(0,1,0);
			glutSolidCube(1);
		endTranslate();
		/*
		InsertColor(mesh_color);
		glUseProgramObjectARB(GLCanvas::program);
		glColor3b(GLbyte(240-127), GLbyte(184-127), GLbyte(0-127));
		DrawMesh(board, board_tri_verts_VBO);
		DrawPieces(makeDemoBoard());
		*/
		glUseProgramObjectARB(0);
		
		//============================All the things
		
		// DEBUG only. this piece of code draw the depth buffer onscreen
		//*
		 glUseProgramObjectARB(0);
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(-args->width/2,args->width/2,-args->height/2,args->height/2,1,20);
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glColor4f(1,1,1,1);
	 glActiveTextureARB(GL_TEXTURE0);
	 glBindTexture(GL_TEXTURE_2D,depthTextureId);
	 glEnable(GL_TEXTURE_2D);
	 glTranslated(0,0,-1);
	 glBegin(GL_QUADS);
	 glTexCoord2d(0,0);glVertex3f(0,0,0);
	 glTexCoord2d(1,0);glVertex3f(args->width/2,0,0);
	 glTexCoord2d(1,1);glVertex3f(args->width/2,args->width/2,0);
	 glTexCoord2d(0,1);glVertex3f(0,args->height/2,0);
		 
		 glEnd();
		 glDisable(GL_TEXTURE_2D);
		 //*/
		
		glutSwapBuffers();
	  
	  //END SHADOW FUN//////////////////////////////////////////////////
	}
	else
	{
		InsertColor(floor_color);
		DrawFloor();
		
		InsertColor(mesh_color);
		glColor3b(GLbyte(240-127), GLbyte(184-127), GLbyte(0-127));

		DrawMesh(board, board_tri_verts_VBO);
		DrawPieces(makeDemoBoard());
	}
  

  // -------------------------
  // ADDITIONAL VISUALIZATIONS (for debugging)
  glColor3f(1,1,0);
  //DrawLight();
  if (args->bounding_box) {
    glColor3f(0,0,0);
    bbox.drawVBOs();
  }
    
  HandleGLError(); 
}

// =================================================================

// Shadow map fun times

void Mesh::generateShadowFBO()
{
	int shadowMapWidth = args->width * SHADOW_MAP_RATIO;
	int shadowMapHeight = args->height * SHADOW_MAP_RATIO;
	
	//GLfloat borderColor[4] = {0,0,0,0};
	
	GLenum FBOstatus;
	
	// Try to use a texture depth component
	glGenTextures(1, &depthTextureId);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);
	
	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Remove artefact on the edges of the shadowmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	
	//glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
	
	
	
	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
	
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// attach the texture to FBO depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);
	
	// check FBO status
	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
	
	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Mesh::setupMatrices(float position_x,float position_y,float position_z,float lookAt_x,float lookAt_y,float lookAt_z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,args->width/args->height,1.0f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position_x,position_y,position_z,0,0,0,0,1,0);
}

void Mesh::setTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];
	
	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5 
	// y = y* 0.5 + 0.5 
	// z = z* 0.5 + 0.5 
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	
	glLoadIdentity();	
	glLoadMatrixd(bias);
	
	// concatating all matrice into one.
	glMultMatrixd (projection);
	glMultMatrixd (modelView);
	
	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
	
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
void Mesh::startTranslate(float x,float y,float z)
{
	glPushMatrix();
	glTranslatef(x,y,z);
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glTranslatef(x,y,z);
}

void Mesh::endTranslate()
{
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

