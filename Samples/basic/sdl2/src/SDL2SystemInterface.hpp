#ifndef SYSTEMINTERFACE_HPP
#define SYSTEMINTERFACE_HPP

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
