import glsl from "./nodeSrc/glslangValidator"
var app = require('./nativeSrc/build/Release/app');

var main = async ()=>{
    console.log("Compiling shaders ------------------------------------------------------")
    await glsl.compileShader("shaders/shader.vert", "shaders/vert.spv")
    await glsl.compileShader("shaders/shader.frag", "shaders/frag.spv")
    console.log("Launching app ------------------------------------------------------")
    app.run("Hello World")
}
main().catch((e)=>{
    console.error(e)
})


