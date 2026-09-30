# SOD — Sound Output Daemon

A Linux daemon written in C that watches the PipeWire audio graph, applies a volume profile to app audio streams, and automatically mutes and unmutes apps so only one plays at a time.

**Author:** Usman O. Olanrewaju  
**Language:** C  
**License:** GPL-3.0  
**Platform:** Linux (PipeWire + systemd)  
**Last updated:** 2026/09/30

---

## Table of Contents

1. [Overview](#1-overview)
2. [Features](#2-features)
3. [Profiles](#3-profiles)
4. [How It Works](#4-how-it-works)
5. [Dependencies](#5-dependencies)
6. [Build & Installation](#6-build--installation)
7. [Usage](#7-usage)
8. [Known Limitations](#8-known-limitations)
9. [Roadmap](#9-roadmap)
10. [License](#10-license)

---

## 1. Overview

SOD connects to PipeWire through its native C API, tracks the audio streams of selected applications, and controls them per app. It has no GUI. You pick a profile, and SOD keeps the session in order while it runs.

> **Note:** Earlier versions of SOD set volume from wired-headphone events through `wpctl`. That design has been replaced. SOD now talks to PipeWire directly and works per application stream.

---

## 2. Features

- **Profiles:** `sod --profile <name>` sets a target volume for tracked app streams.
- **Auto mute/unmute (ducking):** when a tracked app starts playing, other playing tracked apps are muted. When the newest app stops, apps that SOD muted are unmuted again. Mutes you make yourself are left alone.
- **Newest wins:** starting playback in Chrome while Firefox is playing mutes Firefox and leaves Chrome audible.
- **Tracked apps:** Google Chrome, Firefox and GNOME Boxes, held in a `tracked_apps[]` list.
- **Dynamic node tracking:** streams are discovered as they appear in the PipeWire registry and cleaned up when they disappear. This covers apps that create a new stream each time playback restarts.
- **State debouncing:** PipeWire nodes flicker between running and idle during normal playback. SOD settles each node's state with a per-node timer before it acts.
- **Log file:** initial logging support (`include/Log.h`).

---

## 3. Profiles

| Profile   | Volume |
|-----------|--------|
| `night`   | 10%    |
| `office`  | 30%    |
| `focus`   | 45%    |
| `meeting` | 60%    |
| `library` | 80%    |
| `gaming`  | 100%   |

Profile values are defined in `include/blu3.h`.

> **Note:** Profiles also carry a microphone setting. Microphone control is not applied yet. See the [Roadmap](#9-roadmap).

---

## 4. How It Works

1. SOD connects to PipeWire and listens on the registry for new global objects.
2. Each new node is checked against `media.class == "Stream/Output/Audio"` and the `tracked_apps[]` list.
3. A matching node is bound and given a listener. SOD reads its current `Props` (volume, mute) and applies the profile volume once at bind time.
4. Node state changes go through a per-node debounce timer on PipeWire's own event loop. When a state settles, SOD decides whether to duck other nodes or restore them.
5. Muting uses `pw_node_set_param()` with a mute-only `Props` pod (`sod_mute.c`). PipeWire has no generic pause, so "pause" means mute.
6. When a node is removed, SOD destroys its timer and listener, frees the slot, and keeps the tracker array packed.

The read path (`sod_node_param`) is strictly read-only. All writes go through dedicated functions, which avoids feedback loops between the parameter callback and PipeWire.

### Project layout

```
SOD/
├── src/            # daemon, profile and mute logic (sod_profile.c, sod_mute.c, ...)
├── include/
│   ├── blu3.h      # profiles, structs, shared declarations
│   └── Log.h       # log file support
├── systemd/        # systemd unit file
├── Makefile
├── LICENSE
└── README.md
```

---

## 5. Dependencies

| Package | Purpose |
|---------|---------|
| `pipewire` | Audio server |
| `pipewire-devel` (`libpipewire-0.3-dev` on Debian/Ubuntu) | PipeWire C API headers |
| `systemd` | User service management |
| `gcc`, `make` | Build toolchain |

---

## 6. Build & Installation

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

## 7. Usage

Run SOD with a profile:

```bash
$ sod --profile night
$ sod --profile focus
$ sod --profile gaming
```

Once it is running, play audio in Chrome, Firefox or Boxes. Starting a second app mutes the first, and the first is unmuted when the second stops.

---

## 8. Known Limitations

- **Tracker capacity:** SOD tracks a fixed number of nodes at once (currently 3). When the tracker is full, additional nodes are dropped, and muting breaks with 3 or more simultaneously muted nodes.
- **Volume target:** profile volume is applied to each app's stream, not to the system sink. The desktop's master volume slider does not move when a profile is applied.
- **Profile settings not yet applied:** microphone, echo cancellation and noise cancellation are defined in the profile specs but not implemented.
- **Tracked apps are hard-coded:** adding an app means editing `tracked_apps[]` and rebuilding.

---

## 9. Roadmap

- [x] Profile selection (`--profile`) with volume applied at node bind
- [x] Per-app mute and unmute with newest-wins ducking
- [x] Dynamic node tracking and cleanup
- [ ] Raise or replace the tracker capacity limit
- [ ] Sink and source control, including the microphone
- [ ] Full log file output
- [ ] Audio routing
- [ ] DSP (PipeWire echo-cancel module, noise cancellation)

---

## 10. License

GPL-3.0 — see [LICENSE](./LICENSE).  
Copyright (c) 2025 Usman O. Olanrewaju
