# clicksound

A sound on every mouse click, set from the Windows sound settings.

Windows has no click sound of its own. This adds two events, `Mouse Click Down`
and `Mouse Click Up`, to Control Panel → Sound → Sounds, and a small helper that
plays them. Change or clear them there like any other Windows sound; the helper
picks it up right away. The "No Sounds" scheme turns it off.

The helper is plain C with no window and no runtime, and sits at about 0.3 MB of
memory. It starts at sign-in through a scheduled task.

## structure

```
clicksound.c     low-level mouse hook, reads the sound scheme from the registry
build.bat        builds clicksound.exe with MSVC
install.ps1      copies exe and sounds, registers events and the logon task
uninstall.ps1    removes all of it
```

## run

```bat
build.bat
```

```powershell
.\install.ps1 -Down down.wav -Up up.wav
```

Both need an admin shell. No sounds are included; bring your own short PCM
`.wav` files. Left and right buttons are handled, down and up separately.

`uninstall.ps1` takes everything back out.
