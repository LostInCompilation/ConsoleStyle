/*
 
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

#ifndef CONSOLE_STYLE_DOT_H
#define CONSOLE_STYLE_DOT_H

//***********************************************************************
// Determine the target OS
#ifndef TARGET_OS_FORCE_UNIX
    #if (__linux__) || (__unix__)  || (__unix)
        #define TARGET_OS_UNIX
    #elif (__APPLE__ && __MACH__)
        #define TARGET_OS_MAC
    #elif _WIN32 // For Win32 and Win64
        #define TARGET_OS_WINDOWS
    #else
        #error The target OS is unknown or unsupported. If you believe your OS is UNIX compatible, manually define "TARGET_OS_FORCE_UNIX" to skip this check.
    #endif
#else
    #define TARGET_OS_UNIX // User forced to use UNIX
#endif


namespace ConsoleStyle
{
    //***********************************************************************
    // Color and style attributes
    enum class styleattr : uint8_t
    {
        myattr,
        myattr2
    };

    //***********************************************************************
    // Concept for all ConsoleStyle attributes (colors, styles)
    template <typename T>
    concept IsConsoleStyleAttribute = std::is_same_v<T, styleattr>;

    //***********************************************************************
    // IMPL
    class ConsoleStyleImpl final
    {
    private:
        int myPriv = 0;
        
    public:
        ConsoleStyleImpl() = default;
        ~ConsoleStyleImpl() = default;
        
        // Non copyable
        ConsoleStyleImpl(const ConsoleStyleImpl&) = delete;
        void operator=(const ConsoleStyleImpl&) = delete;
        
        // Singleton instance
        static ConsoleStyleImpl& GetInstance()
        {
            static ConsoleStyleImpl me;
            return me;
        }
        
        //***********************************************************************
        // std::ostream operator overload for color and style attributes
        template <IsConsoleStyleAttribute T>
        friend std::ostream& operator<<(std::ostream& os, const T& attribute);
        
        
        //***********************************************************************
        // Debug, will be removed
#ifdef VERBOSE_DBG
        void WhichOS()
        {
#if defined(TARGET_OS_UNIX)
            std::cout << "Running on UNIX/LINUX" << std::endl;
#elif defined(TARGET_OS_MAC)
            std::cout << "Running on MACOS" << std::endl;
#elif defined(TARGET_OS_WIN)
            std::cout << "Running on WINDOWS" << std::endl;
#endif
        }
#endif
    };

    //***********************************************************************
    // std::ostream operator overload for color and style attributes
    template <IsConsoleStyleAttribute T>
    inline std::ostream& operator<<(std::ostream& os, const T& attribute)
    {
        ConsoleStyleImpl::GetInstance().myPriv = 42; // Testing
        
        os << "Overload called: ";
        return os;
    }
}

// Shorter name
namespace cstyle = ConsoleStyle;

// Cleanup our preprocessor OS checks
#undef TARGET_OS_UNIX
#undef TARGET_OS_MAC
#undef TARGET_OS_WINDOWS

#endif
