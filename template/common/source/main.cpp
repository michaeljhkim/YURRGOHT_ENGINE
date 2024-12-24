#include "editor/editor.h"

int main(int argc, char** argv)
{
    Yurrgoht::Editor* editor = new Yurrgoht::Editor;
    editor->init();
    editor->run();
    editor->destroy();
    delete editor;
    
    return 0;
}