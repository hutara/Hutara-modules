<h1>Hutara PsyOperator</h1> 




![git1](https://github.com/hutara/Hutara-VCV-Rack-Fm-Synth/assets/39126232/d5fdcf80-8240-4945-879e-10703f7ae752)
<h2>Psy Operator - VCV Rack Module</h2>
Psy Operator is a versatile and complex frequency modulation (FM) synthesis module for VCV Rack. This module combines multiple oscillators (sine, saw, triangle, and square) with advanced waveshaping and resampling capabilities to create a rich and varied palette of sounds. The module is designed to offer extensive control over the sound generation process, making it suitable for both experimental and traditional synthesis applications.

<h2>Features</h2>

- **Four Waveforms**: Sine, Saw, Triangle, and Square.
- **Frequency Modulation (FM)**: Modulate the frequency of oscillators.
- **Waveshapers**: Unique waveshaping options for each waveform.
- **Psychedelic Effects**: Additional modulation and shaping controls for creative sound design.
- **Resampling**: Adjustable resampling for richer harmonic content.
<h2>Parameters</h2>

## Pitch Controls
- **Sine Pitch**: Adjust the pitch of the sine wave oscillator.
- **Saw Pitch**: Adjust the pitch of the saw wave oscillator.
- **Triangle Pitch**: Adjust the pitch of the triangle wave oscillator.
- **Square Pitch**: Adjust the pitch of the square wave oscillator.
- **Pitch CV for All Osc**: Control pitch for all oscillators simultaneously.
<h2>Volume Controls</h2>

- **Sine Volume**: Adjust the volume of the sine wave output.
- **Saw Volume**: Adjust the volume of the saw wave output.
- **Triangle Volume**: Adjust the volume of the triangle wave output.
- **Square Volume**: Adjust the volume of the square wave output.

<h2>Modulation and Shaping</h2>

- **FM Input**: Modulate the frequency of oscillators.
- **FM Amount**: Adjust the amount of frequency modulation.
- **Sine Waveshaper**: Control the amount of waveshaping applied to the sine wave.
- **Saw Psychedelic**: Apply psychedelic effects to the saw wave.
- **Triangle Psychedelic**: Apply psychedelic effects to the triangle wave.
- **Psychedelic CV**: Control the overall intensity of the psychedelic effects.
<h2>Resampling</h2>

- **Resample**: Adjust the resampling factor to add harmonic complexity.
- **Resample Input**: CV control for resamplin

<h2>Inputs</h2>

- **Sine Pitch CV**: Voltage control for sine pitch.
- **Saw Pitch CV**: Voltage control for saw pitch.
- **Triangle Pitch CV**: Voltage control for triangle pitch.
- **Square Pitch CV**: Voltage control for square pitch.
- **FM CV**: Voltage control for FM input.
- **FM Amount CV**: Voltage control for FM amount.
- **Saw Psychedelic CV**: CV control for saw psychedelic effect.
- **Psychedelic CV for All**: Global CV control for all psychedelic effects.
- **Triangle Waveshaper CV**: CV control for triangle waveshaper effect.
- **Resample Input**: CV control for resampling.

<h2>Outputs</h2>

- **Sine Output**: Audio output for the sine waveform.
- **Saw Output**: Audio output for the saw waveform.
- **Triangle Output**: Audio output for the triangle waveform.
- **Square Output**: Audio output for the square waveform.
- **Final Output**: Combined output of all waveforms, including resampling effects.

<h2>Usage</h2>

- The Psy Operator module allows you to create a wide range of sounds by combining traditional waveforms with frequency modulation and unique psychedelic effects. 
- By adjusting the pitch, volume, and waveshaping parameters, and using the resampling and CV inputs, you can sculpt intricate and evolving sound textures.


<h1>HUTARA Random</h1> <h1>Random Gate and Sample&Hold </h1>

![hutara random module](https://github.com/hutara/Hutara-modules/assets/39126232/dff34189-8563-4f2b-be5b-1446c437e6fb)


# Hutara Random CV Module for VCV Rack

- This module generates random control voltages and gate signals for use in modular synthesis setups within VCV Rack.
 It provides a variety of parameters to control the randomness, gate speed, output voltage, and more.

## Features

- **Random CV Generation**: Generates random control voltages with adjustable strength, bipolar/unipolar modes, and **offset**.
- **Gate Signal Generation**: Produces gate signals with adjustable length and speed.
- **Clock/Trigger Input**: Responds to external clock or trigger signals.
- **Inverted Gate Output**: Provides an inverted gate output for more flexibility.

## Parameters

- **Output Voltage**: Controls the output voltage range for the sample and hold (S&H) circuit. Range: 0.25V to 10V.
- **Bipolar Mode**: Toggles between **bipolar** (±) and **unipolar** (0 to +) output voltage modes.
- **S&H Offset**: offset to the sampled voltage from the **S&H** circuit. Range: -5V to +5V.
- **Random Gate Speed**: Adjusts the speed of the random gate signal. Values: 1 (slow), 2 (medium), 3 (fast).
- **Gate Length**: Sets the length of the gate signal. Range: 0.01s to 1s.
- **Strength**: Controls the modulation strength of the random CV. Range: 1 to 10.
- **Trigger Division**: Divides the incoming clock/trigger signal by the specified value. Values: 1, 2, 3.


## Inputs

- **Clock Input**: Combined input for clock and trigger signals. A voltage of 1V or higher is considered active.

## Outputs

- **Gate Output**: Outputs the generated gate signal.
- **Inverted Gate Output**: Outputs the inverted gate signal. 
- **S&H Output**: Outputs the sampled and held random voltage.


## Installation

To add the Hutara Random CV module to your VCV Rack setup via the VCV Library:

1. Go to the VCV Library website.
2. Search for "Hutara Random CV".
3. Click the "Add" button to include it in your VCV Rack plugin list.
4. Open VCV Rack and update your plugins to install the module.

## Usage

1. Load the Hutara Random CV module in VCV Rack.
2. Connect a clock or trigger signal to the **On Input**.
3. Adjust the parameters to control the random CV and gate signals according to your needs.
4. Use the **S&H Output**, **Gate Output**, and **Inverted Gate Output** in your patch.

## YouTube Videos

### Video 1
[![Watch the video](https://img.youtube.com/vi/nuJy-Rm5gG0/0.jpg)](https://www.youtube.com/watch?v=nuJy-Rm5gG0)

### Video 2
[![Watch the video](https://img.youtube.com/vi/1Oe0zfCR0m0/0.jpg)](https://www.youtube.com/watch?v=1Oe0zfCR0m0)




