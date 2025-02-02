# rustblocks-async

## About
dwmblocks-async written in Rust

## Installation

``cargo build --release``

``config.toml`` default location is:

``$HOME/.config/rustblocks/config.toml``
``$XDG_CONFIG_HOME/.config/rustblocks/config.toml``

## Usage
``rustblocks [-v] [--trace] [--version] [<command>] --config [<args>]``

asynchronous bar for dwm

Options:
  ``-v``, ``--verbose``     set log level to INFO
  ``--trace``           set log level to TRACE
  ``--version``         version
  ``--help``            display usage information

Commands:
  ``run``               Run the bar
  ``hit``               Asynchronously update the block specified in the ID
