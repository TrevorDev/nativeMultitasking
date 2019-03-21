var Vulk:any;

// Initialize window and device
var renderWindow = Vulk.createRenderWindow(800, 600);
var gpuDevice = Vulk.createDeviceForSurface(renderWindow.surface);

// Create pipeline
var sceneDescriptorSetLayout = Vulk.createDescriptorSetLayout();
sceneDescriptorSetLayout.describeBinding(0, Vulk.DescriptorSetLayout.UniformBuffer)

var meshDescriptorSetLayout = Vulk.createDescriptorSetLayout();
meshDescriptorSetLayout.describeBinding(0, Vulk.DescriptorSetLayout.UniformBuffer)

var standardShaderPipeline = new Vulk.ShaderPipeline()
standardShaderPipeline.fragmentShader = Vulk.LoadShader("shader.frag")
standardShaderPipeline.vertexShader = Vulk.LoadShader("shader.vert")
standardShaderPipeline.descriptorSetLayouts = [sceneDescriptorSetLayout, meshDescriptorSetLayout]

// Create scene
var scene = new Vulk.DescriptorSet(sceneDescriptorSetLayout)
scene.setBindingBufferDescription(0, {
    view: "mat4",
    proj: "mat4",
    camPos: "vec3",
    pointLights: Vulk.BufferDescriptionArray(
        Vulk.BufferDescription({
            pos: "vec3"
        }),
        6
    )
})
scene.init(sceneDescriptorSetLayout);


// Create mesh
var mesh = new Vulk.Mesh();
mesh.uniformbufferDescription = {model: "mat4"}
mesh.vertexBuffer = [
    // Lookup done in shader, layout (1,2,3)
    // Possition           // Color             // Normal           // Texture coord
    -0.5, -0.5, 0.0,       0.05, 0.8, 0.8,      0.0, 0.0, 1.0,      1.0, 0.0,      
    0.5, -0.5, 0.0,        0.05, 0.8, 0.8,      0.0, 0.0, 1.0,      0.0, 0.0,      
    0.5, 0.5, 0.0,         0.05, 0.8, 0.8,      0.0, 0.0, 1.0,      0.0, 1.0      
    -0.5, 0.5, 0.0,        0.05, 0.8, 0.8,      0.0, 0.0, 1.0,      1.0, 1.0
]
mesh.indexBuffer = [0, 1, 2, 2, 3, 0]
mesh.init(gpuDevice, meshDescriptorSetLayout);
var meshes = [mesh]

// Create gpu program description
var renderDescription = new Vulk.RenderDescription(gpuDevice);
renderDescription.commands = [
    Vulk.Commands.Clear(),
    Vulk.Commands.BindPipeline(standardShaderPipeline),
    Vulk.Commands.BindDescriptorSet(scene),
    Vulk.Commands.DrawRenderables(meshes),
]



/**
 * CommandName: MainRender
 * targetImage: offscreen
 * Clear
 * BindStandardShaderPipeline
 * BindSceneDescriptorSet (eg. Camera, lights)
 * Render Meshes
 *  BindIndexAndVertexBuffers
 *  BindMeshDescriptorSet (eg. uniformbuffer, etc)
 *  DrawMesh
 * Done
 * 
 * CommandName: DepthMerge
 * targetImage: swapchainImage
 * Clear
 * WaitFor: MainRender, MainRender2
 * BindDepthMergeShaderPipeline
 * Draw PostProcess Quad
 * Done
 * Present swapchainImage
 */