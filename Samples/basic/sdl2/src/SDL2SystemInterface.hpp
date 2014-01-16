#ifndef SYSTEMINTERFACE_HPP
#define SYSTEMINTERFACE_HPP

#include <chrono>

#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/Input.h>

#include <SDL2/SDL_keycode.h>

namespace {
	using Clock = std::chrono::system_clock;
	//using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<float>;
}

/**
 * @author Miguel Martin
 * @url https://github.com/miguelishawt/SDL2-libRocket-SystemInterface
 */
class SDL2SystemInterface : public Rocket::Core::SystemInterface
{
public:
	
	/**
	 * @author Jon Hatchett
	 */
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
    Rocket::Core::Input::KeyIdentifier TranslateKey(const SDL_Keycode&) const;
    
    int TranslateButton(int button) const;
};

#endif // __SYSTEMINTERFACE_H__
