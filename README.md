# SOD: Context-Aware Audio Orchestrator for Linux

> **Status: alpha.** Core features work and are tested daily on Fedora (GNOME, PipeWire). DSP, audio routing and log-file output are not built yet. See [Roadmap](#roadmap) and [Known limitations](#known-limitations).

SOD is a Linux daemon written in C that watches the PipeWire audio graph, applies a volume profile to the system output, and automatically mutes and unmutes apps so only one plays at a time. It talks to PipeWire through its native C API and has no GUI: you pick a profile, and SOD keeps the audio session in order while it runs.

**Context-aware:** SOD's context is the active profile plus the playback state of each tracked app and the state of the microphone. It decides what to mute from that, so the right app stays audible. Switching profiles automatically is planned, not built yet.

## Features

- **Profiles:** `sod --profile <profile_name>` sets the system output volume and microphone state for the session.
- **Sink volume control:** SOD binds the audio sink's PipeWire device and writes stereo channel volumes through the `Route` param, clamped to a safe range. The GNOME master volume slider follows the profile.
- **Auto mute/unmute (ducking):** when a tracked app starts playing, other playing tracked apps are muted. When the newest app stops, apps that SOD muted are unmuted again. Mutes you make yourself are left alone.
- **Newest node wins:** starting playback in Chrome while Firefox is playing mutes Firefox and leaves Chrome audible.
- **Microphone awareness:** when an app opens a capture stream (for example a browser call), playing apps are muted. They come back when the last capture stream stops. Hardware mic nodes only report state and never trigger mutes.
- **Tracked apps:** Google Chrome, Firefox, GNOME Boxes and Spotify, held in a `tracked_apps[]` list.
- **Dynamic node tracking:** streams are discovered as they appear in the PipeWire registry and cleaned up when they disappear. This covers apps that create a new stream each time playback restarts.
- **State debouncing:** PipeWire nodes flicker between running and idle during normal playback. SOD settles each node's state with a per-node timer before it acts.
- **Clean shutdown:** SIGINT and SIGTERM stop the main loop and run an explicit teardown path.

## Profiles

| Profile   | Volume | Microphone |
|-----------|--------|------------|
| `library` | 80%    | off        |
| `office`  | 60%    | on         |
| `focus`   | 28%    | on         |
| `meeting` | 60%    | on         |
| `night`   | 1.5%   | off        |
| `gaming`  | 153%   | on         |

Profile values live in `include/profile.h`. Volumes are clamped to the 1.5% to 153% range before they are written to the sink. The microphone setting is applied once when an `Audio/Source` node is bound: the mic is muted if the profile has the mic off.

## How It Works

1. SOD connects to PipeWire and listens on the registry for new global objects.
2. Each new node is checked against `media.class` (`Stream/Output/Audio`, `Stream/Input/Audio`, `Audio/Source`, `Audio/Sink`) and, for playback streams, the `tracked_apps[]` list.
3. A matching node is bound and given a listener. SOD reads its current `Props` (volume, mute) without writing anything back from the callback.
4. Node state changes go through a per-node debounce timer on PipeWire's own event loop. When a state settles, SOD decides whether to duck other nodes or restore them.
5. Muting uses `pw_node_set_param()` with a mute-only `Props` pod (`sod_mute.c`). PipeWire has no generic pause, so "pause" means mute.
6. Profile volume is written to the audio sink: SOD binds the sink's `Device`, reads its `Route` params, and writes the new channel volumes back through `SPA_PARAM_Route` (`sod_sink.c`).
7. When a node is removed, SOD destroys its timer and listener, frees the slot, and keeps the tracker array packed.
8. Mic-aware muting: apps that capture audio (`Stream/Input/Audio`) trigger auto-mute of playing apps, tracked with a reference count so apps restore only when the last capture stream stops.

The read path (`sod_node_param`) is strictly read-only. All writes go through dedicated functions, which avoids feedback loops between the parameter callback and PipeWire.

## Project layout

```
SOD/
├── src/            # daemon, profile, mute, sink, mic and capture logic
├── include/        # blu3.h, profile.h, conf.h, sink.h, mic.h, capture.h, ...
├── systemd/        # systemd unit file
├── Makefile
├── LICENSE
└── README.md
```

## Dependencies

| Package | Purpose |
|---------|---------|
| `pipewire` | Audio server |
| `pipewire-devel` (`libpipewire-0.3-dev` on Debian/Ubuntu) | PipeWire C API headers |
| `systemd` | User service management |
| `gcc`, `make` | Build toolchain |

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

## Usage

Run SOD with a profile:

```bash
$ sod --profile night      # or: office, gaming, focus, meeting, library
```

Stop it with Ctrl+C. Once it is running, play audio in Chrome, Firefox, Spotify or Boxes. Starting a second app mutes the first, and the first is unmuted when the second stops. Open a call in a browser and playing apps are muted until the call's capture stream ends.

## Known limitations

- **Chrome and the microphone:** Chrome destroys its playback streams and creates one new stream when a mic opens, so per-tab restore is not possible. Firefox and Boxes restore correctly.
- **Tracker capacity:** the node tracker has a fixed capacity (`Track_Capacity`). Streams beyond it are not tracked.
- **Shutdown:** microphone controllers are not yet freed on exit, a small known leak under valgrind.
- **Mute, not pause:** PipeWire has no generic pause, so ducked apps keep playing silently.

## Roadmap

- [x] Profile selection (`--profile`)
- [x] Per-app mute and unmute with newest-wins ducking
- [x] Dynamic node tracking and cleanup
- [x] Microphone awareness (capture-stream auto-mute)
- [x] Sink volume control through the PipeWire device `Route` param
- [ ] Full teardown of microphone controllers on exit
- [ ] Per-profile bass EQ and use of media keys (`media.role`, title, artist) as context
- [ ] Full log file output
- [ ] Audio routing
- [ ] DSP (PipeWire echo-cancel module, noise cancellation)

## References

- [PipeWire API tutorial](https://docs.pipewire.org/page_overview.html)

## License

GPL-3.0, see [LICENSE](./LICENSE). Copyright (c) 2025 Usman O. Olanrewaju
