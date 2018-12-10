```
                                       _ ._  _ , _ ._
                                     (_ ' ( `  )_  .__)
  /\                               ( (  (    )   `)  ) _)
 /  \  __     ___   _ _     _  __  (__ (_   (_ . _) _) ,__)
 |  |  \ \   / / | | | |   | |/ /      `~~`\ ' . /`~~`
 |  |   \ \ / /| | | | |   | ' /            ;   ;
/ == \   \ V / | |_| | |___| . \            /   \ 
|/**\|    \_/   \___/|_____|_|\_\__________/_ __ \_____________

This is not a rendering engine, it's a movement
```

### Setup:
  - Install nodejs
  - Run from admin cmd (Takes like 30 min do not quit):
    - npm install --global --production windows-build-tools
  - npm start
  - good luck since its unlikely to work first build and this is very unstable atm ¯\_(ツ)_/¯

### Dependencies
  - Windows
  - Visual Studio 2017
  - Vulkan

### Goals:
  - Provide a fast renderer that is easy to use with Node
  - Build the multi-tasking vr/ar environment that needs to exist 
  - Load and run a very small subset of browser/html features
  - Learn about low level graphics
  - Improve native development skills
  - Best development platform in the universe

### Include path for IDE:
```
"includePath": [
    "${workspaceFolder}/nativeSrc/app",
    "${workspaceFolder}/nativeSrc/app/engine",
    "${workspaceFolder}/nativeSrc/app/math",
    "${workspaceFolder}/nativeSrc/app/object3d",
    "${workspaceFolder}/node_modules/node-addon-api",
    "${workspaceFolder}/node_modules/node-addon-api/external-napi",
    "${workspaceFolder}/nativeSrc/external/glfw/include",
    "${workspaceFolder}/nativeSrc/external",
    "${workspaceFolder}/nativeSrc/external/openvr/headers",
    "D:/Programs/vulkan/1.1.85.0/Include"
],
```

```
Created by:
_____ _ ___ _       _____ _     _   
|   | |_|  _| |_ _ _|  |  |_|___| |_ 
| | | | |  _|  _| | |    -| |  _| '_|
|_|___|_|_| |_| |_  |__|__|_|___|_,_|
                |___|    
This is not a website, it's a movement
```            


