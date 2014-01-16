/**
 * @author Jon Hatchett
 */

#ifndef GLBITS_H
#define GLBITS_H

#if defined(__APPLE__)
#	include <OpenGL/gl3.h>
#	define _stdcall
#	define _GL_CALL
#elif defined(_WIN32)
#	include <Windows.h>
#	include <GL/glew.h>
#	define _GL_CALL APIENTRY
#elif defined(__linux__)
#	include <GL/glew.h>
#	include <GL/gl.h>
#	define _stdcall
#	define _GL_CALL
#else
#	error 'unsupported platform'
#endif

#endif
