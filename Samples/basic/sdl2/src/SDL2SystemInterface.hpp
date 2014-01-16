#ifndef __SYSTEMINTERFACE_H__
#define __SYSTEMINTERFACE_H__

#include <chrono>

#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/Input.h>

#include <SDL2/SDL_keycode.h>

namespace {
	using Clock = std::chrono::system_clock;
	//using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<float>;
}

class SDL2SystemInterface : public Rocket::Core::SystemInterface
{
public:
	
	/// Get the number of seconds elapsed since the start of the application.
	/// \return Elapsed time, in seconds.
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
