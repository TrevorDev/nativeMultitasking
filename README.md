Dependencies:
nodejs https://nodejs.org/en/
LLVM/Clang http://releases.llvm.org/download.html

Editor Setup:
VSCode https://code.visualstudio.com/
cscode-clangd https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd
To get includes for autocomplete run:
node build.js generateAutoCompleteFile

Run command:
node build.js && main.exe