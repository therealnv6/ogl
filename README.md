# ogl

This is just a simple OpenGL library using modern C++ features. Just a small playground for me; not stable at all. Even if it became "stable" someday, I wouldn't recommend using it as there are much better alternatives out there.

# Voxels?

This used to be a voxel engine; but instead of just focusing on voxels, I decided to split the voxel part into a [different repository](https://github.com/therealnv6/voxels-ogl). That means this repository can now also be used as a standalone library.

# CMake

Implementing this library into your own project is very easy and straight-forward using CMake, here is a small example of how to do this:

```cmake
add_subdirectory(thirdparty/ogl)
target_link_libraries(${PROJECT_NAME} PRIVATE ogl)
```

# Example Usage

The usage is very straight-forward, mostly using the EnTT entity-component system. Here is a very small example:

```cpp

struct example_listener {
  void tick(const frame::tick_event &event)
  {
    std::cout << "called every tick!" << std::endl;
  }
};

int main()
{
  gfx::context context("voxel", 2560, 1440);

  // we have to make our own registry and dispatcher *before* we make the framework, as the framework needs these.  
  entt::registry registry = entt::basic_registry();
  entt::dispatcher dispatcher;

  frame::framework framework(&context, registry, dispatcher);

  // these are mostly recommended defaults, but are not set by default to 
  // stay as unopinionated as possible.
  {
    context.input_mode(input::input_mode::StickyKeys, true);
    context.input_mode(input::input_mode::Cursor, GLFW_CURSOR_DISABLED);
  }

  {
    gfx::enable(gfx::enable_fields::CullFace);
    gfx::depth(gfx::depth_function::Less);
    gfx::clear_color({ 0.0, 0.1, 0.2, 0.0 });
    buffer::reserve_vertex_array(1);
  }

  // this is just a small example of how to insert a resource into the entt registry, ideally, you should use
  // the actual resource management, look here: https://github.com/skypjack/entt/wiki/Crash-Course:-resource-management
  registry.ctx().emplace<shader>("shaders/example.vert", "shaders/simple.frag");

  // this makes a new signal for the frame::tick_event within the dispatcher we created above
  auto tick_signal = dispatcher.sink<frame::tick_event>();

  // this will connect a function to be called every time the tick_signal variable gets signaled to be invoked.
  tick_signal.connect<&example_listener::tick_svo>(example_listener {});

  // this is just to initialize ImGui, feel free to call this wherever.
  framework.init_gui();
}
```
