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
    class Modifier;

    //***********************************************************************
    // Color and style attributes
    enum class style : uint8_t
    {
        reset   = 0,
        
        // No change
        none    = 255
    };

    // Foreground color
    enum class fg : uint8_t
    {
        // Normal intensity
        black   = 30,
        red     = 31,
        green   = 32,
        yellow  = 33,
        blue    = 34,
        magenta = 35,
        cyan    = 36,
        gray    = 37,
        
        // Bright intensity
        black_b   = 90,
        red_b     = 91,
        green_b   = 92,
        yellow_b  = 93,
        blue_b    = 94,
        magenta_b = 95,
        cyan_b    = 96,
        white    = 97,
        
        // Reset
        reset   = 39,
        
        // No change
        none    = 255
    };

    // Background color
    enum class bg : uint8_t
    {
        // Normal intensity
        black   = 40,
        red     = 41,
        green   = 42,
        yellow  = 43,
        blue    = 44,
        magenta = 45,
        cyan    = 46,
        gray    = 47,
        
        // Bright intensity
        black_b   = 100,
        red_b     = 101,
        green_b   = 102,
        yellow_b  = 103,
        blue_b    = 104,
        magenta_b = 105,
        cyan_b    = 106,
        white    = 107,
        
        // Reset
        reset   = 49,
        
        // No change
        none    = 255
    };

    //***********************************************************************
    // Concept for all ConsoleStyle attributes (colors, styles) and a combined Modifier
    template <typename T>
    concept IsConsoleStyleAttribute = std::is_same_v<T, style> or std::is_same_v<T, fg> or std::is_same_v<T, bg>;

    template <typename T>
    concept IsCombinedModifier = std::is_same_v<T, Modifier>;

    //***********************************************************************
    // Implementation
    class ConsoleStyleImpl final
    {
    private:
        
    public:
        ConsoleStyleImpl() = default;
        ~ConsoleStyleImpl() = default;
        
        // Non copyable
        ConsoleStyleImpl(const ConsoleStyleImpl&) = delete;
        void operator=(const ConsoleStyleImpl&) = delete;
        
        // Singleton instance
        inline static ConsoleStyleImpl& GetInstance()
        {
            static ConsoleStyleImpl me;
            return me;
        }
        
        //***********************************************************************
        // Set attribute
#ifdef TARGET_OS_MAC
        template <IsConsoleStyleAttribute T>
        inline std::ostream& SetAttribute(std::ostream& os, const T attribute)
        {
            if(attribute == T::none) // No change
                return os;
            
            return os << "\033[" << static_cast<int32_t>(attribute) << "m";
        }
#endif
        
        //***********************************************************************
        // std::ostream operator overload for color and style attributes
        template <IsConsoleStyleAttribute T>
        friend inline std::ostream& operator<<(std::ostream& os, const T& attribute);
        
        // std::ostream operator overload for Modifier
        template <IsCombinedModifier T>
        friend inline std::ostream& operator<<(std::ostream& os, const T& modifier);
    };

    //***********************************************************************
    // Modifier to combine color and style attributes into one object
    class Modifier
    {
    private:
        style   m_Style;
        fg      m_FG;
        bg      m_BG;
        // MAKING CONST? OR SHOULD MODIFIER BE CHANGEABLE?
        
    public:
        Modifier(const fg& _fg = fg::none, const bg& _bg = bg::none, const style& _style = style::none)
            : m_Style(_style)
            , m_FG(_fg)
            , m_BG(_bg)
        {}
        
        
        
        // CONSIDER USING GETTER/SETTER?
        // std::ostream operator overload for Modifier
        template <IsCombinedModifier T>
        friend inline std::ostream& operator<<(std::ostream& os, const T& modifier);
    };

    //***********************************************************************
    // std::ostream operator overload for color and style attributes
    template <IsConsoleStyleAttribute T>
    inline std::ostream& operator<<(std::ostream& os, const T& attribute)
    {
        return ConsoleStyleImpl::GetInstance().SetAttribute(os, attribute);
    }

    // std::ostream operator overload for Modifier
    template <IsCombinedModifier T>
    inline std::ostream& operator<<(std::ostream& os, const T& modifier)
    {
        ConsoleStyleImpl::GetInstance().SetAttribute(os, modifier.m_Style);
        ConsoleStyleImpl::GetInstance().SetAttribute(os, modifier.m_BG);
        return ConsoleStyleImpl::GetInstance().SetAttribute(os, modifier.m_FG);;
    }
}

// Shorter name
namespace cstyle = ConsoleStyle;

// Cleanup our preprocessor OS checks
#undef TARGET_OS_UNIX
#undef TARGET_OS_MAC
#undef TARGET_OS_WINDOWS

#endif
