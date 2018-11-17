import glsl from "./nodeSrc/glslangValidator"
import * as ipc from "node-ipc"
var app = require('./nativeSrc/build/Release/app');

const { fork } = require('child_process');


var main = async ()=>{
    console.log("Compiling shaders ------------------------------------------------------")
    await glsl.compileShader("shaders/shader.vert", "shaders/vert.spv")
    await glsl.compileShader("shaders/shader.frag", "shaders/frag.spv")
    console.log("Launching app ------------------------------------------------------")
    
    app.init();

    while(!app.shouldClose()){ // This is a blocking call
        app.render();
    }
    
    console.log("Close success ------------------------------------------------------")
    
}
main().catch((e)=>{
    console.error(e)
})




