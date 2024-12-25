#include <iostream>
#include "editor/editor.h"


int main()
{
    std::cout << "Hello World!" << std::endl;
    Yurrgoht::Editor* editor = new Yurrgoht::Editor;
    editor->init();
    editor->run();
    editor->destroy();
    delete editor;
    
    return 0;
}