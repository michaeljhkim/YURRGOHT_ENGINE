#pragma once
/*
For learning more about angelscript:
https://www.angelcode.com/

Download:
https://www.angelcode.com/angelscript/downloads.html

https://github.com/codecat/angelscript-mirror


WHY CHOOSE ANGELSCRIPT:
- Familiar language in the unlikely scenario that someone decides to use this engine
- Easy to allow interaction with compiled C++ code
- It is fast (relative for garbage collecting)
- Uses C-based syntax
- Portable
- implemented for fun and as a personal challenge

WHY ANGELSCRIPT AS A SCRIPTING LANGUAGE:
- I was a big doom modder around 2020-2021, and I wanted to give a similar experience to people
- It should theoretically be most of the benefits of C++, with no auxillary compile tool required for users
- Trade off is longer startup time, but I think that's worthwhile, especially heavy computation SHOULD be done in compiled C++
*/

#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warnings about truncated symbol names
#endif

#include <iostream>  // cout
#include <assert.h>  // assert()
#include <string.h>  // strstr()
#ifdef __linux__
	#include <sys/time.h>
	#include <stdio.h>
	#include <termios.h>
	#include <unistd.h>
#else
	#include <conio.h>   // kbhit(), getch()
	#include <windows.h> // timeGetTime()
#endif
#include <set>
#include <map>

#include <angelscript.h>
#include "angelscript/sdk/add_on/scriptstdstring/scriptstdstring.h"
#include "angelscript/sdk/add_on/scriptbuilder/scriptbuilder.h"

#include "engine/core/base/macro.h"


namespace Yurrgoht {

	class ScriptManager {
	public:
		#ifdef __linux__
		#define UINT unsigned int 
		typedef unsigned int DWORD;
		
		/*
		- Linux doesn't have timeGetTime(), this essentially does the same thing 
		- Except this is milliseconds since Epoch (Jan 1st 1970) instead of system start
		- It will work the same though...
		*/
		static DWORD timeGetTime() {
			timeval time;
			gettimeofday(&time, NULL);
			return time.tv_sec*1000 + time.tv_usec/1000;
		}
		
		/*
		- Linux does have a getch() function in the curses library, but it doesn't work like it does on DOS
		- This does the same thing, without the need of the curses library.
		*/
		int getch() {
			struct termios oldt, newt;
			int ch;
		
			tcgetattr(STDIN_FILENO, &oldt);
			newt = oldt;
			newt.c_lflag &= ~( ICANON | ECHO );
			tcsetattr( STDIN_FILENO, TCSANOW, &newt );
		
			ch = getchar();
		
			tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
			return ch;
		}
		#endif

        void init();
        void destroy();
        int assert_destroy();

        // Function prototypes
        static void MessageCallback(const asSMessageInfo *msg, void *param);

		// Function implementation with native calling convention
		static void PrintString(std::string &str) { std::cout << str; }

    	// Function implementation with generic script interface
		static void PrintString_Generic(asIScriptGeneric *gen) { std::cout << (std::string*)gen->GetArgAddress(0); }
		
		asIScriptEngine* getScriptEngine() { return m_script_engine; };
		asIScriptContext* getScriptContext() { return m_script_context; };

        void ConfigureEngine();
		int StartModuleIfAbsent(const std::string& module_name);
        int AddScriptToModule(const std::string& module_name, const std::string& script_path);
        int BuildScriptModule(const std::string& module_name);

        //void timeGetTime_Generic(asIScriptGeneric *gen);
		int AddFunction(const std::string& module, const std::string& function_name);
		int PrepareFunction(const std::string& module, const std::string& function_name);
		void ExecuteFunction();

		/*
		- Instead of aborting, the script can be suspended and later resumed by calling Execute() again.
		- If the time out is reached we abort the script
		*/
		static void LineCallback(asIScriptContext* script_context, DWORD* timeOut) {
			if(*timeOut < timeGetTime())
				script_context->Abort();
		}

    private:
		asIScriptEngine* m_script_engine;
        asIScriptContext* m_script_context;
		asITypeInfo* m_script_type;
		asIScriptObject* m_script_object;

		std::map<std::string, asIScriptFunction*> m_script_functions;

        // The builder loads the script file and any included files specified by #include directives
		// the key is the module that the builder is defined by
		std::map<std::string, CScriptBuilder> m_script_builders;
    };

}
