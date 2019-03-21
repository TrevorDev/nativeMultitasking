#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define UNUSED(expr) do { (void)(expr); } while (0)

struct WindowManagerFrameBufferSize {
	uint32_t width;
	uint32_t height;
};

class WindowManager {
public:
	std::vector<bool> keys = {};
	bool framebufferResized = false;

	double lastCursorPosDifX = 0;
	double lastCursorPosDifY = 0;
	double lastCursorPosX = 0;
	double lastCursorPosY = 0;

	bool mouseDown = false;

	int currentWidth = 0;
	int currentHeight = 0;

	WindowManager() {
	}
	void init(uint32_t width, uint32_t height){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		this->window = glfwCreateWindow(width, height, "3D Multitasking", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		// glfwSetCharCallback(window, &(this->character_callback));
		glfwSetWindowUserPointer(this->window, this);
		
		this->keys.resize(400);
		for(auto i = 0;i<400;i++){
			keys[i] = false;
		}

		GLFWkeyfun keyCallback = [](GLFWwindow* w, int key, int scancode, int action, int mods)
		{
			UNUSED(scancode);
			UNUSED(mods);
			WindowManager* t = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
			t->keys[key] = (action) ? true : false;
		};
		glfwSetKeyCallback(window, keyCallback);

		GLFWcursorposfun cursorCallback = [](GLFWwindow* w, double x, double y)
		{
			WindowManager* t = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
			t->lastCursorPosDifX = x - t->lastCursorPosX;
			t->lastCursorPosDifY = y - t->lastCursorPosY;
			t->lastCursorPosX = x;
			t->lastCursorPosY = y;
		};
		glfwSetCursorPosCallback(window, cursorCallback);

		GLFWmousebuttonfun cursorButtonCallback = [](GLFWwindow* w, int button, int action, int mods)
		{
			UNUSED(mods);
			WindowManager* t = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				if(action == GLFW_PRESS){
					t->mouseDown = true;
				}else{
					t->mouseDown = false;
				}
			}
		};
		glfwSetMouseButtonCallback(window, cursorButtonCallback);
	}
	vk::SurfaceKHR createSurface(vk::Instance& instance){
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(instance, this->window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		return vk::SurfaceKHR(surface);
	}
	WindowManagerFrameBufferSize getFramebufferSize() {
		WindowManagerFrameBufferSize result;
		glfwGetFramebufferSize(this->window, (int*)&result.width, (int*)&result.height);
		return result;
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
		this->lastCursorPosDifX = 0;
    	this->lastCursorPosDifY = 0;
		glfwPollEvents();
	}

	// Waits until frame buffer has a width and height
	void waitForFrameBuffer(){
		int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
	}

	
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
		app->currentWidth = width;
		app->currentHeight = height;
		
		app->framebufferResized = true;
	}

	// static void character_callback(GLFWwindow* window, unsigned int codepoint)
	// {
	// 	//jlog(codepoint);
	// }
	// //typedef void (*GLFWkeyfun)(GLFWwindow *, int, int, int, int)
	// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	// {
	// 	//keys[key] = action == GLFW_KEY_DOWN ? true : false;
	// 	//jlog(key);
	// }
	GLFWwindow * window;
};