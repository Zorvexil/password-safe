# Changelog

All notable changes to Password Safe are documented in this file.

This project follows a development-focused changelog. Features are listed according to the current implementation, and future changes will be documented as they are released.

## [Unreleased]

### Planned
- Improve the GTK4 interface and user experience.
- Improve vault error handling and data integrity.
- Expand password generation options.
- Investigate Windows and macOS compatibility.
- Continue reviewing and improving security.

## [0.1.0-dev] - Initial Development

### Added
- Initial public release of the source code.
- Native desktop interface built with C and GTK4.
- Local vault storage in the user's application data directory.
- Master password setup and vault unlocking.
- Password-based encryption key derivation using Argon2id through libsodium.
- Authenticated encryption using libsodium's `crypto_secretbox`.
- Random salt generation for new vaults.
- Random nonce generation when saving encrypted data.
- Account creation with application name, account type, username, email, phone number, and password fields.
- Account details viewing with a password visibility control.
- Account deletion with an attempt to restore the entry if saving fails.
- Random 24-character password generation.
- Basic validation for required fields and master password length.
- Basic error messages for vault creation, unlocking, and saving.
- Password security guidance within the application interface.

### Security
- Vault data is encrypted before being written to disk.
- File creation requests restrictive permissions for the vault and its temporary save file.
- The project has not undergone an independent security audit.
- The application is still under development and is not yet recommended for critical credentials.

### Limitations
- Editing existing account entries is not implemented.
- Password strength analysis is not implemented.
- Windows and macOS compatibility has not been verified.
- The application has not been independently assessed for production security.

---

*Password Safe is an open-source project under active development. This changelog will evolve alongside the codebase.*
