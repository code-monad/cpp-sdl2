#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <string>

#include "renderer.hpp"
#include "vec2.hpp"

#ifdef CPP_SDL2_VK_WINDOW
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#endif

namespace sdl
{
///\brief Represent an SDL window. Also contains accessor to any window
/// related adjacent functionality like OpenGL/Vulkan helper functions
class Window
{
public:
	///Construct a window. This safely create an SDL_Window for you
	///\param title Name of the window
	///\param size Size of the window on screen when shown
	///\param flags Any flags needed to be passed to SDL_CreateWindow
	Window(
		std::string const& title,
		Vec2i const&	   size,
		Uint32			   flags = SDL_WINDOW_SHOWN)
		: window_{SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			size.x,
			size.y,
			flags)}
	{
		if (!window_) throw Exception{"SDL_CreateWindow"};
	}

	///Construct an invalid window object. You will need to initialize it by moving in a real sdl::Window in
	Window() = default;

	///Default move ctor
	Window(Window&& other) noexcept { *this = std::move(other); }

	///Move assign operator. If this object represent a valid window, it will be destroyed before
	///acquiring the window_ pointer from other
	///\param other Another sdl::Window object
	Window& operator=(Window&& other) noexcept
	{
		if (this->window_ != other.window_)
		{
			SDL_DestroyWindow(window_);
			window_		  = other.window_;
			other.window_ = nullptr;
		}
		return *this;
	}

	// Non copyable class
	Window(Window&) = delete;
	Window& operator=(Window&) = delete;

	///Destructor. Calls SDL_DestroyWindow() automatically for you.
	virtual ~Window() { SDL_DestroyWindow(window_); }

	///Getter for the raw SDL2 window pointer
	SDL_Window* ptr() const { return window_; }

	///2D renderer factory. Permit to easily create a 2D renderer.
	/// Hardware acceleration enabled by default
	///\param flags Any flags needed to be passed to SDL_CreateRenderer.
	Renderer make_renderer(Uint32 flags = SDL_RENDERER_ACCELERATED) const
	{
		const auto render = SDL_CreateRenderer(window_, -1, flags);
		if (!render) throw Exception{"SDL_CreateRenderer"};
		return Renderer{render};
	}

	///Get the current window display index
	int display_index() const
	{
		const auto r = SDL_GetWindowDisplayIndex(window_);
		if (r == -1) throw Exception{"SDL_GetWindowDisplayIndex"};
		return r;
	}

	///Set the window display mode
	///\param mode Display mode to use
	void set_display_mode(SDL_DisplayMode const& mode) const
	{
		if (SDL_SetWindowDisplayMode(window_, &mode) != 0)
		{
			throw Exception{"SDL_SetWindowDisplayMode"};
		}
	}

	///Get the current display mode
	SDL_DisplayMode display_mode() const
	{
		SDL_DisplayMode mode;
		if (SDL_GetWindowDisplayMode(window_, &mode) != 0)
		{
			throw Exception{"SDL_GetWindowDisplayMode"};
		}
		return mode;
	}

	///Get the flags of this window
	Uint32 flags() const { return SDL_GetWindowFlags(window_); }

	///Grab window
	Window& grab(bool g = true)
	{
		SDL_SetWindowGrab(window_, static_cast<SDL_bool>(g));
		return *this;
	}

	///Release window
	Window& release(bool r = true) { return grab(!r); }

	///Is window grabed
	bool grabbed() const { return SDL_GetWindowGrab(window_); }

	///Move window to specific location on screen
	///\param v Vector pointing to the new window location
	Window& move_to(Vec2i const& v)
	{
		SDL_SetWindowPosition(window_, v.x, v.y);
		return *this;
	}
	///Translate window on screen
	///\param v translation vector
	Window& move_by(Vec2i const& v) { return move_to(position() + v); }
	///Get current window position
	Vec2i position() const
	{
		Vec2i pos;
		SDL_GetWindowPosition(window_, &pos.x, &pos.y);
		return pos;
	}

