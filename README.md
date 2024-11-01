## Preview (Monocle)
![example_image_monocle](https://github.com/DerjenigeUberMensch/vox-wm/blob/Experimental/images/example.png)

## Table of Contents
1. [Requirements](#Requirements)  
2. [Usage](#Usage)  
3. [Compiling](#Compiling)  
4. [Uninstalling](#Uninstalling)  
5. [How to Run](#How-to-Run)
6. [Mousebinds](#Mousebinds)
7. [Keybinds](#Keybinds)
8. [Troubleshoot](#Troubleshoot)  

<a name="headers"/>

## Early Release
This build is ***not*** fully feature ready.

## Why Use this.
- **Low Memory Usage (~4-7 MiB) (When compiled in Release Mode)**
- **Low CPU usage (~.1-2%) idle**
- **Low CPU usage when doing expensive stuff (mapping windows) (~4-10%)**

## Why Not Use this.
- **Doesnt use C++**
- **XCB**

## Requirements
- **dmenu**
- **st**
- **xorg**
- **xorg-xinit**
- **libxcb**
- **xcb-proto**
- **xcb-util**
- **xcb-util-errors**
- **xcb-util-cursor**
- **xcb-util-image**
- **xcb-util-keysyms**
- **xcb-util-wm**

**Pacman && yay**
```
sudo pacman -S --needed dmenu xorg xorg-xinit libxcb xcb-util xcb-util-errors xcb-util-cursor xcb-util-image xcb-util-keysyms xcb-util-wm && yay -S st
```

## Usage 
To **use** the wm you must first **compile it**.
Afterwards you must put `exec vox-wm` in your `~/.xinitrc` file. **See Below.**


## Compiling
1. Clone this repository.
```
git clone https://github.com/DerjenigeUberMensch/vox-wm.git
```
2. Change Directory to vox-wm.
```
cd vox-wm
```
3. Checkout the Experimental branch. (Legacy support)
```
git checkout origin/Experimental
```
4. Configure it See **Configuration** (Optional)
5. Compile using Make
```
make
```
6. Install on system, install location '/usr/local/bin'
```
sudo make install
```
7. Done.

## Uninstalling
1. Change Directory to vox-wm.
```
cd vox-wm
```
2. Uninstall using Make
```
sudo make uninstall
```

## How to Run
1. Compile, See [Compiling](#Compiling).
2. Choose option(s) below.
3. Locate binary 
    - if installed should be **"vox-wm"**
    - else should be located under **"bin/release/"** folder as **"vox-wm"**

### Normal Usage
4. Put **"exec YOUR_BINARY_LOCATION"** in **~/.xinitrc** or similiar XServer configuration.
    - if it doesnt work make sure that no window-manager/desktop manager is run before.

### Xephyr (Debug/Testing Usage)
4. Start a Xephyr server.
    - Example: `Xephyr :1 -ac c 25 +xinerama -resizeable -screen 680x480 &`
5. After locating the binary location make sure you are in the correct display server.
    - Example: `export DISPLAY=:1`
6. After setting the correct external xserver display variable simply run the binary file.
    - Example: `./bin/release/vox-wm` or `vox-wm`

## Configuration

****Currently Configuration is not supported in this version of vox-wm****

## Mousebinds
These are the default mousebinds currently used within vox-wm, they are not planned to change (defaults), but will be when or if a configuration is implemented (NOT YET SUPPORTED)

- RMB: Right Mouse Button.
- LMB: Left Mouse Button.
- MMB: Middle Mouse Button.

| Button | Action |
| ------ | ------ |
| SUPER+RMB | Activates Window Resizing of the Clicked Window. (Must be Held) |
| SUPER+ALT+RMB | See above, does not apply ANY window restrictions, these include: minimum size, maximimum size, base size, inc size, etc... |
| SUPER+LMB | Activates Window Moving of the Clicked Window. (Must be Held) |

## Keybinds
These are the default keybinds currently used within vox-wm, they are not currently planned to change, but will be when or if a configuration is implemented (NOT YET SUPPORTED)

- SUPER: The "Windows" key (next to fn/ctrl, usually the left side of the keyboard).
- RETURN: The Enter key (Right above right SHIFT, Not the numlock Enter key)

| Key | Action |
| ----- | ----- |
| SUPER+n | Debug/Unused |
| SUPER+d | exec dmenu |
| SUPER+RETURN | exec st |
| SUPER+e | exec thunar |
| SUPER+b | Hide/Show Task bar (if applicable) |
| SUPER+SHIFT+q | Exit Current Window |
| CTRL+ALT+q | Terminate Current Window |
| SUPER+w | Maximize Current Window |
| SUPER+SHIFT+p | Quit (exit window manager) |
| SUPER+CTRL+p | Soft Restart |
| CTRL+ALT+p | Hard Restart |
| SUPER+CTRL+z | Make the current Window Sticky |
| SUPER+z | Change Layout To Tiling |
| SUPER+x | Change Layout To Floating |
| SUPER+c | Change Layout To Monocle |
| SUPER+g | Change Layout To Grid |
| SUPER+1 | Set Current Desktop To 1 |
| SUPER+2 | Set Current Desktop To 2 |
| SUPER+3 | Set Current Desktop To 3 |
| SUPER+4 | Set Current Desktop To 4 |
| SUPER+5 | Set Current Desktop To 5 |
| SUPER+6 | Set Current Desktop To 6 |
| SUPER+7 | Set Current Desktop To 7 |
| SUPER+8 | Set Current Desktop To 8 |
| SUPER+9 | Set Current Desktop To 9 |
| SUPER+0 | Set Current Desktop To 10 |

### Base Calls
These are actions that expected supporting basic keyboard support.
| Key | Action |
| ----- | ----- |
| F11 | Fullscreen On The Current Window |
| Increase Volume | Increase Volume Controls |
| Decrease Volume | Decrease Volume Controls |
| Increase Brightness | Increase Brightness Controls |
| Decrease Brightness | Decrease Brightness Controls |
| Mute | Sound Mute Functionality |
| Play | Inverts Playing Status (play-pause) |
| Pause |  Inverts Playing Status (play-pause) |
| Next | Next Audio Playback |
| Prev | Previous Audio Playback |



## Troubleshoot
This is an **_experimental_** build and may contain bugs,
Screen tearing can be mitigated by using a compositor or by enabling it in your driver setting

If you have any **major** **issues** with the **current** commit you can **revert** to a **previous commit** for a generally stabler version.
Or you can open an **[issue](https://github.com/DerjenigeUberMensch/vox-wm/issues)** about it.

## Planned Features
- Panel that user can just click buttons or type values to configure windows (kinda like a dev panel).
- Fully Extend XCB-TRL to use other features.
- More modularity 
- Less Complex Code Base
- Better Documentatin (Likely when I finish implementing the usability part of the Window manager (AKA when I switch over to this.))
