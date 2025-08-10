#ifndef APPLICATIONCONSTANTS_H
#define APPLICATIONCONSTANTS_H

namespace ApplicationConstants
{
    static constexpr auto APPLICATION_NAME{"Time Machine Logs"};
    static constexpr auto APPLICATION_VERSION{"1.0"};
    static constexpr auto APPLICATION_DESCRIPTION{"The one and only app to help you when you are lost in time."};

    static constexpr auto MODE_SHORT{"m"};
    static constexpr auto MODE_LONG{"mode"};
    static constexpr auto INPUT_SHORT{"i"};
    static constexpr auto INPUT_LONG{"input"};
    static constexpr auto OUTPUT_SHORT{"o"};
    static constexpr auto OUTPUT_LONG{"output"};

    static constexpr auto MODE_DESCRIPTION{"Operation mode: pack or unpack"};
    static constexpr auto INPUT_DESCRIPTION{"Input directory or archive file"};
    static constexpr auto OUTPUT_DESCRIPTION{"Output archive file or directory"};

    static constexpr auto MODE_PACK{"pack"};
    static constexpr auto MODE_UNPACK{"unpack"};
}

#endif // APPLICATIONCONSTANTS_H
