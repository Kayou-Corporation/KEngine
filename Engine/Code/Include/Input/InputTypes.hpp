#pragma once

#include <cstdint>
#include <string>



namespace Kayou::Input
{
    enum class EDeviceType : uint8_t
    {
        Unknown = 0,
        Keyboard = 1,
        Mouse = 2,
        Gamepad = 3,
        Other = 4
    };


    enum class EKey : uint16_t
    {
        Unknown = 0,
        A = 1,
        B = 2,
        C = 3,
        D = 4,
        E = 5,
        F = 6,
        G = 7,
        H = 8,
        I = 9,
        J = 10,
        K = 11,
        L = 12,
        M = 13,
        N = 14,
        O = 15,
        P = 16,
        Q = 17,
        R = 18,
        S = 19,
        T = 20,
        U = 21,
        V = 22,
        W = 23,
        X = 24,
        Y = 25,
        Z = 26,
        Num0 = 27,
        Num1 = 28,
        Num2 = 29,
        Num3 = 30,
        Num4 = 31,
        Num5 = 32,
        Num6 = 33,
        Num7 = 34,
        Num8 = 35,
        Num9 = 36,
        Return = 37,
        Escape = 38,
        Backspace = 39,
        Tab = 40,
        Space = 41,
        Minus = 42,
        Equals = 43,
        LeftBracket = 44,
        RightBracket = 45,
        Backslash = 46,
        Semicolon = 47,
        Apostrophe = 48,
        Grave = 49,
        Comma = 50,
        Period = 51,
        Slash = 52,
        CapsLock = 53,
        F1 = 54,
        F2 = 55,
        F3 = 56,
        F4 = 57,
        F5 = 58,
        F6 = 59,
        F7 = 60,
        F8 = 61,
        F9 = 62,
        F10 = 63,
        F11 = 64,
        F12 = 65,
        PrintScreen = 66,
        ScrollLock = 67,
        Pause = 68,
        Insert = 69,
        Home = 70,
        PageUp = 71,
        Delete = 72,
        End = 73,
        PageDown = 74,
        ArrowRight = 75,
        ArrowLeft = 76,
        ArrowDown = 77,
        ArrowUp = 78,
        LeftCtrl = 79,
        LeftShift = 80,
        LeftAlt = 81,
        LeftGui = 82,
        RightCtrl = 83,
        RightShift = 84,
        RightAlt = 85,
        RightGui = 86,

        // SDL_SCANCODE_NONUSHASH and following
        NonUsHash = 87,
        NumLockClear = 88, // Num Lock (PC) / Clear (Mac)
        KpDivide = 89,
        KpMultiply = 90,
        KpMinus = 91,
        KpPlus = 92,
        KpEnter = 93,
        Kp1 = 94,
        Kp2 = 95,
        Kp3 = 96,
        Kp4 = 97,
        Kp5 = 98,
        Kp6 = 99,
        Kp7 = 100,
        Kp8 = 101,
        Kp9 = 102,
        Kp0 = 103,
        KpPeriod = 104,
        NonUsBackslash = 105,
        Application = 106, // Windows contextual menu, "compose"
        Power = 107,
        KpEquals = 108,
        F13 = 109,
        F14 = 110,
        F15 = 111,
        F16 = 112,
        F17 = 113,
        F18 = 114,
        F19 = 115,
        F20 = 116,
        F21 = 117,
        F22 = 118,
        F23 = 119,
        F24 = 120,
        Execute = 121,
        Help = 122,
        Menu = 123,
        Select = 124,
        Stop = 125,
        Again = 126,
        Undo = 127,
        Cut = 128,
        Copy = 129,
        Paste = 130,
        Find = 131,
        Mute = 132,
        VolumeUp = 133,
        VolumeDown = 134,
        KpComma = 135,
        KpEqualsAs400 = 136,

        // International Keys / IME (Asian Keyboards)
        International1 = 137,
        International2 = 138,
        International3 = 139, // Yen
        International4 = 140,
        International5 = 141,
        International6 = 142,
        International7 = 143,
        International8 = 144,
        International9 = 145,
        Lang1 = 146, // Hangul/English toggle
        Lang2 = 147, // Hanja conversion
        Lang3 = 148, // Katakana
        Lang4 = 149, // Hiragana
        Lang5 = 150, // Zenkaku/Hankaku
        Lang6 = 151, // reserve
        Lang7 = 152, // reserve
        Lang8 = 153, // reserve
        Lang9 = 154, // reserve

