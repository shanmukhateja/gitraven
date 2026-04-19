# GitRaven

A free, open source tool for managing Git repositories built for fun.

> This project is a work-in-progress and contains bugs.
> Please do not use it in production environment(s).

![](https://github.com/shanmukhateja/gitraven/raw/main/resources/images/mainwindow.png)

## Goals

1. Be a simple tool for managing source code.
2. No "advanced" features ("Issues", "Pull Requests", etc.) are allowed.
3. Always use `libgit2` for functionality.

### Future Goals

1. Push/Pull from remotes.
2. Manage Git Tags

## Requirements

- Qt6
- libgit2
- libmagic
- CMake >= 3.16
- QtHttpServer
- QtWebEngine
- monaco-editor

## Usage

1. Clone this repository.
2. Build the project.
3. Run `cmake --install` so Monaco editor is copied to `$HOME/local/share/gitraven-qt/` directory.
4. Launch the application.

## License

MIT