	///Change the size of the window
	///\newsize the size of the window
	void resize(Vec2i const& newsize) const
	{
		SDL_SetWindowSize(window_, newsize.x, newsize.y);
	}
	///Get current window size
	Vec2i size() const
	{
		Vec2i s;
		SDL_GetWindowSize(window_, &s.x, &s.y);
		return s;
	}

	///Change window name
	///\param t new window "title"
	Window& rename(std::string const& t)
	{
		SDL_SetWindowTitle(window_, t.c_str());
		return *this;
	}
	///\Get the current window title
	std::string title() const
	{
		return std::string{SDL_GetWindowTitle(window_)};
	}

	///Set the window icon
	///\param icon Surface containing the icon to use
	void set_icon(Surface const& icon) const
	{
		SDL_SetWindowIcon(window_, icon.ptr());
	}

	///Set the window icon (may require linking and activating SDL_Image)
	///\param filename path to a file you can use to set the window icon
	void set_icon(std::string const& filename) const
	{
		auto icon = Surface{filename};
		SDL_SetWindowIcon(window_, icon.ptr());
	}

	///Hide the window
	Window& hide()
	{
		SDL_HideWindow(window_);
		return *this;
	}
	///Maximize the window
	Window& maximize()
	{
		SDL_MaximizeWindow(window_);
		return *this;
	}
	///Minimize the window
	Window& minimize()
	{
		SDL_MinimizeWindow(window_);
		return *this;
	}
	///Raise the window
	Window& raise()
	{
		SDL_RaiseWindow(window_);
		return *this;
	}
	///Restore the window
	Window& restore()
	{
		SDL_RestoreWindow(window_);
		return *this;
	}

	///Returns true if window is currently fullscreen
	/// (both real and "desktop mode")
	bool fullscreen() const
	{
		return flags()
			   & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	///Set the window fullscreen
	Window& set_fullscreen(bool fs)
	{
		if (SDL_SetWindowFullscreen(
				window_, fs ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)
			!= 0)
		{
			throw Exception{"SDL_SetWindowFullscreen"};
		}
		return *this;
	}

	///Toggle the window fullscreen
	Window& toggle_fullscreen() { return set_fullscreen(!fullscreen()); }

	///Get window manager info. Exact content of this structure is fully platform dependant.
	SDL_SysWMinfo wm_info() const
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (SDL_GetWindowWMInfo(window_, &info) == SDL_FALSE)
		{
			throw Exception("SDL_GetWindowWMInfo");
		}
		return info;
	}

#ifdef CPP_SDL2_VK_WINDOW
	///Enumerate the required extensions to create a VkSurfaceKHR on the current system
	/// \return a vector of const char strings containing the extensions names
	std::vector<const char*> vk_get_instance_extensions()
	{
		std::vector<const char*> extensions;
		Uint32					 count;

		if (!SDL_Vulkan_GetInstanceExtensions(window_, &count, nullptr))
			throw Exception("SDL_Vulkan_GetInstanceExtensions");

		extensions.resize(count);

		if (!SDL_Vulkan_GetInstanceExtensions(
				window_, &count, extensions.data()))
			throw Exception("SDL_Vulkan_GetInstaceExtensions");

		return extensions; // Benefit from enforced RVO
	}

	///Create a vulkan surface for the current platform
	/// \return your vulkan surface
	VkSurfaceKHR vk_create_surface(VkInstance instance)
	{
		VkSurfaceKHR surface;
		if (!SDL_Vulkan_CreateSurface(window_, instance, &surface))
			throw Exception("SDL_Vulkan_CreateSurface");

		return surface;
	}

	///Create a vulkan surface using the C++ wrapper around UniqueHandle
	vk::UniqueSurfaceKHR vk_create_unique_surface(vk::Instance instance)
	{
		auto nakedSurface = vk_create_surface(instance);
		return vk::UniqueSurfaceKHR(nakedSurface, instance);
	}

	///Get the underlayoing drawable size of the window. Use this for vulkan viewport size, scissor rects and any place you need `VkExtent`s
	/// \return Size in an integer 2D vector
	Vec2i vk_get_drawable_size()
	{
		Vec2i size;
		SDL_Vulkan_GetDrawableSize(window_, &size.x, &size.y);
		return size;
	}
#endif // vulkan methods

#ifdef CPP_SDL2_GL_WINDOW

