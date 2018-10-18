#pragma once

class AppWindow {
public:
	AppWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		this->window = glfwCreateWindow(WIDTH, HEIGHT, "3D Multitasking", nullptr, nullptr);

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		
	}
	bool framebufferResized = false;
	VkSurfaceKHR createSurface(VkInstance instance){
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(instance, this->window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		return surface;
	}
	void getFramebufferSize(int* width, int* height) {
		glfwGetFramebufferSize(this->window, width, height);
	}
	bool shouldClose() {
		return glfwWindowShouldClose(this->window);
	}
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<AppWindow*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
	GLFWwindow * window;
};