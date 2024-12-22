#include "editor/editor.h"
#include <iostream>

int main()
{
    Yurrgoht::Editor* editor = new Yurrgoht::Editor;
    editor->init();     //Segfault occurs here
    std::cout << "Seg fault here?" << std::endl;
    editor->run();
    editor->destroy();
    delete editor;
    
    return 0;
}