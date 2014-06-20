#ifndef	__IMAGE_UTILS__
#define	__IMAGE_UTILS__

#include <Mmsystem.h>

#define FOURCC_DXT1	MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3	MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5	MAKEFOURCC('D', 'X', 'T', '5')

GLuint	loadDDS(const char * imagepath);
GLuint	loadBMP_custom(const char * imagepath);


#endif