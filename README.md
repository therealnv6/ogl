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
class example : public frame::framework 
{
    buffer::unique_buffer vertex_buffer;

    void initialize() override
    {
        this->init_gui(); // this initializes the "ImGui" gui, and will enable all required hooks.

        // these are just a couple of recommended defaults, but these are not set by default as I try to keep it as unopinionated as possible.
        {
            context->input_mode(input::input_mode::StickyKeys, true);
            context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_DISABLED);
        }

        gfx::enable(gfx::enable_fields::CullFace);
        gfx::depth(gfx::depth_function::Less);

        // of course, this could be moved into an entity if required. not sure if EnTT has a resource structure? if it does, it should probably be in there. 
        // I still have to do some more experimenting with this. 
        vertex_buffer = std::make_unique(
            nullptr, // this is the data
            0, // this is the size of the buffer; this can later be re-allocated.
            draw_type::static_draw, // there are a couple of modes: dynamic_draw, static_draw and stream_draw, these are just an enum over the GL types.
            buffer_type::array // similar to draw_type, there are a couple of types: array, shader_storage, uniform_buffer 
        );

        registry.emplace<int>(registry.create(), 0);
    }
}
```
