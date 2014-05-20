/**
 * @author Jon Hatchett
 */

#ifndef SDLBITS_H
#define SDLBITS_H

#if defined(_WIN32)
#	include <SDL.h>
#elif defined(__APPLE__)
#	include <SDL.h>
#elif defined(__linux__)
#	include <SDL2/SDL.h>
#else
#	error 'unsupported platform'
#endif

#endif

