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

#include <vertices.h>

class test_framework : public frame::framework
{
public:
	entt::registry registry;

	glm::vec3 position = glm::vec3(0, 0, 5);
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFov = 45.0f;

	float speed = 30.0f;
	float mouseSpeed = 1.5f;

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
		this->init_gui();

		context->input_mode(input::input_mode::StickyKeys, true);
		context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_HIDDEN);

		gfx::enable(gfx::enable_fields::CullFace);
		gfx::depth(gfx::depth_function::Less);

		gfx::clear_color({ 0.0, 0.0, 0.4, 0.0 });

		// this must be called first before making the buffer
		buffer::reserve_vertex_array(1);

		this->vertices_buffer = std::make_unique<buffer::buffer<std::array<float, 9 * 12>>>(&data, sizeof(std::array<float, 9 * 12>), buffer::draw_type::Static);
		this->color_buffer = std::make_unique<buffer::buffer<std::array<float, 9 * 12>>>(&colors, sizeof(std::array<float, 9 * 12>), buffer::draw_type::Static);

		this->shader = std::make_unique<shader::shader>("shaders/simple.vert", "shaders/simple.frag");

		this->matrix_id = shader->get_uniform_location("mvp");
	}

	void tick_gui(GLFWwindow *window, framework &framework) override
	{
		this->gui_frame();

		ImGuiIO &io = ImGui::GetIO();
		(void) io;

		ImGui::Begin("ogl voxel");

		if (ImGui::TreeNode("Debug"))
		{
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::Text("horizontalAngle: %f", horizontalAngle);
			ImGui::SameLine();
			ImGui::Text("verticalAngle: %f", verticalAngle);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Controls"))
		{
			ImGui::SliderFloat("speed", &speed, 10.0f, 100.0f);
			ImGui::SliderFloat("mouseSpeed", &mouseSpeed, 0.5f, 10.0f);
			ImGui::TreePop();
		}

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
			cos(horizontalAngle - 3.14f / 2.0f) //
		);

		glm::vec3 up = glm::cross(right, direction);
		glm::mat4 view = glm::lookAt(
			glm::vec3(position), // Camera is at (4,3,3), in World Space
			glm::vec3(position + direction), // and looks at the origin
			glm::vec3(up) // Head is up (set to 0,-1,0 to look upside-down)
		);

		if (ImGui::GetCurrentContext() == nullptr || !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		{
			this->handle_input(&direction, &right);
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

	void handle_input(const glm::vec3 *direction, const glm::vec3 *right)
	{
		auto [width, height] = context->size();
		auto [xpos, ypos] = context->get_mouse_pos();

		context->reset_mouse();

		float mouseDeltaX = static_cast<float>(width / 2.0 - xpos);
		float mouseDeltaY = static_cast<float>(height / 2.0 - ypos);

		horizontalAngle += mouseSpeed * deltaTime * mouseDeltaX;
		verticalAngle += mouseSpeed * deltaTime * mouseDeltaY;

		std::map<input::input, glm::vec3> keyToDirection {
			{ input::input::w, *direction },
			{ input::input::a, -*right },
			{ input::input::s, -*direction },
			{ input::input::d, *right },
			{ input::input::spacebar, glm::vec3(0.0f, 1.0f, 0.0f) }, // Up
			{ input::input::shift, glm::vec3(0.0f, -1.0f, 0.0f) }  // Down
		};

		// Iterate over the input keys and perform the movement
		for (const auto &entry : keyToDirection)
		{
			if (this->is_pressed(entry.first))
			{
				position += entry.second * speed * deltaTime;
			}
		}
	}
};

int main()
{
	gfx::context context("voxel", 2560, 1440);
	test_framework framework(&context);

	framework.run();
}
