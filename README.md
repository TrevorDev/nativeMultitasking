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
Header only vulkan lib with the goal of one day support vr/ar multitasking cleanly

### Setup:
  - Install vulkan, nodejs, clang
  - Run: node build.js && main.exe
  - Tested only on Windows so far
  - good luck since its unlikely to work first build and this is very unstable atm ¯\\_(ツ)_/¯ 

### Dependencies
  - nodejs https://nodejs.org/en/
  - LLVM/Clang http://releases.llvm.org/download.html
  - Vulkan

### Goals:
  - Provide a fast renderer that is easy to use with Node
  - Build the multi-tasking vr/ar environment that needs to exist 
  - Load and run a very small subset of browser/html features
  - Learn about low level graphics
  - Improve native development skills
  - Best development platform in the universe

### Editor Setup:
 - VSCode https://code.visualstudio.com/
 - vscode-clangd https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd
 - To get includes for autocomplete run:
   - node build.js generateAutoCompleteFile

```
Created by:
_____ _ ___ _       _____ _     _   
|   | |_|  _| |_ _ _|  |  |_|___| |_ 
| | | | |  _|  _| | |    -| |  _| '_|
|_|___|_|_| |_| |_  |__|__|_|___|_,_|
                |___|    
This is not a website, it's a movement
```            