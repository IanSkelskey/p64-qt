# N64-Qt

A modern, high-accuracy Nintendo 64 emulator with a feature-rich Qt-based interface.

## Project Vision

This project aims to create **the best possible N64 emulator** by combining:
- The high-accuracy **ares N64 emulation core**
- A modern, feature-rich **Qt-based interface** inspired by PCSX2
- Advanced features like **high-resolution texture support**

![GIF Demo](Screenshots/demo.gif)

## Current Features

- **Enhanced ROM Browser**
  - Detailed list view with customizable, sortable columns
  - Grid view with cover art support (inspired by PCSX2)
  - Quick filtering and searching capabilities
  - Configurable ROM information display
  - Toggle between detail and grid views
  
- **Comprehensive ROM Information System**
  - Robust header data extraction (internal name, country code, CRC values, etc.)
  - Integration with SQLite database for extended ROM information
  - Support for metadata including developers, release dates, genres, and more
  - CIC chip detection and cartridge code identification
  
- **UI Framework**
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

## Recently Completed

- [x] Implemented complete theme engine with light/dark modes
- [x] Created integrated cover art downloader tool
- [x] Added enhancement management interface
- [x] Implemented database integration for ROM information
- [x] Developed configuration dialog with persistent settings

## Roadmap

- [ ] Integrate the ares N64 emulation core for high accuracy
- [ ] Implement save state management
- [ ] Add controller configuration interface
- [ ] Implement enhanced audio controls and visualization
- [ ] Create macOS and Linux compatible builds
- [ ] Add debugger interface
- [ ] Add texture dumping capability
- [ ] Implement high-resolution texture support (similar to Project64)

## Project Goals

1. Create the most accurate N64 emulator possible using the ares core
2. Provide a modern, user-friendly interface inspired by best-in-class emulators like PCSX2
3. Enable advanced features like high-resolution textures and enhancement options
4. Support all major platforms (Windows, macOS, Linux)
5. Build a vibrant community around N64 preservation and enhancement

## Cover Art Support

This project includes support for displaying cover art in the ROM browser's grid view. Cover art is sourced from the [N64-Covers Repository](https://github.com/IanSkelskey/n64-covers), a dedicated collection of Nintendo 64 game covers specifically designed for use with this project.

### Built-in Cover Downloader

The application features an integrated cover downloader that can:
- Scan your ROM directory and identify missing covers
- Support multiple download sources with customizable URL templates
- Match covers using cartridge codes from the ROM database
- Display download progress and success statistics

### Cover Naming Convention

For proper detection, cover images must follow the N64 cartridge ID naming convention:
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

- The ares/higan team for their highly accurate emulation cores
- PCSX2 team for UI design inspiration
- Contributors to the Qt framework
- N64 ROM database maintainers
- All contributors of cover art and ROM information