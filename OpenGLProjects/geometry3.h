/**** Basic setup for defining and drawing objects ****/
// Moved to a header for the second and subsequent OpenGL programs

#ifndef __INCLUDEGEOMETRY
#define __INCLUDEGOEMETRY

const int numobjects = 2 ; // ** NEW ** number of objects for buffer 
const int numperobj  = 3 ;
const int ncolors = 4 ; 

GLuint buffers[numperobj*numobjects+ncolors+1] ; // ** NEW ** List of buffers for geometric data 
GLuint objects[numobjects] ; // ** NEW ** For each object
GLenum PrimType[numobjects] ;
GLsizei NumElems[numobjects] ; 

#define BUFFER_OFFSET(bytes) ((GLubyte *) NULL + (bytes))
#define NumberOf(array) (sizeof(array)/sizeof(array[0])) 

enum {Vertices, Colors, Elements} ; // For arrays for object 
enum {FLOOR} ; // For objects, for the floor

const GLfloat floorverts[4][3] = {
  {1.5, 0.0, 1.0}, {-1.5, 0.0, 1.0}, {-1.5, 0.0, -1.0}, {1.5, 0.0, -1.0}
} ; 

const GLfloat floorcol[4][3] = {
  {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}
} ; 

const GLubyte floorinds[1][4] = { {0, 1, 2, 3} } ; 

const GLfloat floortex[4][2] = { 
  {2.0, 2.0}, {0.0, 2.0}, {0.0, 0.0}, {2.0, 0.0}
} ;

void initobject(GLuint object, GLfloat * vert, GLint sizevert, GLfloat * col, GLint sizecol, GLubyte * inds, GLint sizeind, GLenum type) {
   int offset = object * numperobj ; 

   glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices+offset]) ; 
   glBufferData(GL_ARRAY_BUFFER, sizevert, vert,GL_STATIC_DRAW);
   glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_VERTEX_ARRAY) ; 
   glBindBuffer(GL_ARRAY_BUFFER, buffers[Colors+offset]) ; 
   glBufferData(GL_ARRAY_BUFFER, sizecol, col,GL_STATIC_DRAW);
   glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_COLOR_ARRAY) ; 
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements+offset]) ; 
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeind, inds,GL_STATIC_DRAW);
   PrimType[object] = type ; 
   NumElems[object] = sizeind ; 
}

// Very basic code to read a ppm file
// And then set up buffers for texture coordinates
void inittexture (const char * filename, GLuint program) {
	FILE * fp ; 
	GLint err ; 

   assert(fp = fopen(filename,"rb")) ;
   fscanf(fp,"%*s %*d %*d %*d%*c") ;
   for (int i = 0 ; i < 256 ; i++)
     for (int j = 0 ; j < 256 ; j++)
       for (int k = 0 ; k < 3 ; k++)
		fscanf(fp,"%c",&(woodtexture[i][j][k])) ;
   fclose(fp) ;  

   // Set up Texture Coordinates
   glGenTextures(1, texNames) ; 
   
   glBindBuffer(GL_ARRAY_BUFFER, buffers[numobjects*numperobj+ncolors]) ; 
   glBufferData(GL_ARRAY_BUFFER, sizeof (floortex), floortex,GL_STATIC_DRAW);
   glActiveTexture(GL_TEXTURE0) ; 
   glEnable(GL_TEXTURE_2D) ; 
   glTexCoordPointer(2,GL_FLOAT,0,BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_TEXTURE_COORD_ARRAY) ; 

   glBindTexture (GL_TEXTURE_2D, texNames[0]) ; 
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, woodtexture) ;
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

   // Define a sampler.  See page 709 in red book, 7th ed.
   GLint texsampler ; 
   texsampler = glGetUniformLocation(program, "tex") ; 
   glUniform1i(texsampler,0) ; // Could also be GL_TEXTURE0 
   istex = glGetUniformLocation(program,"istex") ; 
}
/*
// Simple function to set the color separately.  Takes out colors
void initobjectnocol(GLuint object, GLfloat * vert, GLint sizevert, GLubyte * inds, GLint sizeind, GLenum type) {
   int offset = object * numperobj ; 

   glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices+offset]) ; 
   glBufferData(GL_ARRAY_BUFFER, sizevert, vert,GL_STATIC_DRAW);
   glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_VERTEX_ARRAY) ; 
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements+offset]) ; 
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeind, inds,GL_STATIC_DRAW);
   PrimType[object] = type ; 
   NumElems[object] = sizeind ; 
}

// And a function to draw with them, similar to drawobject but with color
void drawcolor(GLuint object, GLuint color) {
   int offset = object * numperobj ; 
   int base = numobjects * numperobj ; 

   glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices+offset]) ; 
   glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_VERTEX_ARRAY) ; 
   glBindBuffer(GL_ARRAY_BUFFER, buffers[base+color]) ; // Set color correctly 
   glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_COLOR_ARRAY) ; 
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements+offset]) ; 
   glDrawElements(PrimType[object], NumElems[object], GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)) ; 
}
*/
// And a function to draw with textures, similar to drawobject
void drawtexture(GLuint object, GLuint texture) {
   int offset = object * numperobj ; 
   int base = numobjects * numperobj + ncolors ; 

   glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices+offset]) ; 
   glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_VERTEX_ARRAY) ; 

   // Even with texturing, so we can blend if needed.
   glBindBuffer(GL_ARRAY_BUFFER, buffers[Colors+offset]) ; 
   glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_COLOR_ARRAY) ; 

   glActiveTexture(GL_TEXTURE0) ; 
   glEnable(GL_TEXTURE_2D) ; 
   glBindTexture(GL_TEXTURE_2D, texture) ; 
   glEnableClientState(GL_TEXTURE_COORD_ARRAY) ; 
   glBindBuffer(GL_ARRAY_BUFFER, buffers[base]) ; // Set texcoords
   glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements+offset]) ; 
   glDrawElements(PrimType[object], NumElems[object], GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)) ; 
}
/*
void drawobject(GLuint object) {
   int offset = object * numperobj ; 

   glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices+offset]) ; 
   glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_VERTEX_ARRAY) ; 
   glBindBuffer(GL_ARRAY_BUFFER, buffers[Colors+offset]) ; 
   glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)) ; 
   glEnableClientState(GL_COLOR_ARRAY) ; 
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements+offset]) ; 
   glDrawElements(PrimType[object], NumElems[object], GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)) ; 
}
*/
#endif
