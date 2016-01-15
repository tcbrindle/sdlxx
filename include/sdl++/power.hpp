/*!
  @file power.hpp
  Header for the SDL power management routines.

  @copyright (C) 2014-2016 Tristan Brindle <t.c.brindle@gmail.com>
  @copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SDLXX_POWER_HPP
#define SDLXX_POWER_HPP

#include "SDL_power.h"

namespace sdl {

/*!
 @defgroup Power Power Management Status

 This category contains functions for handling SDL power management routines.

 Example:

 ```
 auto info = sdl::get_power_info();

 if (info.state == sdl::power_state::on_battery) {
     std::cout << "Running on battery with " << info.secs_left
               << "seconds of power remaining (" << info.percent_left << "%)\n";
 }

 ```

 @{
 */

/*!
 The basic state for the system's power supply.
 */
enum class power_state {
    //! Cannot determine power status
    unknown = SDL_POWERSTATE_UNKNOWN,
    //! Not plugged in, running on the battery
    on_battery = SDL_POWERSTATE_ON_BATTERY,
    //! Plugged in, no battery available
    no_battery = SDL_POWERSTATE_NO_BATTERY,
    //! Plugged in, charging battery
    charging = SDL_POWERSTATE_CHARGING,
    //! Plugged in, battery charged
    charged = SDL_POWERSTATE_CHARGED
};

/*!
 Power information
 */
struct power_info {
    //! The state of the battery (if any)
    power_state state;
    //! Estimated seconds of battery life left. May be -1 if we can't determine
    //! a value, or we're not running on a battery
    int secs_left;
    //! percentage of battery life left, between 0 and 100. May be -1 if we
    //! can't determine a value, or we're not running on a battery
    int percent_left;
};

//! Get the current power supply details
inline power_info get_power_info() {
    power_info i;
    i.state = static_cast<power_state>(
        ::SDL_GetPowerInfo(&i.secs_left, &i.percent_left));
    return i;
}

//! @}

} // end namespace sdl

#endif // SDLXX_POWER_HPP
