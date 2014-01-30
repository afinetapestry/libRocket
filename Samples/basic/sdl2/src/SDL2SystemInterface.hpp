/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2014 Jon Hatchett
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef SDL2SYSTEMINTERFACE_HPP
#define SDL2SYSTEMINTERFACE_HPP

#include <chrono>

#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/Input.h>

#include "sdlbits.h"

namespace {
	using Clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<float>;
}

/**
 * @author Miguel Martin (https://github.com/miguelishawt/SDL2-libRocket-SystemInterface)
 * @author Jon Hatchett
 * @author Gabriel Jacobo
 */
class SDL2SystemInterface : public Rocket::Core::SystemInterface {
public:
	
	virtual float GetElapsedTime() {
		return Duration(Clock::now().time_since_epoch()).count();
	}
	
	/// Activate keyboard (for touchscreen devices)
	virtual void ActivateKeyboard();
	
	/// Deactivate keyboard (for touchscreen devices)
	virtual void DeactivateKeyboard();
	
	/// Called when this system interface is no longer required.
	virtual void Release();
    
    int GetKeyModifiers() const;
    Rocket::Core::Input::KeyIdentifier TranslateKey(const SDL_Keysym &) const;
    
    int TranslateButton(int button) const;
};

#endif // SYSTEMINTERFACE_HPP
