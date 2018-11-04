import {spawn} from "child_process"

export default {
    compileShader: (inputFile:string, outputFile:string)=>{
        return new Promise((res, rej)=>{
            const command = spawn('glslangValidator.exe -V '+inputFile+" -o "+outputFile, [], { shell: true })
            command.stdout.on('data', (data) => {
                console.log(`stdout: ${data}`)
                if((data as string).indexOf("ERROR") != -1){
                    rej("Shader compile failed")
                }
            })
            command.stderr.on('data', (data) => {
                console.log(`stderr: ${data}`)
                rej("Shader compile failed")
            })
            command.on('close', (code) => {
                res()
            })
        })
    }
}