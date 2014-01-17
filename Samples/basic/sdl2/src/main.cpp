/**
 * @author Jon Hatchett
 */

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <Rocket/Core.h>
#include <Rocket/Debugger/Debugger.h>

#include "OpenGL32RenderInterface.hpp"
#include "SDL2SystemInterface.hpp"
#include "ShellFileInterface.hpp"

#include "glbits.h"
#include "glexception.hpp"
#include "sdlbits.h"

#define FPS 60.0f

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<float>;
using namespace std;

int main(int argc, const char * argv[]) {
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {throw runtime_error(SDL_GetError());}

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window * window;
	if ((window = SDL_CreateWindow("libRocket+SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)) == NULL) {throw runtime_error(SDL_GetError());}

	SDL_GLContext glContext;
	if ((glContext = SDL_GL_CreateContext(window)) == NULL) {
		throw runtime_error(SDL_GetError());
	}

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw runtime_error("GLEW init failed.");
	}
	glGetError(); // GLEW causes an error. Throw it away.
#endif

	if (SDL_GL_SetSwapInterval(-1) == -1) {
		if (SDL_GL_SetSwapInterval(1) == -1) {
			cerr << "Neither late swap tearing or VSync supported. Not biggie." << endl;
		}
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	_glException();

	// Init libRocket
	ShellFileInterface fileInterface("../../assets/");
	OpenGL32RenderInterface renderInterface;
	SDL2SystemInterface systemInterface;

	Rocket::Core::SetFileInterface(&fileInterface);
	Rocket::Core::SetRenderInterface(&renderInterface);
	Rocket::Core::SetSystemInterface(&systemInterface);

	if (!Rocket::Core::Initialise()) {
		throw runtime_error("Initialise failed.");
	}

	Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
	Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");
	Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
	Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");

	Rocket::Core::Context * context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(1024, 768));

	Rocket::Debugger::Initialise(context);
	Rocket::Debugger::SetVisible(true);

	Rocket::Core::ElementDocument * document = context->LoadDocument("demo.rml");
	if (!document) {
		throw runtime_error("Document not loaded.");
	}

	document->Show();
	document->RemoveReference();

	// Main loop
	bool running = true;
	SDL_Event event;
	TimePoint tick = Clock::now();
	while (running) {
		context->Update();

		SDL_GL_MakeCurrent(window, glContext);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		_glException();
		glClear(GL_COLOR_BUFFER_BIT);
		_glException();
		context->Render();
		SDL_GL_SwapWindow(window);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					Rocket::Core::Vector2i v(event.window.data1, event.window.data2);
					context->SetDimensions(v);
					renderInterface.SetViewport(event.window.data1, event.window.data2);
					break;
				}
				break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						running = false;
						break;
					}
					context->ProcessKeyDown(systemInterface.TranslateKey(event.key.keysym), systemInterface.GetKeyModifiers());
					break;
				case SDL_KEYUP:
					context->ProcessKeyUp(systemInterface.TranslateKey(event.key.keysym), systemInterface.GetKeyModifiers());
					break;
				case SDL_MOUSEMOTION:
					context->ProcessMouseMove(event.motion.x, event.motion.y, systemInterface.GetKeyModifiers());
					break;
				case SDL_MOUSEBUTTONDOWN:
					context->ProcessMouseButtonDown(systemInterface.TranslateButton(event.button.button), systemInterface.GetKeyModifiers());
					break;
				case SDL_MOUSEBUTTONUP:
					context->ProcessMouseButtonUp(systemInterface.TranslateButton(event.button.button), systemInterface.GetKeyModifiers());
					break;
				case SDL_MOUSEWHEEL:
					context->ProcessMouseWheel(-event.wheel.y, systemInterface.GetKeyModifiers());
					break;
				case SDL_QUIT:
					running = false;
					break;
			}
		}

		Duration delta = Clock::now() - tick;
		Duration delay = max(Duration(1.0f / FPS) - delta, Duration(1e-3)); // Max FPS 1/1000. Give the OS a break.
		this_thread::sleep_for(delay);
		tick = Clock::now();
	}

	// Clean up libRocket
	document->GetContext()->UnloadDocument(document);

	// Clean up SDL
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

