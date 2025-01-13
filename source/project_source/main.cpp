#include <iostream>
#include "editor/editor.h"
//#include "engine/engine.h"

/*
NOTE: Log output for terminal and file will be a little different

- The terminal log output is synchronous, and file log output is synchronous
- Done for efficiency reasons
- IF NESSECARY, OPEN log_system AND CHANGE THAT SECTION
*/


int main() {
    Yurrgoht::Editor* editor = new Yurrgoht::Editor;
    editor->init();
    editor->run();
    editor->destroy();
    delete editor;
    
    return 0;
}