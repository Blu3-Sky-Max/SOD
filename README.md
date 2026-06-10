# SOD — Sound Output Daemon

A Linux daemon written in C that manages audio volume automatically based on mood presets when wired headphones are detected.

---

## Table of Contents

1. [Overview](#1-overview)
2. [How It Works](#2-how-it-works)
3. [Mood Presets](#3-mood-presets)
4. [Dependencies](#4-dependencies)
5. [Build & Installation](#5-build--installation)
6. [Usage](#6-usage)
7. [Configuration](#7-configuration)
8. [Key Design Decisions](#8-key-design-decisions)
9. [License](#9-license)

---

## 1. Overview

SOD is a lightweight C daemon that listens for wired headphone plug/unplug events and automatically adjusts system audio volume to a predefined mood preset via PipeWire. No GUI, no bloat — it runs silently in the background and does exactly one job.

**Author:** Usman O. Olanrewaju  
**Language:** C  
**License:** MIT  
**Platform:** Linux (PipeWire + systemd)

---

## 2. How It Works

```
headphone plug event detected
        │
        ▼
  read active mood preset
        │
        ▼
  detect active user session
  via loginctl
        │
        ▼
  invoke wpctl via runuser
  with correct D-Bus environment
        │
        ▼
  volume set on PipeWire sink
```

SOD detects wired headphone connection events, reads the currently configured mood preset, then calls `wpctl set-volume` to apply the corresponding volume level.

The critical detail: `wpctl` communicates over D-Bus as a user-session service under PipeWire. Running it directly as root breaks this — the D-Bus session doesn't exist in the root context. SOD solves this by:

1. Detecting the active user session with `loginctl`
2. Extracting the correct `DBUS_SESSION_BUS_ADDRESS` for that session
3. Invoking `wpctl` via `runuser` with the session environment injected

This keeps the daemon functional without patching PipeWire or relaxing permissions.

---

## 3. Mood Presets

| Mood   | Volume |
|--------|--------|
| normal | 50%    |
| happy  | 60%    |
| sad    | 85%    |

---

## 4. Dependencies

| Dependency | Purpose |
|------------|---------|
| `pipewire` | Audio server — SOD controls volume through its session |
| `wireplumber` | PipeWire session manager — provides `wpctl` |
| `systemd` / `loginctl` | Used to detect the active user session at runtime |
| `gcc` | Build toolchain |
| `make` | Build system |

---

## 5. Build & Installation

```bash
$ git clone https://github.com/Blu3-Sky-Max/SOD.git
$ cd SOD
$ make
$ sudo make install
```

`make install` places the daemon binary in the appropriate system path and sets up the necessary permissions.

To remove:

```bash
$ sudo make uninstall
```

---

## 6. Usage

Once installed, SOD runs as a background daemon. Plug in wired headphones and the volume adjusts automatically to the active mood preset.

To set the active mood before plugging in:

```bash
$ sod --mood happy
$ sod --mood sad
$ sod --mood normal
```

> **Note:** Mood selection persists until changed. SOD applies it on the next detected headphone event.

---

## 7. Configuration

Configuration is minimal by design. Mood presets and their volume values are defined in the source and can be adjusted before building. The defaults are:

```c
#define VOLUME_NORMAL 0.50
#define VOLUME_HAPPY  0.60
#define VOLUME_SAD    0.85
```

---

## 8. Key Design Decisions

**Why C?**  
SOD is a systems-level daemon. C gives direct control over process management, signal handling, and system calls without runtime overhead. It also serves as a practical learning project aligned with deeper systems programming work.

**Why not run wpctl directly as root?**  
PipeWire is a user-session service. `wpctl` communicates over D-Bus, which is scoped per-user. Running `wpctl` as root fails because the root environment has no active PipeWire/D-Bus session. SOD detects the active user session via `loginctl` and passes the correct environment to `runuser`, keeping everything within the user session where PipeWire actually lives.

**Why MIT license?**  
Maximum freedom. No copyleft restrictions — anyone can use, fork, modify, or embed this without conditions beyond attribution.

---

## 9. License

MIT License — see [LICENSE](./LICENSE) for full terms.

Copyright (c) 2025 Usman O. Olanrewaju
