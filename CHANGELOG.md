# Changelog

All notable changes to Password Safe will be documented in this file.

This project is currently in early development. Features and implementation details may change between versions.

## [Unreleased]

### Planned
- Improve the GTK4 user interface and overall usability.
- Improve password generation rules and customization.
- Continue testing and improving vault reliability.
- Investigate support for Windows and macOS.

## [0.1.0-dev] - Initial Development

### Added
- Initial public GitHub repository.
- Desktop application interface built with C and GTK4.
- Local password vault functionality.
- Master-password-based vault protection.
- Password and account entry management.
- Support for storing account information, including application name, account type, username, email, phone number, and password.
- Password generation functionality.
- Dynamic password suggestions based on user-provided words.
- Password suggestion controls, including viewing and copying generated suggestions.
- Cryptographic functionality using libsodium.
- Argon2id-based key derivation in the vault protection workflow.

### Development Notes
- Linux is the primary development platform.
- The application is still under active development.
- The project has not undergone an independent security audit.
- Windows and macOS support have not been verified.
- The application is not yet recommended for storing critical credentials.

---

[Unreleased]: https://github.com/Zorvexil/password-safe/compare/main...HEAD
