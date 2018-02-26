<!---

To maintain this document use the following markdown:

# First level heading
## Second level heading
### Third level heading

- First level bullet point
 - Second level bullet point
  - Third level bullet point

`inline code`

``` preformatted text etc.  ```

[hyperlink](url for the hyperlink)

Any valid HTML can also be used.

--->
# CSOUND VERSION 6.101RELEASE NOTES

There has been a great amount of internal reorganisation, which should
not affect most users.  Some components are now independently managed
and installablevia the new package manager.  The realtime optaion is
now considered stable and has the "experim`ental" tag removed.

-- The Developers

## USER-LEVEL CHANGES

### New opcodes

- loscilphs, loscil3phs are like loscil but return the phase in
addition to the audio.

### New Gen and Macros

- 

### Orchestra

- 

### Score

- 

### Options

- 

### Modified Opcodes and Gens

- print, printk2 now take an additionl optional argument which if
  non-zero causes the k-variable name to be printed as well as the value.

### Utilities

-

### Frontends

- icsound:

- csound~:

- csdebugger:

- Emscripten: 

- CsoundQt: 

### General Usage

## Bugs Fixed

- linen was reworked to allow for long durations and overlaps

- resetting csound caused a crash if Lua opcodes were used; fixed

## SYSTEM LEVEL CHANGES

- OPCODE6DIR{64} now can contain a colon-separated list of directories

### System Changes

-

### Translations

- As ever the French translations are complete.

- 

### API

- 

### Platform Specific

- iOS

 -

- Android

 -

- Windows

- OSX

- GNU/Linux



==END==


========================================================================
mmit 1b10b9e8aacb5e6b9240ce418741a1346909eed3
Author: veplaini <victor.lazzarini@nuim.ie>
Date:   Fri Feb 9 10:00:53 2018 +0000

**end**