// Supproted platform
if(process.platform != "win32"){
    throw "Platfrom not supported. (win32 only)"
}

// Configuration
var buildConfig = {
    compilerCmd: "clang-cl",
    srcFile: "main.cpp",
    includeDirs: [
        "external/glfw/include",
        "external",
        "$(VULKAN_SDK)/Include"
    ],
    linkLibs: [
        "external/glfw/lib-vc2015/glfw3dll.lib",
        "external/openvr/lib/win64/openvr_api.lib",
        "$(VULKAN_SDK)/Lib/vulkan-1.lib"
    ],
    options: [
        // Enable exceptions:
        "/EHa",
        // Wall check eg. unused variables etc.
        //"/W4",
        // Disable optimizations:
        "/Od",
        
        // Dr memory debug recomended options https://drmemory.org/docs/page_running.html
        "/Zi",
        "/MT",
        "/EHsc",
        "/Oy-",
        "/Ob0"
        //
    ],
    stdVersion: "c++17"
}

// Replace environment variables with path eg. $(VULKAN_SDK)
var replaceWithEnvVar = (str)=>{
    var regex = /\$\((.*?)\)/g
    var match;
    do{
        match = regex.exec(str)
        if(match){
            if(!process.env[match[1]]){
                throw "Env variable "+match[1]+" not found"
            }
            str = str.replace(match[0], process.env[match[1]])
        }
    }while(match)
    return str;
}
buildConfig.includeDirs = buildConfig.includeDirs.map((str)=>{return replaceWithEnvVar(str)})
buildConfig.linkLibs = buildConfig.linkLibs.map((str)=>{return replaceWithEnvVar(str)})


// Build commands
var main = async()=>{
    var cmd = process.argv[2] || "build"
    if(cmd == "build"){
        var buildCmd = (
            buildConfig.compilerCmd+" "+
            buildConfig.srcFile+" "+
            buildConfig.linkLibs.join(" ")+
            " /I "+buildConfig.includeDirs.join(" /I ")+
            " "+buildConfig.options.join(" ")+" -Xclang -std="+buildConfig.stdVersion
        )
        console.log(">: "+buildCmd)
        const util = require('util');
        try{
            const exec = util.promisify(require('child_process').exec);
            const { stdout, stderr } = await exec(buildCmd);
            console.log(stdout);
            console.log(stderr);
        }catch(e){
            console.log('\x1b[33m%s\x1b[0m', e);
        }
        
        
    }else if(cmd == "generateAutoCompleteFile"){
        const fs = require('fs');
        var includes = "-I"+buildConfig.includeDirs.join("\n-I")+"\n-std="+buildConfig.stdVersion

        fs.writeFile("compile_flags.txt", includes, function(err) {
            if(err) {
                return console.log(err);
            }
            console.log(includes)
            console.log("Was written to compile_flags.txt, restart vs code for it to take effect");
        }); 
    }else if(cmd == "--help"){
        console.log("build")
        console.log("generateAutoCompleteFile")
    }
}
main();
