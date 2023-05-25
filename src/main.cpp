#include "imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <buffer.hpp>
#include <camera.hpp>
#include <framework.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <render.hpp>
#include <shader.hpp>
#include <spdlog/spdlog.h>
#include <vertices.h>
#include <window.hpp>

const static float MOVEMENT_SPEED = 30.0f;
const static float MOUSE_SPEED = 1.5f;

struct movement {
	glm::vec3 position = glm::vec3(0, 0, 5);

	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
};

struct poll_input_event {
	entt::registry *registry;
	frame::framework *framework;
};

class test_framework : public frame::framework
{
public:
	std::unique_ptr<buffer::buffer<std::array<float, 9 * 12>>> vertices_buffer;
	std::unique_ptr<buffer::buffer<std::array<float, 9 * 12>>> color_buffer;

	std::unique_ptr<shader::shader> shader;

	int matrix_id = 0;

	test_framework(gfx::context *context)
		: frame::framework(context)
	{
		this->initialize();
	}

	struct listener {
		void update_gui(const frame::tick_event &event)
		{
			imgui::frame();

			auto registry = event.registry;
			auto view = registry->view<movement>();

			for (auto [entity, movement] : view.each())
			{
				ImGuiIO &io = ImGui::GetIO();
				(void) io;

				ImGui::Begin("ogl voxel");

				if (ImGui::BeginTabBar("whale"))
				{
					if (ImGui::BeginTabItem("Debug"))
					{
						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

						ImGui::Text("horizontalAngle: %f", movement.horizontalAngle);
						ImGui::SameLine();
						ImGui::Text("verticalAngle: %f", movement.verticalAngle);
						ImGui::Text("x: %f, y: %f, z: %f", movement.position.x, movement.position.y, movement.position.z);

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Controls"))
					{
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}

				ImGui::End();
			}
		}

		void tick(const frame::tick_event &event)
		{
			auto registry = event.registry;
			auto framework = static_cast<test_framework *>(event.data);

			auto entity_view = registry->view<movement>();

			for (auto [entity, move] : entity_view.each())
			{
				{
					float phase = glfwGetTime() * 2.0f * M_PI * 0.2f;

					for (int y = 0; y < 12; y++)
					{
						for (int x = 0; x < 9; x++)
						{
							float fo = static_cast<float>(x) / (9 - 1);
							float so = static_cast<float>(y) / (12 - 1);

							float wave = std::sin(fo * 2 * M_PI + phase) * 2.0f;
							int index = (y * 9 + x) * 3;

							// Map wave value to different color channels
							colors[index] = (std::sin(wave) + 1.0f) * 0.5f; // Red
							colors[index + 1] = (std::sin(wave + 2.0f * M_PI / 3.0f) + 1.0f) * 0.5f; // Green
							colors[index + 2] = (std::sin(wave + 4.0f * M_PI / 3.0f) + 1.0f) * 0.5f; // Blue
						}
					}
				}

				framework->color_buffer->update(&colors);

				gfx::clear(gfx::clear_buffer::Color | gfx::clear_buffer::Depth);

				framework->shader->bind();

				framework->vertices_buffer->bind_vertex(0, 3);
				framework->color_buffer->bind_vertex(1, 3);

				gfx::draw_arrays(0, 12 * 3);
			}
		}

		void update_camera(const frame::tick_event &event)
		{
			auto registry = event.registry;
			auto framework = static_cast<test_framework *>(event.data);

			auto entity_view = registry->view<movement>();

			auto camera_entity = registry->view<gfx::camera>().front();
			auto camera = registry->get<gfx::camera>(camera_entity);

			for (auto [entity, move] : entity_view.each())
			{
				camera.rotate(move.horizontalAngle, -move.verticalAngle);
				camera.move(move.position);

				auto projection = camera.get_projection(2560.0 / 1440.0);
				auto view = camera.get_view_matrix();

				if (ImGui::GetCurrentContext() == nullptr || !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
				{
					framework->dispatcher.trigger(poll_input_event { registry, framework });
				}

				glm::mat4 model = glm::mat4(1.0f);
				glm::mat4 mvp = (projection * view * model);

				glUniformMatrix4fv(framework->matrix_id, 1, false, &mvp[0][0]);
			}
		}

		void input(const poll_input_event &event)
		{
			auto registry = event.registry;
			auto entity_view = registry->view<movement>();

			auto framework = static_cast<test_framework *>(event.framework);

			if (framework->is_pressed(input::key::esc))
			{
				ImGui::SetWindowFocus();
				return; // we don't want to handle any of the other input (e.g. resetting the mouse)
			}

			auto [width, height] = framework->context->size();
			auto [xpos, ypos] = framework->context->get_mouse_pos();

			framework->context->reset_mouse();

			float mouseDeltaX = static_cast<float>(width / 2.0 - xpos);
			float mouseDeltaY = static_cast<float>(height / 2.0 - ypos);

			auto camera_entity = registry->view<gfx::camera>().front();
			auto camera = registry->get<gfx::camera>(camera_entity);

			for (auto [entity, move] : entity_view.each())
			{
				move.horizontalAngle += MOUSE_SPEED * framework->frame.deltaTime * mouseDeltaX;
				move.verticalAngle += MOUSE_SPEED * framework->frame.deltaTime * mouseDeltaY;

				std::map<input::key, glm::vec3> keyToDirection {
					{ input::key::w, camera.get_direction() },
					{ input::key::a, -camera.forward() },
					{ input::key::s, -camera.get_direction() },
					{ input::key::d, camera.forward() },
					{ input::key::spacebar, glm::vec3(0.0f, 1.0f, 0.0f) }, // Up
					{ input::key::shift, glm::vec3(0.0f, -1.0f, 0.0f) }	// Down
				};

				// Iterate over the input keys and perform the movement
				for (const auto &entry : keyToDirection)
				{
					if (framework->is_pressed(entry.first))
					{
						move.position += entry.second * MOVEMENT_SPEED * framework->frame.deltaTime;
					}
				}
			}
		}
	};

	void initialize() override
	{
		this->init_gui();

		context->input_mode(input::input_mode::StickyKeys, true);
		context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_HIDDEN);

		gfx::enable(gfx::enable_fields::CullFace);
		gfx::depth(gfx::depth_function::Less);

		gfx::clear_color({ 0.0, 0.1, 0.2, 0.0 });

		// this must be called first before making the buffer
		buffer::reserve_vertex_array(1);

		this->vertices_buffer = std::make_unique<buffer::buffer<std::array<float, 9 * 12>>>(&data, sizeof(std::array<float, 9 * 12>), buffer::draw_type::Static);
		this->color_buffer = std::make_unique<buffer::buffer<std::array<float, 9 * 12>>>(&colors, sizeof(std::array<float, 9 * 12>), buffer::draw_type::Stream);

		this->shader = std::make_unique<shader::shader>("shaders/simple.vert", "shaders/simple.frag");

		this->matrix_id = shader->get_uniform_location("mvp");

		registry.emplace<gfx::camera>(registry.create());
		registry.emplace<movement>(registry.create());

		listener listener;

		dispatcher.sink<frame::tick_event>().connect<&listener::update_gui>(listener);
		dispatcher.sink<frame::tick_event>().connect<&listener::update_camera>(listener);
		dispatcher.sink<frame::tick_event>().connect<&listener::tick>(listener);
		dispatcher.sink<poll_input_event>().connect<&listener::input>(listener);
	}
};

int main()
{
	gfx::context context("voxel", 2560, 1440);
	test_framework framework(&context);

	framework.run();
}
