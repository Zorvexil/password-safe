# Password Safe

A lightweight, open-source password manager built with **C, GTK4, and libsodium**.

Password Safe is a desktop application designed to store account credentials in a local encrypted vault, with a focus on simplicity, privacy, and security.

## Features

- **Local storage** — Keep your credentials in a local vault.
- **Encrypted vault** — Protect stored credentials using cryptographic libraries.
- **Password generation** — Generate strong passwords with configurable rules.
- **Account management** — Organize credentials for different accounts and services.
- **GTK4 interface** — A native desktop interface.
- **Open source** — Built with free and open-source software.

## Technologies

- C
- GTK4
- libsodium
- Argon2id for password-based key derivation

## Requirements

To build Password Safe from source, you need:

- GCC or Clang
- GTK4 development libraries
- libsodium development libraries
- `pkg-config`

## Installation

### Linux

#### Fedora

Install the dependencies:

```bash
sudo dnf install gcc gtk4-devel libsodium-devel pkgconf-pkg-config
```

Clone the repository:

```bash
git clone https://github.com/Zorvexil/password-safe.git
cd password-safe
```

Compile the application:

```bash
gcc -Wall -Wextra -O2 password_safe.c \
    -o password-safe \
    $(pkg-config --cflags --libs gtk4 libsodium)
```

Run it:

```bash
./password-safe
```

#### Ubuntu and Debian

Install the dependencies:

```bash
sudo apt update
sudo apt install build-essential libgtk-4-dev libsodium-dev pkg-config
```

Clone and compile:

```bash
git clone https://github.com/Zorvexil/password-safe.git
cd password-safe

gcc -Wall -Wextra -O2 password_safe.c \
    -o password-safe \
    $(pkg-config --cflags --libs gtk4 libsodium)
```

Run the application:

```bash
./password-safe
```

#### Arch Linux

Install the dependencies:

```bash
sudo pacman -S --needed base-devel gtk4 libsodium pkgconf
```

Then clone the repository and use the compilation command above.

### macOS

**Status: Not yet verified.**

Install [Homebrew](https://brew.sh/) and the required dependencies:

```bash
brew install gtk4 libsodium pkgconf
```

Clone the repository:

```bash
git clone https://github.com/Zorvexil/password-safe.git
cd password-safe
```

Try compiling:

```bash
clang -Wall -Wextra -O2 password_safe.c \
    -o password-safe \
    $(pkg-config --cflags --libs gtk4 libsodium)
```

If compilation succeeds, run:

```bash
./password-safe
```

macOS support requires testing and may need platform-specific adjustments.

### Windows

**Status: Not yet verified.**

The suggested development environment is [MSYS2](https://www.msys2.org/).

Open the **MSYS2 UCRT64** terminal and install the dependencies:

```bash
pacman -S --needed \
    mingw-w64-ucrt-x86_64-toolchain \
    mingw-w64-ucrt-x86_64-gtk4 \
    mingw-w64-ucrt-x86_64-libsodium \
    mingw-w64-ucrt-x86_64-pkgconf
```

Clone the repository:

```bash
git clone https://github.com/Zorvexil/password-safe.git
cd password-safe
```

Try compiling:

```bash
gcc -Wall -Wextra -O2 password_safe.c \
    -o password-safe.exe \
    $(pkg-config --cflags --libs gtk4 libsodium)
```

If compilation succeeds, run:

```bash
./password-safe.exe
```

Windows support requires further testing. A successful compilation does not necessarily guarantee that the application runs correctly on every Windows system.

## Security

Password Safe is a developing open-source project and has not undergone an independent security audit.

- Use a strong, unique master password.
- Keep secure backups of your encrypted vault.
- Never commit passwords, private keys, or personal vault files to Git.
- Review the implementation before storing important credentials.

Using a cryptographic library does not automatically guarantee that an application is secure. Correct encryption, key derivation, vault handling, and memory management are essential.

## Project Status

Password Safe is under development.

Linux is the primary development platform. macOS and Windows build instructions are experimental and have not yet been verified by the maintainer.

Bug reports, suggestions, and contributions are welcome.

## Contributing

1. Fork the repository.
2. Create a branch for your changes.
3. Make and test your changes.
4. Submit a pull request describing your contribution.

Never include real credentials or personal vault data in issues, pull requests, or test files.

## License

Password Safe is intended to be distributed under the **GNU General Public License v2.0 (GPL-2.0-only)**.

See the `LICENSE` file for the complete license text.

## Dependencies

- [GTK4](https://www.gtk.org/) — Graphical user interface toolkit.
- [libsodium](https://doc.libsodium.org/) — Cryptographic library.

---

Maintained by [Zorvexil](https://github.com/Zorvexil).