        AltErase = 155,
        SysReq = 156,
        Cancel = 157,
        Clear = 158,
        Prior = 159,
        Return2 = 160,
        Separator = 161,
        Out = 162,
        Oper = 163,
        ClearAgain = 164,
        CrSel = 165,
        ExSel = 166,

        Kp00 = 167,
        Kp000 = 168,
        ThousandsSeparator = 169,
        DecimalSeparator = 170,
        CurrencyUnit = 171,
        CurrencySubunit = 172,
        KpLeftParen = 173,
        KpRightParen = 174,
        KpLeftBrace = 175,
        KpRightBrace = 176,
        KpTab = 177,
        KpBackspace = 178,
        KpA = 179,
        KpB = 180,
        KpC = 181,
        KpD = 182,
        KpE = 183,
        KpF = 184,
        KpXor = 185,
        KpPower = 186,
        KpPercent = 187,
        KpLess = 188,
        KpGreater = 189,
        KpAmpersand = 190,
        KpDblAmpersand = 191,
        KpVerticalBar = 192,
        KpDblVerticalBar = 193,
        KpColon = 194,
        KpHash = 195,
        KpSpace = 196,
        KpAt = 197,
        KpExclam = 198,
        KpMemStore = 199,
        KpMemRecall = 200,
        KpMemClear = 201,
        KpMemAdd = 202,
        KpMemSubtract = 203,
        KpMemMultiply = 204,
        KpMemDivide = 205,
        KpPlusMinus = 206,
        KpClear = 207,
        KpClearEntry = 208,
        KpBinary = 209,
        KpOctal = 210,
        KpDecimal = 211,
        KpHexadecimal = 212,

        Mode = 213, // AltGr, cf. SDL_KMOD_MODE

        // Usage page 0x0C (Multimedia keys/consumer)
        Sleep = 214,
        Wake = 215,
        ChannelIncrement = 216,
        ChannelDecrement = 217,
        MediaPlay = 218,
        MediaPause = 219,
        MediaRecord = 220,
        MediaFastForward = 221,
        MediaRewind = 222,
        MediaNextTrack = 223,
        MediaPreviousTrack = 224,
        MediaStop = 225,
        MediaEject = 226,
        MediaPlayPause = 227,
        MediaSelect = 228,
        AcNew = 229,
        AcOpen = 230,
        AcClose = 231,
        AcExit = 232,
        AcSave = 233,
        AcPrint = 234,
        AcProperties = 235,
        AcSearch = 236,
        AcHome = 237,
        AcBack = 238,
        AcForward = 239,
        AcStop = 240,
        AcRefresh = 241,
        AcBookmarks = 242,

        // Mobile inputs
        SoftLeft = 243,
        SoftRight = 244,
        Call = 245,
        EndCall = 246,

        Count = 247
    };


    enum class EMouseButton : uint8_t
    {
        Unknown = 0,
        Left = 1,
        Middle = 2,
        Right = 3,
        X1 = 4,
        X2 = 5,
        Count = 6
    };


    // Name buttons based on their physical position (like SDL_GamepadButton) rather than their Xbox/PlayStation symbol, which is already SDL's cross-platform approach.
    enum class EGamepadButton : uint8_t
    {
        Unknown = 0,
        South = 1,
        East = 2,
        West = 3,
        North = 4,
        Back = 5,
        Guide = 6,
        Start = 7,
        LeftStick = 8,
        RightStick = 9,
        LeftShoulder = 10,
        RightShoulder = 11,
        DPadUp = 12,
        DPadDown = 13,
        DPadLeft = 14,
        DPadRight = 15,
        Count = 16
    };


    enum class EGamepadAxis : uint8_t
    {
        Unknown = 0,
        LeftX = 1,
        LeftY = 2,
        RightX = 3,
        RightY = 4,
        LeftTrigger = 5,
        RightTrigger = 6,
        Count = 7
    };


    // Brut normalized value: digital buttons emit 0.0f / 1.0f
    // Axes emit a value in between [-1, 1] or [0, 1] for triggers
    struct RawInputEvent
    {
        EDeviceType device = EDeviceType::Keyboard;
        uint16_t code = 0; // Based on the `device` (EKey / EMouseButton / EGamepadButton / EGamepadAxis)
        float value = 0.0f;
        uint32_t deviceId = 0; // Useful when you have multiple gamepads
        uint64_t timestampNs = 0;
    };


    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;
    };
}
