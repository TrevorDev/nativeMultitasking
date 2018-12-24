#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (constant_id = 0) const int NUM_LIGHTS = 1;
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

struct PointLightUniformBufferObject {
	vec4 position;
	vec3 color;
	float radius;
};

layout(binding = 1) uniform PointLightsUniformBufferObject {
    PointLightUniformBufferObject lights[NUM_LIGHTS];
} pointLights;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragColor = vec3(pointLights.lights[0].color.x,pointLights.lights[0].color.y,pointLights.lights[0].color.z);//vec3(pointLights.lights[0].radius,0,0);///pointLights.lights[0].color;//vec3(0,0,.x);
    fragTexCoord = inTexCoord;
}