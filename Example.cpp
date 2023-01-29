/*
 
 EXAMPLE
 
 ConsoleStyle - Colors and styles for your Terminal
 
 ***********************************************************************************
 The zlib License

 Copyright ©2023 Marc Schöndorf

 This software is provided 'as-is', without any express or implied warranty. In
 no event will the authors be held liable for any damages arising from the use of
 this software.

 Permission is granted to anyone to use this software for any purpose, including
 commercial applications, and to alter it and redistribute it freely, subject to
 the following restrictions:

 1.  The origin of this software must not be misrepresented; you must not claim
     that you wrote the original software. If you use this software in a product,
     an acknowledgment in the product documentation would be appreciated but is
     not required.

 2.  Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

 3.  This notice may not be removed or altered from any source distribution.
 ***********************************************************************************
 
*/

// Compile: g++ -std=c++20 -o Example Example.cpp -fsanitize=address -static-libsan -g

#include <iostream>

// Include the header. Nothing else is required to use the lib
#include "ConsoleStyle.h"

using namespace cstyle;
//using namespace ConsoleStyle; // Equivalent to cstyle

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl << "Env: " << std::getenv("TERM") << std::endl;
    
    // Simple example
    std::cout << BG::red << "Attribute" << BG::reset << std::endl;
    
    // Modifier example
    Modifier modifier(FG::red_b, BG::blue, STYLE::strike);
    std::cout << modifier << "Modifier" << STYLE::reset << std::endl;
    
    modifier.Set(FG::black);
    modifier.Set(BG::white);
    std::cout << modifier << "Second Modifier" << resetAll << std::endl;
    
    return 0;
}
