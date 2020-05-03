#pragma once

#include <SDL2/SDL.h>

#include "color.hpp"
#include "event.hpp"
#include "exception.hpp"
#include "game_controller.hpp"
#include "haptic.hpp"
#include "joystick.hpp"
#include "mouse.hpp"
#include "rect.hpp"
#include "renderer.hpp"
#include "shared_object.hpp"
#include "simd.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include "utils.hpp"
#include "vec2.hpp"
#include "window.hpp"

/**
 *
 * \mainpage cpp-sdl2 header-only C++ bindings for SDL2
 *
 *
 * \section doc_intro Introduction
 *
 * SDL2 is arguably one of the most helpfull free and open source libraries. As
 * it is both a common API for many platform and operating systems to do
 * frequent operations (opening window/ display surfaces, getting inputs from
 * the user, loading libraries...).
 *
 * SDL is written in C.
 * C here is really valuable for the portability of the SDL to many platforms,
 * and bindings to other programming languages. However, this makes the library
 * rely on direct manipulation of pointers and data structure in memory, and
 * manual management of the lifetime of the resources owned by said pointer.
 *
 * The goal of this project is to provide a modern, easy to use C++ wrapper
 * around SDL2. Including it's internal 2D renderer, and as an aid in
 * developement using other libraries in a cross-platform context.
 *
 * These wrappers are implemented only on header files, and it's permissive
 * license allows you to simply add it's source code to any project that include
 * and links the SDL, and that can be built with a modern C++ compiler following
 * the latest standards.
 *
 * This library mainly contains wrapper around SDL resources that are managed
 * using the RAII (Resource Acquisition Is Initialization) idiom. This simply
 * means that theses objects will do the dirty work for you by obtaining these
 * resources (window, memory...) at their construciton, and clean up when they
 * naturally go out of scope, making memory safe and leak-free programs easier
 * to accomplish.
 *
 * This library also allows you to access to all the SDL functionalities that
 * are exposed via a collection of functions.
 *
 * For example, all the "SDL_Functions" that act on a window and take an
 * SDL_Window pointer as argument are now simpler to use function members of the
 * sdl::Window class.
 *
 * \section license License
 *
 * This softwrae is distributed under the terms of the MIT License
 *
 * Copyright (c) 2018-2019 Edhebi and contributors
 *
 */

namespace sdl
{
///\brief This class represent the "library entry point". Create and keep an instance of this object before creating any other sdl object (e.g. sdl::Window)
///
///Create an sdl::Root object on the stack, and SDL is initialized.
///When your instance of Root goes out of scope, SDL will be deinitialized for you.
///This pattern (idiom) is called RAII. If you aren't familiar with it, I suggest reading
///https://en.cppreference.com/w/cpp/language/raii

// clang-format off
struct [[nodiscard]] Root
{
	///\brief Construct a root object. This object initialize SDL for you. SDL is de-initialized when object quit scope
	///\param flags The intialization flags that you want to pass to SDL_Init. Default value is SDL_INIT_EVERYTHING
	///
	///This constructor will throw an sdl::Exception if SDL_Init fails.
	Root(Uint32 flags = SDL_INIT_EVERYTHING)
	{
		if (SDL_Init(flags) != 0)
			throw Exception{"SDL_Init"};
	}

	///Automatically quit SDL for you!
	~Root()
	{
		SDL_Quit();
	}
};
// clang-format on

} // namespace sdl
