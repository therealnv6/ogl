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
#include <info.hpp>
#include <render.hpp>
#include <shader.hpp>
#include <spdlog/spdlog.h>
#include <vertices.h>
#include <voxel/grid.hpp>
#include <voxel/ray.hpp>
#include <voxel/tracing.hpp>
#include <window.hpp>

const static float MOVEMENT_SPEED = 30.0f;
const static float MOUSE_SPEED = 1.5f;

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 16;
constexpr int CHUNK_DEPTH = 16;

struct movement {
	glm::vec3 position = glm::vec3(0, 0, 5);

	float horizontalAngle = 0.0f;
	float verticalAngle = 0.0f;
};

struct poll_input_event {
	entt::registry *registry;
	frame::framework *framework;
};

class test_framework : public frame::framework
{
public:
	buffer::unique_buffer vertices_buffer;
	buffer::unique_buffer color_buffer;

	std::unique_ptr<shader::shader> shader;

	test_framework(gfx::context *context)
		: frame::framework(context)
	{
		this->initialize();
	}

	struct listener {
		void update_gui(const frame::tick_event &event)
		{
			event.data->gui_frame();

			auto registry = event.registry;
			auto view = registry->view<movement>();

			frame::frame_history history = event.data->frame.frameHistory;

			for (auto [entity, movement] : view.each())
			{
				ImGuiIO &io = ImGui::GetIO();
				(void) io;

				float total_mem_gb = info::get_memory(memory_type::available_memory, memory_scale::gigabytes);
				float total_mem_mb = info::get_memory(memory_type::available_memory, memory_scale::megabytes);
				float cur_avail_mem_mb = info::get_memory(memory_type::free_space, memory_scale::megabytes);

				ImGui::Begin("ogl voxel");

				if (ImGui::BeginTabBar(""))
				{
					if (ImGui::BeginTabItem("Debug"))
					{
						ImGui::PlotLines("", history.frames.data(), history.max_frames);
						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
						ImGui::Text("Memory usage %.2f/%.3f MB", total_mem_mb - cur_avail_mem_mb, total_mem_gb);
						ImGui::Text("horizontalAngle: %.3f", movement.horizontalAngle);
						ImGui::SameLine();
						ImGui::Text("verticalAngle: %.3f", movement.verticalAngle);

						ImGui::InputFloat("= x", &movement.position.x);
						ImGui::InputFloat("= y", &movement.position.y);
						ImGui::InputFloat("= z", &movement.position.z);

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

			auto camera_entity = registry->view<gfx::camera>().front();
			auto camera = registry->get<gfx::camera>(camera_entity);

			auto grid_entity = registry->view<voxel::grid<CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH>>().front();
			auto grid = registry->get<voxel::grid<CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH>>(grid_entity);

			gfx::clear(gfx::clear_buffer::Color | gfx::clear_buffer::Depth);

			framework->shader->bind();

			framework->vertices_buffer->bind_vertex(0, 3);
			framework->color_buffer->bind_vertex(1, 3);

			gfx::draw_arrays(0, 12 * 3);

			{
				ray::raycast cast(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));

				// deltas and steps, these are used for the raycasting
				glm::vec3 deltas(1.0f, 1.0f, 1.0f);
				glm::vec3 steps(1.0f, 1.0f, 1.0f);

				// Iterate over each voxel position in the chunk
				for (int x = 0; x < CHUNK_WIDTH; ++x)
				{
					for (int y = 0; y < CHUNK_HEIGHT; ++y)
					{
						for (int z = 0; z < CHUNK_DEPTH; ++z)
						{
							cast.set_origin(glm::vec3(x, y, z));

							auto voxel = ray::trace_ray(cast, grid, deltas, steps);

							if (voxel != std::nullopt)
							{
								spdlog::info("rendering voxel at {}, {}, {}",
									voxel.value().position.x,
									voxel.value().position.y,
									voxel.value().position.z);
							}
						}
					}
				}
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
					framework->dispatcher.trigger(
						poll_input_event { registry, framework } // we could probably make a single poll_input_event instead of making a new one.
					);
				}

				glm::mat4 model = glm::mat4(1.0f);
				glm::mat4 mvp = (projection * view * model);

				framework->shader->bind_mat4("mvp", mvp, false);
			}
		}

		void input(const poll_input_event &event)
		{
			auto registry = event.registry;
			auto entity_view = registry->view<movement>();

			auto framework = static_cast<test_framework *>(event.framework);

			if (framework->is_pressed(input::key::esc))
			{
				return; // we don't want to handle any of the other input (e.g. resetting the mouse)
			}

			static double i = 0.0;

			if (i < 0.5)
			{
				i += 0.1;
				return;
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

				camera.rotate(move.horizontalAngle, -move.verticalAngle);

				// yikes (should change this sometime)
				glm::vec3 direction = camera.get_direction();
				glm::vec3 right = glm::normalize(glm::cross(direction, camera.get_up()));

				// Define the key-to-direction mapping
				std::map<input::key, glm::vec3> keyToDirection {
					{ input::key::w, direction }, // Forwards
					{ input::key::a, -right }, // Left
					{ input::key::s, -direction }, // Backwards
					{ input::key::d, right }, // Right
					{ input::key::spacebar, glm::vec3(0.0f, 1.0f, 0.0f) }, // Up
					{ input::key::shift, glm::vec3(0.0f, -1.0f, 0.0f) }	// Down
				};

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
		{
			context->input_mode(input::input_mode::StickyKeys, true);
			context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_DISABLED);
		}

		{
			gfx::enable(gfx::enable_fields::CullFace);
			gfx::depth(gfx::depth_function::Less);

			gfx::clear_color({ 0.0, 0.1, 0.2, 0.0 });

			buffer::reserve_vertex_array(1);
		}

		{
			vertices_buffer = std::make_unique<buffer::buffer>(&data, sizeof(data), draw_type::Static);
			color_buffer = std::make_unique<buffer::buffer>(&colors, sizeof(colors), draw_type::Static);
		}

		shader = std::make_unique<shader::shader>("shaders/simple.vert", "shaders/simple.frag");

		voxel::grid<CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH> grid(
			glm::vec3(1.0, 1.0, 1.0),
			glm::vec3(1.0, 1.0, 1.0),
			glm::vec3(1.0, 1.0, 1.0));

		grid.set_voxel_at(glm::vec3(3.0, 3.0, 3.0), glm::zero<glm::vec3>());

		registry.emplace<voxel::grid<CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH>>(registry.create(), grid);

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
