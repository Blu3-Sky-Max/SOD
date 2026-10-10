## SOD: Context-Aware Audio Orchestrator for Linux
 

A Linux daemon written in C that watches the PipeWire audio graph, applies a volume profile to app audio streams, and automatically mutes and unmutes apps so only one plays at a time. 

SOD connects to PipeWire through its native C API, tracks the audio streams of selected applications, and controls them per app. It has no GUI. You pick a profile, and SOD keeps the session in order while it runs.

**Context-aware:** SOD's context is the active profile plus the playback state of each tracked app. It chooses what to mute from that, so the right app stays audible. Switching profiles automatically is planned, not built yet.

## Features
 

- **Profiles:** `sod --profile <profile_name>` sets a target volume for tracked app streams.

- **Auto mute/unmute (ducking):** when a tracked app starts playing, other playing tracked apps are muted. When the newest app stops, apps that SOD muted are unmuted again. Mutes you make yourself are left alone.

- **Newest nodes wins:** starting playback in Chrome while Firefox is playing mutes Firefox and leaves Chrome audible.

- **Tracked apps:** Google Chrome, Firefox and GNOME Boxes, held in a `tracked_apps[]` list.

- **Dynamic node tracking:** streams are discovered as they appear in the PipeWire registry and cleaned up when they disappear. This covers apps that create a new stream each time playback restarts.

- **State debouncing:** PipeWire nodes flicker between running and idle during normal playback. SOD settles each node's state with a per-node timer before it acts.


## Profiles

| Profile   | Volume | Microphone | 
|-----------|--------|------------|
| `library` | 80%    |   OFF      |
| `office`  | 30%    |   ON       |
| `focus`   | 45%    |   ON       |
| `meeting` | 60%    |   ON       |
| `night`   | 10%    |   OFF      | 
| `gaming`  | 100%   |   ON       |


Profile values are defined in `include/profile.h`. The microphone setting
is applied once when an `Audio/Source` node is bound: the mic is muted if
the profile has the mic off.

## How It Works
 

1. SOD connects to PipeWire and listens on the registry for new global objects.
2. Each new node is checked against `media.class == "Stream/Output/Audio"` and the `tracked_apps[]` list.
3. A matching node is bound and given a listener. SOD reads its current `Props` (volume, mute) and applies the profile volume once at bind time.
4. Node state changes go through a per-node debounce timer on PipeWire's own event loop. When a state settles, SOD decides whether to duck other nodes or restore them.
5. Muting uses `pw_node_set_param()` with a mute-only `Props` pod (`sod_mute.c`). PipeWire has no generic pause, so "pause" means mute.
6. When a node is removed, SOD destroys its timer and listener, frees the slot, and keeps the tracker array packed.
7. Mic-aware muting: apps that capture audio (`Stream/Input/Audio`, such as a  browser using the microphone) trigger auto-mute of playing apps. Apps restore when the last capture stream stops. Source/Hardware mic nodes only report state and never trigger mutes.
 
The read path (`sod_node_param`) is strictly read-only. All writes go through dedicated functions, which avoids feedback loops between the parameter callback and PipeWire.

## Project layout

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
$ sod --profile night
```
****OR****
```bash 
$ sod --profile office
```
****OR****
```bash
$ sod --profile gaming
```

Once it is running, play audio in Chrome, Firefox or Boxes. Starting a second app mutes the first, and the first is unmuted when the second stops.



## Roadmap

- [x] Profile selection (`--profile`) with volume applied at node bind
- [x] Per-app mute and unmute with newest-wins ducking
- [x] Dynamic node tracking and cleanup
- [ ] Sink and source control, including the microphone
- [ ] Full log file output
- [ ] Audio routing
- [ ] DSP (PipeWire echo-cancel module, noise cancellation)


## References


- [PipeWire API tutorial](https://docs.pipewire.org/page_overview.html)





## License

GPL-3.0-- [LICENSE](./LICENSE) Copyright (c) 2025 Usman O. Olanrewaju 

