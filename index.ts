import glsl from "./nodeSrc/glslangValidator"
import * as ipc from "node-ipc"
var app = require('./nativeSrc/build/Release/app');

const { fork } = require('child_process');


// var main = async ()=>{
//     console.log("Compiling shaders ------------------------------------------------------")
//     await glsl.compileShader("shaders/shader.vert", "shaders/vert.spv")
//     await glsl.compileShader("shaders/shader.frag", "shaders/frag.spv")
//     console.log("Launching app ------------------------------------------------------")
//     app.init("Hello World")
//     app.render("Hello World")
// }
// main().catch((e)=>{
//     console.error(e)
// })

var main = async ()=>{
    console.log("Compiling shaders ------------------------------------------------------")
    await glsl.compileShader("shaders/shader.vert", "shaders/vert.spv")
    await glsl.compileShader("shaders/shader.frag", "shaders/frag.spv")
    console.log("Launching SERVER ------------------------------------------------------")
    var handle = app.init("");
    //app.init2(handle);
    
    console.log("server handle: "+handle)

    ipc.config.id = 'compositor';
    ipc.config.retry = 1500;
    ipc.config.silent = true;
    ipc.serve(() => {
        ipc.server.on('requestImgHandle', (message:string, socket) => {
            ipc.server.emit(socket, "imgHandle", {handle: handle})
            app.render("")
        })
    });
    ipc.server.start();

    const command = fork('client.ts');
}
main().catch((e)=>{
    console.error(e)
})


