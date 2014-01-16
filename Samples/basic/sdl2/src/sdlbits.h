/**
 * @author Jon Hatchett
 */

#ifndef SDLBITS_H
#define SDLBITS_H

#if defined(WIN32)
#	undef main
#	include <SDL.h>
#elif defined(__APPLE__)
#	include <SDL2/SDL.h>
#elif defined(__linux__)
#	include <SDL2/SDL.h>
#else
#	error 'unsupported platform'
#endif

#endif
