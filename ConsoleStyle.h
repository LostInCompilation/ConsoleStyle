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
        
        void foo() {}
        
        template <IsConsoleStyleAttribute T>
        friend std::ostream& operator<<(std::ostream& os, const T& attribute);
    };

    //***********************************************************************
    // std::ostream operator overload
    template <IsConsoleStyleAttribute T>
    inline std::ostream& operator<<(std::ostream& os, const T& attribute)
    {
        ConsoleStyleImpl::GetInstance().myPriv = 42;
        ConsoleStyleImpl::GetInstance().foo();
        
        os << "Overload called: ";
        return os;
    }

}

// Shorter name
namespace cstyle = ConsoleStyle;

#endif
