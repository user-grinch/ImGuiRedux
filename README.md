# ImGuiRedux
[Dear ImGui](https://github.com/ocornut/imgui) bindings for [CLEORedux](https://re.cleo.li/).

Example script [here](https://github.com/user-grinch/ImGuiRedux/blob/master/resource/imgui_test.js).
Download from [here](https://github.com/user-grinch/ImGuiRedux/releases)

## Supported Games
*Classics only*
- GTA III 
- GTA Vice City
- GTA San Andreas

## Installation Notes

### GTA III/ GTA VC
1. Install [SilentPatch](https://gtaforums.com/topic/669045-silentpatch/) & [d3d8to9 Wrapper](https://github.com/crosire/d3d8to9/releases)

### GTA SA
1. Install [SilentPatch](https://gtaforums.com/topic/669045-silentpatch/) 

### Unknown hosts
*Using ImGuiRedux plugin with CLEORedux Unknown host*
1. Input (Mouse & Keyboard) won't work properly.
2. You need to hook into game loop yourself (May not work properly with CLEORedux SelfHost).
3. Only games with Dx9 & Dx11 backend are supported! If you're on a older game with d3d8 or earlier use a Dx9 wrapper.
4. Get the x64 version for 64 bit games

More info [here](https://re.cleo.li/docs/en/embedding.html)

