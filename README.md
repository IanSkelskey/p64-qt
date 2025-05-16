# Project64-Qt

A modern Qt-based reimplementation of Project64, the popular Nintendo 64 emulator.

## Project Status: Active Development

This project is making steady progress in **active development**. While it's not a fully functional emulator yet, the UI framework is now significantly developed with several core features implemented.

![GIF Demo](Screenshots/demo.gif)

## Current Features

- **Enhanced ROM Browser**
  - Detailed list view with customizable, sortable columns
  - Grid view with cover art support
  - Quick filtering and searching capabilities
  - Configurable ROM information display
  - Toggle between detail and grid views
  
- **Comprehensive ROM Information System**
  - Robust header data extraction (internal name, country code, CRC values, etc.)
  - Integration with SQLite database for extended ROM information
  - Support for metadata including developers, release dates, genres, and more
  - CIC chip detection and cartridge code identification
  
- **UI Framework**
  - Functional abstraction layer for interfacing with the emulation core
  - Modern, responsive UI with proper scaling and DPI awareness
  - Complete theme support with light and dark modes
  - System theme detection and preference following
  
- **Cover Art System**
  - Integrated cover downloader with progress tracking
  - Multiple source URL templates support
  - Database-assisted matching of ROM files to cover art
  - Customizable cover display options
  
- **Settings Management**
  - Centralized settings system using Qt's built-in mechanisms
  - User-friendly configuration dialog
  - Persistent preferences across sessions
  - ROM-specific configuration support

- **Enhancement Support**
  - UI for managing enhancement codes
  - GameShark code integration
  - Overclock settings management
  - Plugin-specific enhancements

## Recently Completed

- [x] Implemented complete theme engine with light/dark modes
- [x] Developed UI abstraction layer with essential emulation controls
- [x] Created integrated cover art downloader tool
- [x] Added enhancement management interface
- [x] Implemented database integration for ROM information
- [x] Developed configuration dialog with persistent settings

## Roadmap

- [ ] Complete integration with Project64 emulation core
- [ ] Implement save state management
- [ ] Add controller configuration interface
- [ ] Develop plugin management system
- [ ] Create macOS and Linux compatible builds
- [ ] Add debugger interface
- [ ] Implement enhanced audio controls and visualization

## Project Goals

1. Create a more modern, user-friendly interface for Project64
2. Maintain compatibility with existing Project64 plugins and ROM databases
3. Improve cross-platform support
4. Add features missing from the original Project64
5. Gradually port over and improve the emulation core functionality

## Cover Art Support

This project includes support for displaying cover art in the ROM browser's grid view. Cover art is sourced from the [N64-Covers Repository](https://github.com/IanSkelskey/n64-covers), a dedicated collection of Nintendo 64 game covers specifically designed for use with this project.

### Built-in Cover Downloader

The application now features an integrated cover downloader that can:
- Scan your ROM directory and identify missing covers
- Support multiple download sources with customizable URL templates
- Match covers using cartridge codes from the ROM database
- Display download progress and success statistics

### Cover Naming Convention

For proper detection by Project64-Qt, cover images must follow the N64 cartridge ID naming convention:
- Files should be named using the complete cartridge code (e.g., `NUS-NSME-USA.png` for Super Mario 64)
- PNG format with transparency is preferred

### Contributing Covers

If you'd like to help expand the cover art collection, please visit the [N64-Covers Repository](https://github.com/IanSkelskey/n64-covers) and follow the contribution guidelines provided there.

## Building the Project

The project uses CMake for build management and requires Qt 6.

*Detailed build instructions will be added as the project matures.*

## Contributing

As this project is in active development, contributions are welcome. Feel free to open issues for feature suggestions or bug reports.

## License

*License information will be added*

## Acknowledgments

- The original Project64 development team
- Contributors to the Qt framework
- N64 ROM database maintainers
- All contributors of cover art and ROM information

---

*This project is not affiliated with the original Project64 development team.*
