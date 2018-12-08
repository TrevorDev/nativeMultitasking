#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class WindowManager {
public:
	WindowManager(int width, int height) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		this->window = glfwCreateWindow(width, height, "3D Multitasking", nullptr, nullptr);

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
	bool framebufferResized = false;
	vk::SurfaceKHR createSurface(vk::Instance instance){
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(instance, this->window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		return vk::SurfaceKHR(surface);
	}
	void getFramebufferSize(int* width, int* height) {
		glfwGetFramebufferSize(this->window, width, height);
	}
	void getRequiredInstanceExtensions(std::vector<std::string>& vec){
		uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		for(auto& ext:extensions){
			vec.push_back(ext);
		}
	}
	bool shouldClose() {
		return glfwWindowShouldClose(this->window);
	}
	void update() {
		glfwPollEvents();
	}
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
	GLFWwindow * window;
};