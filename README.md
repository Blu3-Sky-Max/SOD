# SOD — Sound Output Daemon

A Linux daemon written in C that automatically adjusts audio volume based on mood presets when wired headphones are detected.

---

## Overview

SOD listens for wired headphone plug events and sets the system volume through PipeWire to a predefined mood level. No GUI, no config files at runtime — plug in, volume is set, done.

**Author:** Usman O. Olanrewaju  
**Language:** C  
**License:** MIT  
**Platform:** Linux (PipeWire + systemd)

---

## Mood Presets

| Mood   | Volume |
|--------|--------|
| normal | 50%    |
| happy  | 60%    |
| sad    | 85%    |

---

## How It Works

SOD detects wired headphone connection events, reads the active mood preset, then applies the corresponding volume via `wpctl`.

The non-obvious part: `wpctl` is a user-session service communicating over D-Bus. Running it as root fails — root has no PipeWire session. SOD resolves this by detecting the active user session via `loginctl`, extracting the correct `DBUS_SESSION_BUS_ADDRESS`, and invoking `wpctl` through `runuser` with the session environment injected.

---

## Dependencies

| Package | Purpose |
|---------|---------|
| `pipewire` | Audio server |
| `wireplumber` | PipeWire session manager — provides `wpctl` |
| `systemd` | Session detection via `loginctl` |
| `gcc`, `make` | Build toolchain |

---

## Build & Installation

```bash
$ git clone https://github.com/Blu3-Sky-Max/SOD.git
$ cd SOD
$ make
$ sudo make install
```

To remove:

```bash
$ sudo make uninstall
```

---

## Usage

Set the mood before plugging in:

```bash
$ sod --mood normal
$ sod --mood happy
$ sod --mood sad
```

SOD applies the active preset on the next headphone event.

---

## License

MIT — see [LICENSE](./LICENSE).  
Copyright (c) 2025 Usman O. Olanrewaju
