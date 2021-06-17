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
* Fx chain per mixer channel
* Support for LADSPA plugins
* Parameter control through MIDI CC
* Recorder

#### Usage example
![Usage example](doc/images/usage.png "Usage example")

## ChangeLog
* v0.7.0 - Recorder
* v0.6.0 - New modules: Oscilloscope and Spectrum Analyzer
* v0.5.0 - MIDI CC parameter control. Flexible routing.
* v0.4.0 - Fx chain and LADSPA fx plugins support.
* v0.3.0 - Mute and solo. Modular audio engine.
* v0.2.0 - In/out configuration. Panning and balance.
* v0.1.0 - Initial release. Basic mixer.
