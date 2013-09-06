#include "hello.h"

GLuint vertex_shader, fragment_shader, program;

CvVideoWriter *writer = 0;

int counter = 0;
int image_width = 512;
int image_height = 512;

static struct {
   struct {
       GLint position;
   } attributes;

   GLuint vertex_buffer;

   GLuint textures[2];
   struct {
       GLint textures[2];
   } uniforms;

} g_resources;

static GLuint makeBuffer(
    GLenum target,
    const void *buffer_data,
    GLsizei buffer_size
) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}

static GLuint makeTexture(const char *filename)
{
    int width, height;
    unsigned char *pixels = read_jpeg(filename, &width, &height);
    GLuint texture;

    if (!pixels)
        return 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level */
        GL_RGB8,                    /* internal format */
        width, height, 0,           /* width, height, border */
        GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
        pixels                      /* pixels */
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(pixels);
    return texture;
}

static GLuint makeShader(GLenum type, const char *filename)
{
    GLint length;
    GLchar *source = (GLchar *)file_contents(filename, &length);
    GLuint shader;
    GLint shader_ok;

    if (!source)
        return 0;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    free(source);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint makeProgram(GLuint vertex_shader, GLuint fragment_shader)
{
    GLint program_ok;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

static void setupScreen(int w, int h){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, w, h);
}

static int initShaders(void){
    vertex_shader = makeShader( GL_VERTEX_SHADER, "noop.v.glsl");
    if (vertex_shader == 0)
        return 0;

    fragment_shader = makeShader( GL_FRAGMENT_SHADER, "noop.f.glsl");
    if (fragment_shader == 0)
        return 0;

    program = makeProgram(vertex_shader, fragment_shader);
    if (program == 0)
        return 0;

    glUseProgram(program);
}

static void loadTexture(void){
    g_resources.textures[0] = makeTexture("images/01-qwiki-reference.jpg");
    g_resources.uniforms.textures[0] = glGetUniformLocation(program, "textures[0]");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_resources.textures[0]);
    glUniform1f(g_resources.uniforms.textures[0], 0);
}

static void setupTexture(void){
  // makeTexture already calls texImage2D
}

static void setupBuffers(void){
    GLfloat g_vertex_buffer_data[] = {
       -0.5f, -0.5f, 0.0f,
       0.5, -0.5, 0.0f,
       0.5f, 0.5f, 0.0f,

       0.5f, 0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f
    };

    g_resources.vertex_buffer = makeBuffer(
        GL_ARRAY_BUFFER,
        g_vertex_buffer_data,
        sizeof(g_vertex_buffer_data)
    );
}

static float angle = 0.0;

static void setupDynamicBuffers(){
  GLfloat x_translation = sin(angle)/2.0f;
  GLfloat triangleVertices[] = {
          //left triangle
  -0.5 + x_translation, 0.5, 0.0,
  0.0  + x_translation, 0.5, 0.0,
  -0.5 + x_translation, -0.5, 0.0,
  //right triangle
  0.0 + x_translation, 0.5, 0.0,
  0.0 + x_translation, -0.5, 0.0,
  -0.5 + x_translation, -0.5, 0.0
  };

  angle += 0.01;

  g_resources.vertex_buffer = makeBuffer(
      GL_ARRAY_BUFFER,
      triangleVertices,
      sizeof(triangleVertices)
  );
}

static void drawScene(void)
{
  int width = image_width;
  int height = image_height;

  setupScreen(width, height);
  int numOfVertices = 6;

  g_resources.attributes.position
      = glGetAttribLocation(program, "position");

  glEnableVertexAttribArray(g_resources.attributes.position);
  glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
  glVertexAttribPointer(
      g_resources.attributes.position,  /* attribute */
      3,                                /* size */
      GL_FLOAT,                         /* type */
      GL_FALSE,                         /* normalized? */
      0,//sizeof(GLfloat)*3,                /* stride */
      (void*)0                          /* array buffer offset */
  );
  glDrawArrays(GL_TRIANGLES, 0, numOfVertices);

  glutSwapBuffers();
}

static void screenCapture(int width, int height){
  unsigned char *raw_image = (unsigned char*) calloc(width * height * 3, sizeof(char));
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, raw_image);

  IplImage* img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  img->imageData = (char *)raw_image;
  cvWriteFrame(writer, img);      // add the frame to the file
  cvReleaseImage(&img);
}

static void replay(){
  int width = image_width;
  int height = image_height;

  setupDynamicBuffers();
  drawScene();
  screenCapture(width, height);
}

static int setupBoard(int width, int height, int argc, char** argv){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutCreateWindow("Hello World");

  glewInit();
  if (!GLEW_VERSION_2_0) {
      fprintf(stderr, "OpenGL 2.0 not available\n");
      return -1;
  }

  return 1;
}

static int setupVideoWriting(){
  int isColor = 1;
  int fps     = 30;
  int width  = image_width;
  int height  = image_height;

  // You can change CV_FOURCC('' to CV_FOURCC('X', '2', '6', '4') for x264 output
  // Make sure you compiled ffmpeg with x264 support and OpenCV with x264
  writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(width, height), isColor);

  return 0;
}

int main(int argc, char** argv){
  setupVideoWriting();

  int width = image_width;
  int height = image_height;

  setupBoard(width, height, argc, argv);

  setupScreen(width, height);

  initShaders();

  loadTexture();
  setupTexture();

  setupBuffers();

  glutDisplayFunc(&drawScene);
  glutIdleFunc(&replay);

  glutMainLoop();

  cvReleaseVideoWriter(&writer);
  return 0;
}
