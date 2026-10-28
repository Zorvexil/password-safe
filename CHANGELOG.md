# Changelog

All notable changes to Password Safe are documented in this file.

The project is currently in early development. This changelog describes the functionality implemented in the current source code.

## [Unreleased]

Changes under development will be documented here as they are implemented.

## [0.1.0-dev] - Initial Development Version

### Added

- Initial GTK4 desktop application written in C.
- Local vault storage in the user's application data directory.
- Master password creation and vault unlocking.
- Minimum master password length requirement of 12 characters.
- Password-based encryption key derivation using Argon2id through libsodium.
- Authenticated encryption of vault data using `crypto_secretbox`.
- Random salt generation for new vaults.
- Random nonce generation when saving the vault.
- Account creation with the following fields:
  - Application or website name.
  - Account type.
  - Username.
  - Email address.
  - Phone number.
  - Password.
- Password details view with a password visibility control.
- Account deletion with an attempt to restore the entry if saving fails.
- Random 24-character password generation.
- Password generation using libsodium's random number generation functions.
- Encrypted vault saving and loading.
- Basic validation and error messages for vault creation, unlocking, and saving.
- Basic password security guidance in the application interface.

### Security Notes

- The vault uses libsodium for key derivation and authenticated encryption.
- The application has not undergone an independent security audit.
- The current implementation has not been comprehensively tested against file corruption, unexpected shutdowns, or other failure scenarios.
- The application should not yet be considered production-ready for critical credentials.

### Limitations

- Linux is the primary development environment.
- Windows and macOS support have not been verified.
- Editing existing account entries is not currently implemented.
- The application does not currently include a password strength analysis feature.
- Password generation currently produces 24-character passwords using a predefined character set.

---

*This changelog will be updated as Password Safe develops.*
