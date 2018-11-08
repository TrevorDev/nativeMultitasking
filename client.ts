import glsl from "./nodeSrc/glslangValidator"
import * as ipc from "node-ipc"
var app = require('./nativeSrc/build/Release/app');

console.log("START CLIENT -----------------------")

ipc.config.id = 'a-unique-process-name2';
ipc.config.retry = 1500;
ipc.config.silent = true;
ipc.connectTo('compositor', () => {
    //console.log(ipc.of)
    ipc.of['compositor'].on('connect', () => {
        ipc.of['compositor'].emit('requestImgHandle', ipc.config.id );
    });
    ipc.of['compositor'].on("imgHandle", (data:any)=>{
        var handle = parseInt(data.handle)
        app.init("");
        console.log("Client init handle: "+handle);
        app.init2(handle);
        console.log("Client done init handle")
    })
});