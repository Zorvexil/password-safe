# Changelog

All notable changes to Password Safe are documented in this file.

## [Unreleased]

Changes made after the current development version will be listed here.

## [0.1.0-dev] — Initial Development

### Added
- Initial C and GTK4 desktop application.
- Local vault storage.
- Master password setup and vault unlocking.
- Argon2id key derivation using libsodium.
- Vault encryption and authentication using `crypto_secretbox`.
- Random salt generation for new vaults.
- Random nonce generation when saving vault data.
- Account creation with application name, account type, username, email, phone number, and password.
- Account details viewing with password visibility control.
- Account deletion.
- Random 24-character password generation.
- Basic input validation and error messages.
- Basic password security guidance.

### Known Limitations
- Editing existing accounts is not implemented.
- Password strength analysis is not implemented.
- Windows and macOS support have not been verified.
- The application has not undergone an independent security audit and is not recommended for critical credentials.

---

*Password Safe is an open-source project under active development.*
