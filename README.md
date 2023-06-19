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
  entt::registry registry = entt::basic_registry();
  entt::dispatcher dispatcher;

  frame::framework framework(&context, registry, dispatcher);

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

  registry.ctx().emplace<shader>("shaders/example.vert", "shaders/simple.frag");

  auto tick_signal = dispatcher.sink<frame::tick_event>();
  tick_signal.connect<&example_listener::tick_svo>(example_listener {});

  framework.init_gui();
}
```
