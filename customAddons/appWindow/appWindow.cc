// hello.cc using N-API
#include <node_api.h>
//#include <napi.h>
#include "test.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace demo {
    VkInstance instance;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
    };
    auto enableValidationLayers = true;
    std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

    napi_value Method(napi_env env, napi_callback_info args) {

        VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}



        napi_value greeting;
        napi_status status;
        glfwInit();
        auto str = glfwGetVersionString();
        status = napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &greeting);
        if (status != napi_ok) return nullptr;
        return greeting;
    }

    napi_value init(napi_env env, napi_value exports) {
        napi_status status;
        napi_value fn;

        status = napi_create_function(env, nullptr, 0, Method, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "hello", fn);
        if (status != napi_ok) return nullptr;
        return exports;
    }

    NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

}  // namespace demo