	// This function is mostly used to set values regarding SDL GL context, and
	// is intertwined with window creation. However, this can be called before
	// creation the window. This wrapping is mostly for API consistency and for
	// automatic error checking.
	static void gl_set_attribute(SDL_GLattr attr, int val)
	{
		if (SDL_GL_SetAttribute(attr, val) < 0)
			throw Exception("SDL_GL_SetAttribute");
	}

	static void gl_set_attribute(SDL_GLattr attr, bool val)
	{
		gl_set_attribute(attr, val ? 1 : 0);
	}

	///Get the value of the specified OpenGL attribute
	static int gl_get_attribute(SDL_GLattr attr)
	{
		int value;
		if (SDL_GL_GetAttribute(attr, &value) != 0)
			throw Exception("SDL_GL_GetAttribute");
		return value;
	}

	///Reset all OpenGL attributes to their default values
	static void gl_reset_attribute() { SDL_GL_ResetAttributes(); }

	///Return true if the specified extension is supported
	static bool gl_is_extension_supported(std::string const& ext_name)
	{
		return SDL_GL_ExtensionSupported(ext_name.c_str()) == SDL_TRUE;
	}

	///Define the type of window swapping strategy for opengl windows.
	enum class gl_swap_interval
	{
		immediate,
		vsync,
		adaptive_vsync
	};

	///Set the swap interval. If exception thrown while attempting to use adaptive vsync, use standard vsync.
	static void gl_set_swap_interval(gl_swap_interval swap_mode)
	{
		auto result = 0;
		switch (swap_mode)
		{
		case gl_swap_interval::immediate:
			result = SDL_GL_SetSwapInterval(0);
			break;
		case gl_swap_interval::vsync: result = SDL_GL_SetSwapInterval(1); break;
		case gl_swap_interval::adaptive_vsync:
			result = SDL_GL_SetSwapInterval(-1);
			break;
		}

		if (result != 0) throw Exception("SDL_GL_SetSwapInterval");
	}

	///Get the current swap interval
	static gl_swap_interval gl_get_swap_interval()
	{
		const auto value = SDL_GL_GetSwapInterval();
		if (value == 1) return gl_swap_interval::vsync;
		if (value == -1) return gl_swap_interval::adaptive_vsync;
		return gl_swap_interval::immediate;
	}

	///Nested class that represent a managed OpenGL Context by the SDL
	class GlContext
	{
	public:
		///Create a GlContext for the given window.
		/// You should use sdl::Window::gl_create_context() instead of this
		GlContext(SDL_Window* w) : context_{SDL_GL_CreateContext(w)}, owner_{w}
		{
			if (!context_) throw Exception("SDL_GL_CreateContext");
		}

		///Construct an invalid GLContext object where you will need to move a new one
		GlContext() = default;

		///Dtor will call SDL_GL_DeleteContext on the enclosed context
		~GlContext() { SDL_GL_DeleteContext(context_); }

		// Only movable, not copyable
		GlContext(GlContext const&) = delete;
		GlContext& operator=(GlContext const&) = delete;

		GlContext(GlContext&& other) noexcept { *this = std::move(other); }

		GlContext& operator=(GlContext&& other) noexcept
		{
			if (context_ != other.context_)
			{
				context_	   = other.context_;
				owner_		   = other.owner_;
				other.context_ = nullptr;
			}
			return *this;
		}

		void make_current() const
		{
			if (SDL_GL_MakeCurrent(owner_, context_) < 0)
				throw Exception("SDL_GL_MakeCurrent");
		}

		SDL_GLContext ptr() const { return context_; }

	private:
		SDL_GLContext context_ = nullptr;
		SDL_Window*	  owner_   = nullptr;
	};

	///Create an OpenGL context from the current window
	GlContext create_context() const { return GlContext{window_}; }

	///Swap buffers for GL when using double buffering on the current window
	void gl_swap() const { SDL_GL_SwapWindow(window_); }
#endif

private:
	///Raw naked pointer to an SDL window
	SDL_Window* window_ = nullptr;
};

} // namespace sdl
