
<div align="right">
  <details>
    <summary >🌐 Language</summary>
    <div>
      <div align="center">
        <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=en">English</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=zh-CN">简体中文</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=zh-TW">繁體中文</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=ja">日本語</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=ko">한국어</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=hi">हिन्दी</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=th">ไทย</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=fr">Français</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=de">Deutsch</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=es">Español</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=it">Italiano</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=ru">Русский</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=pt">Português</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=nl">Nederlands</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=pl">Polski</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=ar">العربية</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=fa">فارسی</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=tr">Türkçe</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=vi">Tiếng Việt</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=id">Bahasa Indonesia</a>
        | <a href="https://openaitx.github.io/view.html?user=nooploop&project=piejam&lang=as">অসমীয়া</
      </div>
    </div>
  </details>
</div>

<!--
SPDX-FileCopyrightText: 2020-2026 Dimitrij Kotrev

SPDX-License-Identifier: CC0-1.0
-->

[![REUSE status](https://api.reuse.software/badge/github.com/nooploop/piejam)](https://api.reuse.software/info/github.com/nooploop/piejam)

# PieJam
**PieJam** is a flexible, touchscreen-first audio mixer for Linux.

It was designed specifically for the 7″ Raspberry Pi touchscreen, with the goal of running as a standalone mixer on a Raspberry Pi paired with a USB audio interface. While this is the primary target, PieJam is not limited to the Raspberry Pi and can run on other Linux systems as well.

PieJam supports dynamic audio routing, per-channel processing, MIDI control, and session recording, making it suitable for live performance, rehearsal setups, and embedded audio systems.

For building a complete Raspberry Pi–based system image, see the accompanying [PieJam OS](https://github.com/nooploop/piejam_os) repository.

More [documentation](https://piejam.readthedocs.io/en/latest/)

## Features

### Mixing & Routing
* Dynamic input/output configuration
* Flexible channel routing
* Panning, stereo balance, volume
* Mute and solo

### Effects & Processing
* Per-channel FX chains
* Built-in modules: Dual Pan, Filter, Utility
* Analysis tools: Oscilloscope, Spectrum Analyzer, Tuner
* LADSPA plugin support

### Control & Automation
* MIDI CC/Pitchbend parameter control

### Sessions
* Session recording
* Session management
