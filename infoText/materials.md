Desired descriptor layouts:
    scene data
        lights:
            pointLights
            spotlights
            ambientLights
        environmentMap?

    material data
        color? (or use vertex color)
        bumpmapTexture
        opacityTexture
        diffuseTexture
        emissiveTexture

    instanceData
        UBO
            mat4 model;
            mat4 view;
            mat4 proj;
            mat3 normal // http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/, https://stackoverflow.com/questions/21079623/how-to-calculate-the-normal-matrix
        vec3 camPos;

    blinn phong shader: https://web.archive.org/web/20160318154627/www.sunandblackcat.com/tipFullView.php?l=eng&topicid=30



// How to initialize a descriptor layout
Create descriptor layout
Create render pipeline
Create descriptor pool
Create descriptor sets (memory for an instance of the layout)

// How to update descriptor set memory
Just memcopy cpu memory to the vulkan buffer
eg. memcpy(uniformBuffer.mapped, &ubo, sizeof(ubo));

// How to draw with a descriptor set
vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

// Multiple descriptor sets
As some of the structures and function calls hinted at, it is actually possible to bind multiple descriptor sets simultaneously. You need to specify a descriptor layout for each descriptor set when creating the pipeline layout. Shaders can then reference specific descriptor sets like this:
layout(set = 0, binding = 0) uniform UniformBufferObject { ... }
You can use this feature to put descriptors that vary per-object and descriptors that are shared into separate descriptor sets. In that case you avoid rebinding most of the descriptors across draw calls which is potentially more efficient.