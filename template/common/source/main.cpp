#include "editor/editor.h"

int main()
{
    Yurrgoht::Editor* editor = new Yurrgoht::Editor;
    editor->init();
    editor->run();
    editor->destroy();
    delete editor;
    
    return 0;
}