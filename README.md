# Password Safe

**Version:** `0.1.0-dev`  
**Status:** Early development — not production-ready  
**License:** GNU General Public License v2.0

Password Safe is a lightweight, open-source password manager built with C, GTK4, and libsodium.

The project aims to provide a simple desktop application for managing passwords locally, with a focus on privacy, usability, and security.

Password Safe is currently under active development. Some features may be incomplete, and the application has not undergone an independent security audit.

## Features

- **Local storage** — Designed to keep password data on the local device.
- **Encrypted vault** — Intended to protect stored credentials using cryptographic libraries.
- **Password generation** — A feature for generating strong passwords.
- **Account management** — Organize credentials for different accounts and services.
- **GTK4 interface** — A graphical desktop interface built with GTK4.
- **Open source** — Free and open-source software that can be studied and improved by the community.

*Features may be incomplete or subject to change during development.*

## Technologies

- **C** — Main programming language.
- **GTK4** — Graphical user interface.
- **libsodium** — Cryptographic library.
- **Argon2id** — Password-based key derivation, if implemented in the current version.

## Requirements

To build Password Safe from source, you may need:

- GCC or Clang
- GTK4 development libraries
- libsodium development libraries
- `pkg-config`
- GNU Make (if a Makefile is added)

## Installation

### Fedora

Install the required development packages:

```bash
sudo dnf install gcc gtk4-devel libsodium-devel pkgconf-pkg-config
```

Clone the repository:

```bash
git clone https://github.com/Zorvexil/password-safe.git
cd password-safe
```

Compile the source file:

```bash
gcc password_safe.c -o password-safe \
  $(pkg-config --cflags --libs gtk4 libsodium)
```

Run the application:

```bash
./password-safe
```

### Ubuntu / Debian

Install the dependencies:

```bash
sudo apt update
sudo apt install build-essential libgtk-4-dev libsodium-dev pkg-config
```

Clone and compile the project:

```bash
git clone https://github.com/Zorvexil/password-safe.git
cd password-safe

gcc password_safe.c -o password-safe \
  $(pkg-config --cflags --libs gtk4 libsodium)

./password-safe
```

### Arch Linux

Install the dependencies:

```bash
sudo pacman -S base-devel gtk4 libsodium pkgconf
```

Then clone and compile the project using the commands shown above.

### macOS and Windows

Support for macOS and Windows has not yet been verified. Build instructions for these platforms may be added in the future.

## Security

Password Safe is an early-stage project and should not yet be considered suitable for storing sensitive or critical credentials.

Please keep the following points in mind:

- The application has not undergone an independent security audit.
- Do not assume that every credential is securely encrypted unless the relevant implementation has been verified.
- Use a strong, unique master password if the current version supports one.
- Keep secure backups of important data.
- Never commit real passwords, private keys, vault files, or other sensitive information to the repository.
- Using a cryptographic library does not automatically guarantee that an application is secure.

Security improvements and testing are important parts of the project's ongoing development.

## Development Status

Password Safe is under active development.

The current priorities are to improve the application's functionality, usability, reliability, and security.

The primary development environment is Linux. Support for other operating systems remains unverified.

Contributions, bug reports, and constructive suggestions are welcome.

## Contributing

Contributions are welcome!

If you would like to contribute:

1. Fork the repository.
2. Create a branch for your changes.
3. Make your changes and test them.
4. Submit a pull request describing your improvements.

Please report bugs and suggest improvements through the repository's GitHub Issues page.

## License

Password Safe is intended to be distributed under the **GNU General Public License v2.0 (GPL-2.0-only)**.

See the [`LICENSE`](LICENSE) file for the full license text.

## Dependencies

- [GTK4](https://www.gtk.org/) — Graphical user interface toolkit.
- [libsodium](https://doc.libsodium.org/) — Cryptographic library.

## Maintainer

Maintained by [Zorvexil](https://github.com/Zorvexil).

---

*Built with C and GTK4, with an emphasis on privacy, simplicity, and open-source development.*
