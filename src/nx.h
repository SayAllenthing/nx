//----------------------------------------------------------------------------------------------------------------------
// The emulator object
// Manages a Spectrum-derived object and the UI (including the debugger)
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "spectrum.h"
#include "debugger.h"
#include "tape.h"

#include <SFML/Graphics.hpp>
#include <experimental/filesystem>
#include <map>
#include <mutex>
#include <thread>

namespace fs = experimental::filesystem::v1;

enum class Joystick
{
    Left,
    Right,
    Up,
    Down,
    Fire
};

//----------------------------------------------------------------------------------------------------------------------
// Emulator overlay
//----------------------------------------------------------------------------------------------------------------------

class Nx;

class Emulator : public Overlay
{
public:
    Emulator(Nx& nx);

    void render(Draw& draw) override;
    void key(sf::Keyboard::Key key, bool down, bool shift, bool ctrl, bool alt) override;
    void text(char ch) override;

    void showStatus();

    void openFile();
    void saveFile();

private:
    void joystickKey(Joystick key, bool down);
    void calculateKeys();

private:
    // Keyboard state
    vector<bool>        m_speccyKeys;
    vector<u8>          m_keyRows;
    int                 m_counter;


};

//----------------------------------------------------------------------------------------------------------------------
// Emulator class
//----------------------------------------------------------------------------------------------------------------------

class Nx
{
public:
    Nx(int argc, char** argv);
    ~Nx();

    // Obtain a reference to the current machine.
    Spectrum& getSpeccy() { return *m_machine; }

    // Render the currently generated display
    void render();

    // The emulator main loop.  Will exit when the window is closed.
    void run();

    // Generate a single frame, including processing audio.
    void frame();
    
    // Open a file, detect it's type and try to open it
    bool openFile(string fileName);

    // Save a file as a snapshot.
    bool saveFile(string fileName);
    
    // Settings
    void setSetting(string key, string value);
    string getSetting(string key, string defaultSetting = "no");
    void updateSettings();

    // Debugging
    bool isDebugging() const { return Overlay::currentOverlay() == &m_debugger; }
    void togglePause(bool breakpointHit);
    void stepOver();
    void stepIn();
    void stepOut();
    RunMode getRunMode() const { return m_runMode; }
    void setRunMode(RunMode runMode) { m_runMode = m_runMode; }

    // Peripherals
    bool usesKempstonJoystick() const { return m_kempstonJoystick; }

    // Mode selection
    void showTapeBrowser();
    void toggleDebugger();
    void hideAll();

    // Zoom
    void toggleZoom();
    bool getZoom() const { return m_zoom; }
    
private:
    // Loading
    bool loadSnaSnapshot(string fileName);
    bool loadZ80Snapshot(string fileName);
    bool loadTape(string fileName);
    bool loadNxSnapshot(string fileName);

    // Saving
    bool saveSnaSnapshot(string fileName);
    bool saveNxSnapshot(string fileName);
    
    // Debugging helper functions
    u16 nextInstructionAt(u16 address);
    bool isCallInstructionAt(u16 address);

    // Window scale
    void setScale(int scale);

private:
    Spectrum*           m_machine;
    Ui                  m_ui;
    Signal              m_renderSignal;
    bool                m_quit;
    int                 m_frameCounter;
    bool                m_zoom;

    // Emulator overlay
    Emulator            m_emulator;

    // Debugger state
    Debugger            m_debugger;
    RunMode             m_runMode;

    // Settings
    map<string, string> m_settings;

    // Rendering
    sf::RenderWindow    m_window;

    // Peripherals
    bool                m_kempstonJoystick;

    // Tape emulation
    TapeBrowser         m_tapeBrowser;

    // Files
    fs::path            m_tempPath;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
