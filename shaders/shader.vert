#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (constant_id = 0) const int NUM_LIGHTS = 1;
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camPos;
} ubo;

// struct PointLightUniformBufferObject {
// 	vec4 position;
// 	vec3 color;
// 	float radius;
// };

// layout(binding = 1) uniform PointLightsUniformBufferObject {
//     PointLightUniformBufferObject lights[NUM_LIGHTS];
// } pointLights;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragToLight;
layout(location = 4) out vec3 fragToCamera;

// out vec3 o_normal;
// out vec3 o_toLight;
// out vec3 o_toCamera;
// out vec2 o_texcoords;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    
    fragColor = inColor;
    //fragColor = ubo.camPos.xyz;//vec3(pointLights.lights[0].color.x,pointLights.lights[0].color.y,pointLights.lights[0].color.z);//vec3(pointLights.lights[0].radius,0,0);///pointLights.lights[0].color;//vec3(0,0,.x);

    fragTexCoord = inTexCoord;
    
    fragNormal = inNormal;
    
    //fragToLight	= normalize(pointLights.lights[0].position.xyz - worldPos.xyz);

    fragToCamera = normalize(ubo.camPos.xyz - worldPos.xyz);
    
    gl_Position = ubo.proj * ubo.view * worldPos;
}