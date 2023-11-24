# kstate
Simple Linux keyboard monitor

"kstate" is a simple quick and nasty program to monitor and control keyboard from Linux.
I had a problem with non-blocking check of full keboard state and also standard shortcuts manager was not enough for me
(ie shortcus not working on rdp sessions, hard to automate etc).

So - sit, worked two evenings - and here is the effect.
Haven't programmed in "c" for 20 years with a hook ... 

For handling keyboard kstate creates a symbolic link "/dev/input/kbd", then using this link checks kbd state with loop for sequent integer keyCodes (0-K_MAX).
If key is pressed - a file "/dev/input/kstate/&lt;keyCodesWithDots&gt;" is created (or deleted if key released).
If there is a file "/dev/input/kstate-shortcuts/&lt;keyCodesWithDots&gt;kst" with command to be run - the command is started in separate thread.

IE: read what keys are pressed from folder /dev/input/kstate (ie: leftCtrl+Down=29+108)
- then create file "29.108.kst" (growing sequence) in folder /dev/input/kstate-shortcuts
- put inside the file your command to be run (can have additional lines for commands or comments starting with "#")
That's it - working, multikey, low level, global shortcut on Your machine ! :)

That's all the story here.

If You want more You can pay me for my hours :)


## Build & development

Simple as hell:
```bash
./build.sh
```
or:
```bash
gcc -pthread kstate.c -o kstate
```

Run:
```bash
./run.sh
```
or:
```bash
 sudo ./kstate simple
```

## Initial idea

https://stackoverflow.com/questions/3649874/how-can-i-get-the-keyboard-state-in-linux
