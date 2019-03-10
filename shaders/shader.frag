#version 450
#extension GL_ARB_separate_shader_objects : enable

// layout(binding = 1) uniform sampler2D texSampler;

// layout(location = 0) in vec3 fragColor;
// layout(location = 1) in vec2 fragTexCoord;

// layout(location = 0) out vec4 outColor;

// void main() {
//     outColor = texture(texSampler, fragTexCoord);
// }

// layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragToLight;
layout(location = 4) in vec3 fragToCamera;
// layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;



void main() {
    // vec3 nfragNormal = normalize(fragNormal);
    // vec3 nfragToLight = normalize(fragToLight);
    // vec3 nfragToCamera = normalize(fragToCamera);

    // // parameters of the light and possible values
    // vec3 lightAmbientIntensitys = vec3(0.3, 0.3, 0.3);
    // vec3 lightDiffuseIntensitys = vec3(1, 1, 1);
    // vec3 lightSpecularIntensitys = vec3(1, 1, 1);

    // // parameters of the material and possible values
    // vec3 matAmbientReflectances = vec3(1, 1, 1);
    // vec3 matDiffuseReflectances = vec3(1, 1, 1);
    // vec3 matSpecularReflectances = vec3(1, 1, 1);
    // float matShininess = 64*2*2;

    // // calculation as for Lambertian reflection
    // float diffuseTerm = clamp(dot(nfragNormal, nfragToLight), 0, 1);
    // vec3 diffuseLighting =  matDiffuseReflectances * lightDiffuseIntensitys * diffuseTerm;

    // // calculate specular reflection only if
    // // the surface is oriented to the light source
    // float specularTerm = 0;
    // if(dot(nfragNormal, nfragToLight) > 0)
    // {
    //     // half vector
    //     vec3 H = normalize(nfragToLight + nfragToCamera);
    //     specularTerm = pow(dot(nfragNormal, H), matShininess);
    // }
    // vec3 specularLighting = matSpecularReflectances * lightSpecularIntensitys * specularTerm;

    // outColor = vec4(fragColor*(diffuseLighting+specularLighting),1); //  vec4(fragColor,1)//  = vec4(0.5,0.3,0.3,1); //texture(texSampler, fragTexCoord);
    outColor = vec4(0.3,0.5,0.7,1);
}