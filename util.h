void *file_contents(const char *filename, GLint *length);
void *read_tga(const char *filename, int *width, int *height);
unsigned char *read_jpeg( const char *filename, int *width, int *height );
int write_jpeg(const char *filename, unsigned char* raw_image, int width, int height);
 
