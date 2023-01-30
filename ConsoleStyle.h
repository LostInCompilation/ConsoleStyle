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

#include <mutex>

namespace ConsoleStyle
{
    class Modifier;

    //***********************************************************************
    // Color and style attributes
    enum class STYLE : uint8_t
    {
        // Styles
        bold      = 1,
        faint     = 2,
        italic    = 3,
        underline = 4,
        blink     = 5,
        invert    = 7,
        hide      = 8, // Not widely supported
        strike    = 9,
        
        // Reset
        reset   = 0,
        
        // No change
        none    = 255
    };

    // Foreground color
    enum class FG : uint8_t
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
        white     = 97,
        
        // Reset
        reset   = 39,
        
        // No change
        none    = 255
    };

    // Background color
    enum class BG : uint8_t
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
        white     = 107,
        
        // Reset
        reset   = 49,
        
        // No change
        none    = 255
    };

    //***********************************************************************
    // Capability modes
    enum class CapabilityMode : uint8_t
    {
        Disable   = 0,
        CheckOnce = 1,
        Auto      = 2,
        Force     = 3
    };

    //***********************************************************************
    // Concept for all ConsoleStyle attributes (colors, styles) and a Modifier
    template <typename T>
    concept IsConsoleStyleAttribute = std::is_same_v<T, STYLE> or std::is_same_v<T, FG> or std::is_same_v<T, BG>;

    template <typename T>
    concept IsCombinedModifier = std::is_same_v<T, Modifier>;

    //***********************************************************************
    // Implementation
    class ConsoleCapabilities final
    {
    private:
        std::atomic<bool>   m_GotCapabilityInfo = false;
        std::mutex          m_CapabilitiesMutex;
        
        inline int Fileno_OS(FILE* const file) const noexcept
        {
#ifdef TARGET_OS_WINDOWS
            return _fileno(file);
#else
            return fileno(file);
#endif
        }
        
        inline bool Is_pty_Win(int fd) const noexcept
        {
            return false;
        }
        
        inline bool Is_tty_OS(int fd) const noexcept
        {
            return false;
        }
        
        //***********************************************************************
        // Is output a terminal
        inline bool IsTerminalOutput(const std::streambuf* const sbuf) const noexcept
        {
            int fd = -1;
            
            // Get file descriptor
            if(sbuf == std::cout.rdbuf())
                fd = Fileno_OS(stdout);
            else if(sbuf == std::cerr.rdbuf() || sbuf == std::clog.rdbuf())
                fd = Fileno_OS(stderr);
            
            bool istty = (Is_tty_OS(fd) != 0);
            
#ifdef TARGET_OS_WINDOWS
            // Additional check for Pty/Msys on Windows
            if(!istty)
                istty |= Is_pty_Win(fd);
#endif
            
            return istty;
        }
        
        //***********************************************************************
        // Are styles and colors supported
        inline bool AreStylesSupported() const noexcept
        {
            return false;
        }
        
    public:
        ConsoleCapabilities() = default;
        ~ConsoleCapabilities() = default;

        // Non copyable
        ConsoleCapabilities(const ConsoleCapabilities&) = delete;
        void operator=(const ConsoleCapabilities&) = delete;
    };

    //***********************************************************************
    // Implementation
    class ConsoleStyleImpl final
    {
    private:
        ConsoleCapabilities         m_Capabilities;
        std::atomic<CapabilityMode> m_Mode = CapabilityMode::Auto;
        
        
    public:
        ConsoleStyleImpl() = default;
        ~ConsoleStyleImpl() = default;
        
        // Non copyable
        ConsoleStyleImpl(const ConsoleStyleImpl&) = delete;
        void operator=(const ConsoleStyleImpl&) = delete;
        
        // Singleton interface
        inline static ConsoleStyleImpl& GetInstance() noexcept // Thread safe in C++11 and above
        {
            static ConsoleStyleImpl me;
            return me;
        }
        
        //***********************************************************************
        // Check capabilities (if necessary and configured)
        bool CheckCapabilities(const std::streambuf* const sbuf) noexcept
        {
            if(m_Mode == CapabilityMode::Disable)
                return false;
            if(m_Mode == CapabilityMode::Force)
                return true;
            
            // Check only once on the first usage
            if(m_Mode == CapabilityMode::CheckOnce)
            {
                
            }
            
            // Auto: Check on every call (slower)
            if(m_Mode == CapabilityMode::Auto)
            {
                // AreStylesSupported
                // IsTerminalOutput
            }
            
            return false;
        }
        
        // Setting the capability mode
        void SetCapabilityMode(const CapabilityMode& mode) noexcept
        {
            m_Mode = mode;
        }
        
        //***********************************************************************
        // Set color or style attribute
#ifdef TARGET_OS_MAC
        template <IsConsoleStyleAttribute T>
        inline std::ostream& SetAttribute(std::ostream& os, const T attribute)
        {
            if(attribute == T::none) // No change
                return os;
            
            return os << "\033[" << static_cast<int32_t>(attribute) << "m";
        }
#else
#error No impl. ToDo
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
        STYLE   m_Style;
        FG      m_FG;
        BG      m_BG;
        
    public:
        Modifier(const FG& fg = FG::none, const BG& bg = BG::none, const STYLE& style = STYLE::none)
            : m_Style(style)
            , m_FG(fg)
            , m_BG(bg)
        {}
        
        template <IsConsoleStyleAttribute T>
        inline void Set(const T& attribute) noexcept
        {
            if constexpr (std::is_same_v<T, FG>)
                m_FG = attribute;
            else if constexpr (std::is_same_v<T, BG>)
                m_BG = attribute;
            else if constexpr (std::is_same_v<T, STYLE>)
                m_Style = attribute;
        }
        
        inline STYLE GetStyle() const noexcept { return m_Style; }
        inline FG    GetFG()    const noexcept { return m_FG; }
        inline BG    GetBG()    const noexcept { return m_BG; }
    };
    
    // Reset-all Modifier
    static Modifier resetAll(FG::reset, BG::reset, STYLE::reset);

    //***********************************************************************
    // std::ostream operator overload for color and style attributes
    template <IsConsoleStyleAttribute T>
    inline std::ostream& operator<<(std::ostream& os, const T& attribute)
    {
        if(ConsoleStyleImpl::GetInstance().CheckCapabilities(os.rdbuf()))
            return ConsoleStyleImpl::GetInstance().SetAttribute(os, attribute);
        else
            return os;
    }

    // std::ostream operator overload for Modifier
    template <IsCombinedModifier T>
    inline std::ostream& operator<<(std::ostream& os, const T& modifier)
    {
        if(ConsoleStyleImpl::GetInstance().CheckCapabilities(os.rdbuf()))
        {
            ConsoleStyleImpl::GetInstance().SetAttribute(os, modifier.GetStyle());
            ConsoleStyleImpl::GetInstance().SetAttribute(os, modifier.GetBG());
            return ConsoleStyleImpl::GetInstance().SetAttribute(os, modifier.GetFG());
        }
        else
        {
            return os;
        }
    }

    //***********************************************************************
    // Functions to interface configs
    void SetConsoleCapabilityMode(const CapabilityMode& mode) noexcept
    {
        ConsoleStyleImpl::GetInstance().SetCapabilityMode(mode);
    }
}

// Shorter name
namespace cstyle = ConsoleStyle;

// Cleanup our preprocessor OS checks
#undef TARGET_OS_UNIX
#undef TARGET_OS_MAC
#undef TARGET_OS_WINDOWS

#endif
