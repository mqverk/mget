# mget

A fast CLI tool to download Modrinth plugins directly from the terminal using the Modrinth API.

Made and owned by [mqverk](https://github.com/mqverk)

## Features

- Search for plugins on Modrinth
- Download specific plugin versions
- Filter by server type (Paper, Spigot, Bukkit, etc.)
- Fast and lightweight
- Written in C

## Requirements

- GCC or Clang
- Make
- libcurl (for HTTP requests)
- jq or jansson (for JSON parsing)

## Installation

Clone the repository:

```bash
git clone https://github.com/mqverk/mget.git
cd mget
```

Install dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential libcurl4-openssl-dev libjson-c-dev

# Fedora
sudo dnf install gcc make libcurl-devel json-c-devel

# Arch Linux
sudo pacman -S base-devel curl json-c

# macOS
brew install curl json-c
```

## Compile

### Search for plugins

```bash
./mget -f luckperms
```

Example output:
```
[ info ] Searching Modrinth for 'luckperms'...

 ID (Slug)                 | Name                           | Author
--------------------------------------------------------------------------------
 luckperms                 | LuckPerms                      | lucko
 clearlag++                | ClearLag++                     | fernsehheft
 lpc-chat                  | LPC Minimessage - Chat Format  | Ayont
 luckprefix                | LuckPrefix                     | ezTxmMC
 prefix-system             | Prefix System                  | 7AGAM
 axrankmenu                | AxRankMenu                     | ArtillexStudios
 universal-perms           | Universal Perms                | MattiDragon
 homeward-bound            | Homeward Bound                 | AdyTech99
 chat-formatter            | Chat Formatter                 | 7AGAM
 lprm                      | LPRM - LuckPerms Rank Maker    | Olafcio1
--------------------------------------------------------------------------------

[ info ] Found 10 plugins matching 'luckperms'.
```

### Download a specific plugin version

```bash
./mget -p luckperms -v 1.21.8 -s paper
```

Example output:
```
[ info ] Resolving version for 'luckperms' (v1.21.8, paper)...
[ info ] Downloading file: LuckPerms-Bukkit-5.5.17.jar
[ success ] Saved as LuckPerms-Bukkit-5.5.17.ja
./mget -f luckperms
```

Download a specific plugin version:

```bash
./mget -p luckperms -v 1.21.8 -s paper
```

### Options

- `-f, --find <name>` - Search for a plugin by name
- `-p, --plugin <slug>` - Plugin slug to download
- `-v, --version <version>` - Target Minecraft version
- `-s, --server <type>` - Server type (paper, spigot, bukkit, etc.)
- `-h, --help` - Show help message

## Project Structure

```
mget
├── README.md
├── LICENSE
└── src
    ├── main.c       - Entry point and CLI argument parsing
    ├── mget.h       - Header file with function declarations
    ├── network.c    - HTTP requests and API calls
    ├── search.c     - Plugin search functionality
    ├── download.c   - Plugin download functionality
    ├── json.c       - JSON parsing utilities
    ├── logs.c       - Logging utilities
    └── Makefile     - Build configuration
```

## License

MIT License

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
