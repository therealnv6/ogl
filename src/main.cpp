#include "imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <buffer.hpp>
#include <entt/entt.hpp>
#include <framework.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <render.hpp>
#include <shader.hpp>
#include <spdlog/spdlog.h>
#include <window.hpp>

std::array<float, 9 * 12> data = {
	-1.0f, -1.0f, -1.0f, // triangle 1 : begin
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f, -1.0f, // triangle 2 : begin
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f, // triangle 2 : end
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

std::array<float, 9 * 12> colors = {
	0.583f, 0.771f, 0.014f,
	0.609f, 0.115f, 0.436f,
	0.327f, 0.483f, 0.844f,
	0.822f, 0.569f, 0.201f,
	0.435f, 0.602f, 0.223f,
	0.310f, 0.747f, 0.185f,
	0.597f, 0.770f, 0.761f,
	0.559f, 0.436f, 0.730f,
	0.359f, 0.583f, 0.152f,
	0.483f, 0.596f, 0.789f,
	0.559f, 0.861f, 0.639f,
	0.195f, 0.548f, 0.859f,
	0.014f, 0.184f, 0.576f,
	0.771f, 0.328f, 0.970f,
	0.406f, 0.615f, 0.116f,
	0.676f, 0.977f, 0.133f,
	0.971f, 0.572f, 0.833f,
	0.140f, 0.616f, 0.489f,
	0.997f, 0.513f, 0.064f,
	0.945f, 0.719f, 0.592f,
	0.543f, 0.021f, 0.978f,
	0.279f, 0.317f, 0.505f,
	0.167f, 0.620f, 0.077f,
	0.347f, 0.857f, 0.137f,
	0.055f, 0.953f, 0.042f,
	0.714f, 0.505f, 0.345f,
	0.783f, 0.290f, 0.734f,
	0.722f, 0.645f, 0.174f,
	0.302f, 0.455f, 0.848f,
	0.225f, 0.587f, 0.040f,
	0.517f, 0.713f, 0.338f,
	0.053f, 0.959f, 0.120f,
	0.393f, 0.621f, 0.362f,
	0.673f, 0.211f, 0.457f,
	0.820f, 0.883f, 0.371f,
	0.982f, 0.099f, 0.879f
};

class test_framework : public frame::framework
{
public:
	entt::registry registry;

	// position
	glm::vec3 position = glm::vec3(0, 0, 5);
	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 45.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.005f;

	std::unique_ptr<buffer::buffer<std::array<float, 9 * 12>>> vertices_buffer;
	std::unique_ptr<buffer::buffer<std::array<float, 9 * 12>>> color_buffer;
	std::unique_ptr<shader::shader> shader;

	int matrix_id = 0;

	test_framework(gfx::context *context)
		: frame::framework(context)
	{
		this->initialize();
	}

	void initialize() override
	{
		this->registry = entt::basic_registry();
		// this->init_gui();

		context->input_mode(input::input_mode::StickyKeys, true);
		context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_HIDDEN);

		gfx::enable(gfx::enable_fields::CullFace);
		gfx::depth(gfx::depth_function::Less);

		gfx::clear_color({ 0.0, 0.0, 0.4, 0.0 });

		// this must be called first
		buffer::reserve_vertex_array(1);

		this->vertices_buffer = std::make_unique<buffer::buffer<std::array<float, 9 * 12>>>(&data, sizeof(std::array<float, 9 * 12>), buffer::draw_type::Static);
		this->color_buffer = std::make_unique<buffer::buffer<std::array<float, 9 * 12>>>(&colors, sizeof(std::array<float, 9 * 12>), buffer::draw_type::Static);

		this->shader = std::make_unique<shader::shader>("shaders/simple.vert", "shaders/simple.frag");

		this->matrix_id = shader->get_uniform_location("mvp");
	}

	void tick_gui(GLFWwindow *window, framework &framework) override
	{
		this->gui_frame();
		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
		ImGui::End();
	}

	void tick(GLFWwindow *window, framework &framework) override
	{

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) context->width() / (float) context->height(), 0.1f, 100.0f);
		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle));

		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f));

		glm::vec3 up = glm::cross(right, direction);
		glm::mat4 view = glm::lookAt(
			glm::vec3(position), // Camera is at (4,3,3), in World Space
			glm::vec3(position + direction), // and looks at the origin
			glm::vec3(up) // Head is up (set to 0,-1,0 to look upside-down)
		);

		if (ImGui::GetCurrentContext() == nullptr || !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		{
			context->reset_mouse();

			auto [xpos, ypos] = context->get_mouse_pos();

			horizontalAngle += mouseSpeed * deltaTime * float(1920.0 / 2 - xpos);
			verticalAngle += mouseSpeed * deltaTime * float(1080.0 / 2 - ypos);

			if (this->is_pressed(input::input::w))
			{
				position += direction * deltaTime * speed;
			}

			if (this->is_pressed(input::input::a))
			{
				position -= right * deltaTime * speed;
			}

			if (this->is_pressed(input::input::s))
			{
				position -= direction * deltaTime * speed;
			}

			if (this->is_pressed(input::input::d))
			{
				position += right * deltaTime * speed;
			}
		}

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = (projection * view * model);

		gfx::clear(gfx::clear_buffer::Color | gfx::clear_buffer::Depth);

		shader->bind();

		glUniformMatrix4fv(matrix_id, 1, false, &mvp[0][0]);

		vertices_buffer->bind_vertex(0, 3);
		color_buffer->bind_vertex(1, 3);

		gfx::draw_arrays(0, 12 * 3);
	}
};

int main()
{
	gfx::context context("voxel", 1920, 1080);
	test_framework framework(&context);

	framework.run();
}
