<!--
SPDX-FileCopyrightText: 2020 Dimitrij Kotrev

SPDX-License-Identifier: CC0-1.0
-->

[![REUSE status](https://api.reuse.software/badge/github.com/nooploop/piejam)](https://api.reuse.software/info/github.com/nooploop/piejam)

# PieJam
PieJam is a simple audio mixer. The user interface was designed to perfectly fit the 7" Raspberry Pi
touchscreen. The initial idea was to run it as a standalone application on a Raspberry Pi
with an attached USB audio interface. But it should be possible to use on any other Linux platform.

See accompanying [PieJam OS](https://github.com/nooploop/piejam_os) repository
on how to create a Linux image for the Raspberry Pi.

More [documentation](https://piejam.readthedocs.io/en/stable/)

## Features
* Dynamic configuration of inputs and outputs
* Panning, stereo balance and volume controls
* Mute and solo
* Flexible routing between mixer channels
* Sends
* Fx chain per mixer channel
* Fx modules:
    * Dual Pan
    * Filter
    * Oscilloscope
    * Spectrum Analyzer
    * Tuner
    * Utility
* Support for LADSPA plugins
* Parameter control through MIDI CC
* Session recorder
