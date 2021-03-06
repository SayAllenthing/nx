//----------------------------------------------------------------------------------------------------------------------
// Base class for all models of Spectrum
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "types.h"
#include "config.h"
#include "z80.h"
#include "audio.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// Configuration structure
//----------------------------------------------------------------------------------------------------------------------

struct SpectrumConfig
{
    u32*        image;
};

//----------------------------------------------------------------------------------------------------------------------
// Keyboard keys
//----------------------------------------------------------------------------------------------------------------------

enum class Key
{
    Shift, Z, X, C, V,
    A, S, D, F, G,
    Q, W, E, R, T,
    _1, _2, _3, _4, _5,
    _0, _9, _8, _7, _6,
    P, O, I, U, Y,
    Enter, L, K, J, H,
    Space, SymShift, M, N, B,
    
    COUNT
};

//----------------------------------------------------------------------------------------------------------------------
// Run mode
//----------------------------------------------------------------------------------------------------------------------

enum class RunMode
{
    Stopped,    // Don't run any instructions.
    Normal,     // Emulate as normal, run as fast as possible for a frame.
    StepIn,     // Step over a single instruction, and follow CALLs.
    StepOver,   // Step over a single instruction, and run a subroutine CALL till it returns to following instruction.
};

//----------------------------------------------------------------------------------------------------------------------
// Spectrum base class
// Each model must override this and implement the specifics
//----------------------------------------------------------------------------------------------------------------------

class Tape;

class Spectrum: public IExternals
{
public:
    // TState counter
    using TState        = i64;

    //------------------------------------------------------------------------------------------------------------------
    // Construction/Destruction
    //------------------------------------------------------------------------------------------------------------------

    Spectrum(function<void()> frameFunc);
    virtual ~Spectrum();

    //------------------------------------------------------------------------------------------------------------------
    // Control
    //------------------------------------------------------------------------------------------------------------------
    
    void            reset               (bool hard = true);

    //------------------------------------------------------------------------------------------------------------------
    // State
    //------------------------------------------------------------------------------------------------------------------

    sf::Sprite&     getVideoSprite      ();
    TState          getFrameTime        () const { return 69888; }
    u8              getBorderColour     () const { return m_borderColour; }
    Z80&            getZ80              () { return m_z80; }
    TState          getTState           () { return m_tState;}
    Audio&          getAudio            () { return m_audio; }
    Tape*           getTape             () { return m_tape; }

    //------------------------------------------------------------------------------------------------------------------
    // IExternals interface
    //------------------------------------------------------------------------------------------------------------------

    u8              peek                (u16 address) override;
    u8              peek                (u16 address, TState& t) override;
    u16             peek16              (u16 address, TState& t) override;
    void            poke                (u16 address, u8 x, TState& t) override;
    void            poke16              (u16 address, u16 x, TState& t) override;
    void            contend             (u16 address, TState delay, int num, TState& t) override;
    u8              in                  (u16 port, TState& t) override;
    void            out                 (u16 port, u8 x, TState& t) override;

    //------------------------------------------------------------------------------------------------------------------
    // General functionality, not specific to a model
    //------------------------------------------------------------------------------------------------------------------

    // Main function called to generate a single frame or instruction (in single-step mode).  Will return true if
    // a frame was complete.
    bool            update              (RunMode runMode, bool& breakpointHit);

    // Emulation control
    void            togglePause         ();
    
    // Set the keyboard state.
    void            setKeyboardState    (vector<u8>& rows);
    
    // Set the border
    void            setBorderColour     (u8 borderColour);

    // Reset the tState counter
    void            resetTState         () { m_tState = 0; }

    // Set the tState counter
    void            setTState           (TState t) { m_tState = t;}

    // Set the tape, it will be played if not stopped.
    void            setTape             (Tape* tape) { m_tape = tape;}

    // Render all video, irregardless of t-state.
    void            renderVideo         ();

    //------------------------------------------------------------------------------------------------------------------
    // Memory interface
    //------------------------------------------------------------------------------------------------------------------

    bool            isContended         (u16 addr) const;
    TState          contention          (TState t);
    void            poke                (u16 address, u8 x);
    void            load                (u16 address, const vector<u8>& buffer);
    void            load                (u16 address, const void* buffer, i64 size);
    void            setRomWriteState    (bool writable);

    //------------------------------------------------------------------------------------------------------------------
    // I/O interface
    //------------------------------------------------------------------------------------------------------------------

    void            ioContend           (u16 port, TState delay, int num, TState& t);

    //------------------------------------------------------------------------------------------------------------------
    // Kempston interface
    //------------------------------------------------------------------------------------------------------------------

    void            setKempstonState(u8 state);
    u8              getKempstonState() const;

    //------------------------------------------------------------------------------------------------------------------
    // Debugger interface
    //------------------------------------------------------------------------------------------------------------------

    void            toggleBreakpoint        (u16 address);
    void            addTemporaryBreakpoint  (u16 address);
    bool            hasUserBreakpointAt     (u16 address);

private:
    //
    // Memory
    //
    void            initMemory          ();

    //
    // Video
    //
    void            initVideo           ();
    void            updateVideo         ();

    //
    // Tape
    //
    void            updateTape          (TState numTStates);

    //
    // Breakpoints
    //
    enum class BreakpointType
    {
        User,       // User added breakpoint, only user can remove it
        Temporary,  // System added breakpoint, and it should be removed when hit.
    };

    struct Breakpoint
    {
        BreakpointType  type;
        u16             address;
    };

    vector<Breakpoint>::iterator    findBreakpoint          (u16 address);
    bool                            shouldBreak             (u16 address);


private:

    // Clock state
    TState          m_tState;

    // Video state
    u32*            m_image;
    sf::Texture     m_videoTexture;
    sf::Sprite      m_videoSprite;
    u8              m_frameCounter;
    vector<u16>     m_videoMap;         // Maps t-states to addresses
    int             m_videoWrite;       // Write point into 2D image array
    TState          m_startTState;      // Starting t-state for top-left of window
    TState          m_drawTState;       // Current t-state that has been draw to

    // Audio state
    Audio           m_audio;
    Tape*           m_tape;

    // Memory state
    vector<u8>      m_ram;
    vector<u8>      m_contention;
    bool            m_romWritable;

    // CPU state
    Z80             m_z80;

    // ULA state
    u8              m_borderColour;
    vector<u8>      m_keys;
    u8              m_speaker;
    u8              m_tapeEar;

    // Debugger state
    vector<Breakpoint>  m_breakpoints;

    // Kempston
    bool            m_kempstonJoystick;
    u8              m_kempstonState;
};
