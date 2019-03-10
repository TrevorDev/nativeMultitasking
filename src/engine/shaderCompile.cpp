#include "../engine/vulkanInc.hpp"
#include "../j.hpp"
#include <sstream> 

void compileShader(const char* inputFile, const char* outputFile){
    std::ostringstream s;
    s << "glslangValidator.exe -V " << inputFile << " -o " << outputFile;
    jlog("Compiling shader (may fail silently)");
    jlog(s.str());
    system(s.str().c_str());
}

