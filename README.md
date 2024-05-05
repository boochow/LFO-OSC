# LFO-OSC
This oscillator produces a frequency that is 1/100th of the frequency corresponding to each MIDI note number.


The **shape** parameter allows you to adjust the scale from 1/100th to a full 1/1 ratio.
Additionally, you can mix the LFO sound with the note sound using the **shift-shape** parameter.


The **form** parameter allows you to select the waveform type, with options including sine wave, triangular wave, parabolic wave, sawtooth wave, ramp down wave, and square wave.

The **rtrgr** parameter enables the retrigger mode, which restarts the LFO with each note-on event. The default value(=1) is `off`.

This software is compatible with the logue SDK v1.1 and tested on NTS-1.
