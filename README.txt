BUILD:
 - node-gyp configure build

Installing dependencies
 - GLFW: 
   - Download from https://www.glfw.org/download.html
   - move dll to root
   - add path to include_dirs and libraries to binding.gyp

Include paths:
"includePath": [
  "${workspaceFolder}/**",
  "C:/Users/trevo/workspace/nodeAddon/externalLibs/glfw-3.2.1.bin.WIN64/glfw-3.2.1.bin.WIN64/include",
  "C:/Users/trevo/.node-gyp/8.11.3/include/node"
],