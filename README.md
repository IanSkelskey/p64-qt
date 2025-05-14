# Project64-Qt

A modern Qt-based reimplementation of Project64, the popular Nintendo 64 emulator.

## Project Status: Early Development

This project is currently in the **prototype/early development** stage. It is **not a working emulator yet**, but rather a foundational UI framework that will eventually connect to the Project64 emulation core.

![GIF Demo](Screenshots/demo.gif)

## Currently Implemented Features

- **Enhanced ROM Browser**
  - Detailed list view with sortable columns
  - New grid view with cover art support
  - Quick filtering/searching
  - Customizable ROM information display
  
- **ROM Information Parsing**
  - Header data extraction (internal name, country code, CRC values, etc.)
  - Integration with Project64 ROM databases (RDB/RDX)
  - Support for additional metadata (developers, release dates, genres)
  
- **UI Framework**
  - Abstraction layer for interfacing with the emulation core (WIP)
  - Modern Qt-based interface with better scaling and theming support

## Roadmap

- [ ] Complete UI abstraction layer
- [ ] Integrate with Project64 emulation core
- [ ] Port over settings and configuration management
- [ ] Implement save state and memory management features
- [ ] Add controller configuration interface
- [ ] Develop plugin management system
- [ ] Create macOS and Linux compatible builds

## Project Goals

1. Create a more modern, user-friendly interface for Project64
2. Maintain compatibility with existing Project64 plugins and ROM databases
3. Improve cross-platform support
4. Add features missing from the original Project64 (like the grid view with cover art)
5. Gradually port over and improve the emulation core functionality

## Cover Art Support

This project includes support for displaying cover art in the ROM browser's grid view. Cover art is sourced from the [N64-Covers Repository](https://github.com/IanSkelskey/n64-covers), a dedicated collection of Nintendo 64 game covers specifically designed for use with this project.

### Cover Naming Convention

For proper detection by Project64-Qt, all cover images must follow the N64 cartridge ID naming convention:
- Files should be named using the complete cartridge code (e.g., `NUS-NSME-USA.png` for Super Mario 64)
- PNG format with transparency is preferred

### Contributing Covers

If you'd like to help expand the cover art collection, please visit the [N64-Covers Repository](https://github.com/IanSkelskey/n64-covers) and follow the contribution guidelines provided there.

## Building the Project

*Build instructions will be added as the project matures.*

## Project Goals

1. Create a more modern, user-friendly interface for Project64
2. Maintain compatibility with existing Project64 plugins and ROM databases
3. Improve cross-platform support
4. Add features missing from the original Project64 (like the grid view with cover art)
5. Gradually port over and improve the emulation core functionality

## Contributing

As this project is in early development, contribution guidelines will be established as development progresses. Feel free to open issues for feature suggestions or bug reports.

## License

*License information will be added*

## Acknowledgments

- The original Project64 development team
- Contributors to the Qt framework
- N64 ROM database maintainers

---

*This project is not affiliated with the original Project64 development team.*
