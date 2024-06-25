#include "plugin.hpp"
#include "osdialog.h"
#include <iostream>
#include <app/ModuleWidget.hpp>

struct FmOperator : Module {
    float phaseSine = 0.f;
    float phaseSaw = 0.f;
    float phaseTriangle = 0.f;
    float phaseSquare = 0.f;
    const float fmScale = 33.23;
    float lpFilterSine = 0.0f;
    float lpFilterSaw = 0.0f;
    float lpFilterSQUARE = 0.0f;
    const float lpFilterFreq = 8000.0f;
    const float distortionGain = 2.0f;
    std::vector<float> wavetable;
    const float psychedelicCVKnobScale = 5.0f;  // Adjust the scale as needed
    float psychedelicCVKnobValue = 0.0f;

    enum ParamId {
        PITCH_PARAM_SINE,
        PITCH_PARAM_SAW,
        PITCH_PARAM_TRIANGLE,
        PITCH_PARAM_SQUARE,
        FM_PARAM,
        SINE_WAVESHAPER_PARAM, // Changed parameter name
        SAW_WAVESHAPER_PARAM,
        PSYCHEDELIC_PARAM_TRIANGLE,
        PSYCHEDELIC_CV_KNOB_PARAM,
        FM_AMOUNT_PARAM,
        VOLUME_PARAM_SINE,
        VOLUME_PARAM_SAW,
        VOLUME_PARAM_TRIANGLE,
        VOLUME_PARAM_SQUARE,
        WAVEFOLD_PARAM,
        PARAMS_LEN
    };

    enum InputId {
        PITCH_INPUT_SINE,
        PITCH_INPUT_SAW,
        PITCH_INPUT_TRIANGLE,
        PITCH_INPUT_SQUARE,
        FM_INPUT,
        FM_AMOUNT_INPUT,
        PSYCHEDELIC_CV_INPUT_SAW,
        PSYCHEDELIC_CV_INPUT_FOR_All,
        PSYCHEDELIC_CV_INPUT_TRIANGLE,
        SQUARE_WAVEFOLD_CV_INPUT, 
        INPUTS_LEN
    };

    enum OutputId {
        SINE_OUTPUT,
        SAW_OUTPUT,
        TRIANGLE_OUTPUT,
        SQUARE_OUTPUT,
        FINAL_OUTPUT,
        OUTPUTS_LEN,
    };
    enum LightId {
        MANDALA_LIGHT,  // Existing light ID
        YOUR_LIGHT,     // Add your light ID
        LIGHTS_LEN
    };
    FmOperator() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN);

        configParam(PSYCHEDELIC_CV_KNOB_PARAM, -0.5f, 1.f, 0.f, "Psychedelic CV ");
        configParam(PITCH_PARAM_SINE, -4.f, 4.f, 0.f, "Sine Pitch");
        configParam(PITCH_PARAM_SAW, -4.f, 4.f, 0.f, "Saw Pitch");
        configParam(PITCH_PARAM_TRIANGLE, -4.f, 4.f, 0.f, "Triangle Pitch");
        configParam(PITCH_PARAM_SQUARE, -4.f, 4.f, 0.f, "SQUARE Pitch");
        configParam(FM_PARAM, -0.7, 1.23, 0.f, "FM Input");
        configParam(SINE_WAVESHAPER_PARAM, -0.5, 1.5f, 0.f, "Sine Waveshaper"); // Changed parameter name
        configParam(SAW_WAVESHAPER_PARAM, -0.5f, 1.f, 0.f, "Saw Psychedelic");
        configInput(PSYCHEDELIC_CV_INPUT_FOR_All, "CV_INPUT_FOR_All");
        configParam(PSYCHEDELIC_PARAM_TRIANGLE, -0.5f, 1.f, 0.f, "Triangle Psychedelic");
        configParam(FM_AMOUNT_PARAM, -0.5f, 0.6f, 0.f, "FM Amount");
        configParam(VOLUME_PARAM_SINE, 0.f, 1.f, 1.f, "Sine Volume");
        configParam(VOLUME_PARAM_SAW, 0.f, 1.f, 1.f, "Saw Volume");
        configParam(VOLUME_PARAM_TRIANGLE, 0.f, 1.f, 1.f, "Triangle Volume");
        configParam(VOLUME_PARAM_SQUARE, 0.f, 1.f, 1.f, "SQUARE Volume");
        configParam(WAVEFOLD_PARAM, 0.0f, 1.0f, 0.5f, "Wavefolding");
        configInput(SQUARE_WAVEFOLD_CV_INPUT, "Square Wavefold CV Input");
        configInput(PITCH_INPUT_SINE, "Sine Pitch CV");
        configInput(PITCH_INPUT_SAW, "Saw Pitch CV");
        configInput(PITCH_INPUT_TRIANGLE, "Triangle Pitch CV");
        configInput(PITCH_INPUT_SQUARE, "SQUARE Pitch CV");
        configInput(FM_INPUT, "FM CV");
        configInput(FM_AMOUNT_INPUT, "FM Amount CV");
        configInput(PSYCHEDELIC_CV_INPUT_SAW, "Saw Psychedelic CV");
        configInput(PSYCHEDELIC_CV_INPUT_TRIANGLE, "Triangle Psychedelic CV");
        configInput(PSYCHEDELIC_CV_INPUT_TRIANGLE, "Triangle Psychedelic CV");
        configOutput(SINE_OUTPUT, "Sine Output");
        configOutput(SAW_OUTPUT, "Saw Output");
        configOutput(TRIANGLE_OUTPUT, "Triangle Output");
        configOutput(SQUARE_OUTPUT, "SQUARE Output");
        configOutput(FINAL_OUTPUT, "Final Output");
    }
    
    float triangleWaveshaper(float x) {
        return 2.0f * std::abs(2.0f * (x - 0.25f) - 1.0f) - 1.0f;
    }

    float sineWaveshaper(float x) {
        return 2.0f * std::abs(2.0f * (x - 0.25f) - 1.0f) - 1.0f;
    }

    float sawWaveshaper(float x) {
        return 0.8f * (std::sin(4.f * M_PI * x) + std::cos(6.f * M_PI * x));
    }
    float frequencyWaveshaper(float x) {
        if (wavetable.size() > 0) {
            float index = (x + 1.0f) * 0.5f * (wavetable.size() - 1);
            int i0 = static_cast<int>(std::floor(index));
            int i1 = static_cast<int>(std::ceil(index));
            float frac = index - i0;

            float y0 = triangleWaveshaper(wavetable[i0]); // You can replace this with sineWaveshaper to use the sine waveshaper
            float y1 = triangleWaveshaper(wavetable[i1]); // Replace with sineWaveshaper

            return y0 + frac * (y1 - y0);
        } else {
            return x;
        }
    }

    float wavefolding(float x, float amount, float modulation) {
        const float threshold = amount;

        // Apply wavefolding with modulation
        float foldedValue = x + modulation * std::sin(2.0f * M_PI * x);
        
        // Clip the folded value at the specified threshold
        if (foldedValue > threshold) {
            foldedValue = threshold - (foldedValue - threshold);
        } else if (foldedValue < -threshold) {
            foldedValue = -threshold - (foldedValue + threshold);
        }

        return foldedValue;
    }

    void process(const ProcessArgs &args) override {
        try {
            // Read the PSYCHEDELIC_CV_KNOB_PARAM control value
            psychedelicCVKnobValue = params[PSYCHEDELIC_CV_KNOB_PARAM].getValue();

            // Modulate the PSYCHEDELIC_CV_INPUT_FOR_All using the knob value
            float psychedelicCVAll = inputs[PSYCHEDELIC_CV_INPUT_FOR_All].getVoltage() * params[PSYCHEDELIC_CV_KNOB_PARAM].getValue();
            psychedelicCVAll *= psychedelicCVKnobValue * psychedelicCVKnobScale;    
            // Read the FM_PARAM control value
            float fmAmount = params[FM_PARAM].getValue();
            // Read the FM_AMOUNT_PARAM control value
            float fmAmountParam = params[FM_AMOUNT_PARAM].getValue();
            // Read the FM_AMOUNT_INPUT CV value
            float fmAmountCV = inputs[FM_AMOUNT_INPUT].getVoltage() * params[FM_AMOUNT_PARAM].getValue();

            // Combine the FM_AMOUNT_PARAM and FM_AMOUNT_INPUT CV
            fmAmount += fmAmountParam * fmAmountCV;
            // Read the SINE_WAVESHAPER_PARAM control value
            float sineWaveshaperAmount = params[SINE_WAVESHAPER_PARAM].getValue();
            // Read the SINE_WAVESHAPER_CV_INPUT CV value
            float sineWaveshaperCV = inputs[PSYCHEDELIC_CV_INPUT_FOR_All].getVoltage() * params[SINE_WAVESHAPER_PARAM].getValue() * params[PSYCHEDELIC_CV_KNOB_PARAM].getValue();

            // Combine the SINE_WAVESHAPER_PARAM and PSYCHEDELIC_CV_INPUT_FOR_All CV
            sineWaveshaperAmount += sineWaveshaperCV;
            // Read the WAVEFOLD_CV_PARAM control value
            float squareWavefoldCV = params[WAVEFOLD_PARAM].getValue();
            // Read the SQUARE_WAVEFOLD_CV_INPUT CV value
            float squareWavefoldCVInput = inputs[SQUARE_WAVEFOLD_CV_INPUT].getVoltage() * params[WAVEFOLD_PARAM].getValue();

            // Combine the WAVEFOLD_CV_PARAM and SQUARE_WAVEFOLD_CV_INPUT CV
            squareWavefoldCV += squareWavefoldCVInput;
            // Triangle Oscillator
            float pitchTriangle = params[PITCH_PARAM_TRIANGLE].getValue() + inputs[PITCH_INPUT_TRIANGLE].getVoltage();
            float volumeTriangle = params[VOLUME_PARAM_TRIANGLE].getValue();
            float freqTriangle = dsp::FREQ_C4 * std::pow(2.f, pitchTriangle + fmAmount * inputs[FM_INPUT].getVoltage());

            float psychedelicCVTriangle = inputs[PSYCHEDELIC_CV_INPUT_TRIANGLE].getVoltage() + inputs[PSYCHEDELIC_CV_INPUT_FOR_All].getVoltage() * params[PSYCHEDELIC_CV_KNOB_PARAM].getValue();
            float psychedelicParamTriangle = params[PSYCHEDELIC_PARAM_TRIANGLE].getValue() + psychedelicCVTriangle;

            phaseTriangle += freqTriangle * args.sampleTime;
            if (phaseTriangle >= 1.f)
                phaseTriangle -= 1.f;

            float triangle = 2.f * (std::abs(2.f * phaseTriangle - 1.f) - 0.5f);
            triangle = (psychedelicParamTriangle == 0.0) ? triangle : (1.0 - psychedelicParamTriangle) * triangle + psychedelicParamTriangle * 0.5f * (std::sin(3.f * M_PI * triangle) + std::cos(5.f * M_PI * triangle));
            outputs[TRIANGLE_OUTPUT].setVoltage(5.f * volumeTriangle * triangle);

            // Sine Oscillator
            float pitchSine = params[PITCH_PARAM_SINE].getValue() + inputs[PITCH_INPUT_SINE].getVoltage();
            float volumeSine = params[VOLUME_PARAM_SINE].getValue();
            float freqSine = dsp::FREQ_C4 * std::pow(2.f, pitchSine + fmAmount * inputs[FM_INPUT].getVoltage());

            phaseSine += freqSine * args.sampleTime;
            if (phaseSine >= 1.f)
                phaseSine -= 1.f;

            float sine = std::sin(2.f * M_PI * phaseSine);
            // Apply the sineWaveshaper effect to the Sine oscillator using SINE_WAVESHAPER_PARAM
            sine = (sineWaveshaperAmount == 0.0) ? sine : (1.0 - sineWaveshaperAmount) * sine + sineWaveshaperAmount * 0.5f * (std::sin(3.f * M_PI * sine) + std::cos(5.f * M_PI * sine));
            outputs[SINE_OUTPUT].setVoltage(5.f * volumeSine * sine);

            // Saw Oscillator
            float pitchSaw = params[PITCH_PARAM_SAW].getValue() + inputs[PITCH_INPUT_SAW].getVoltage();
            float volumeSaw = params[VOLUME_PARAM_SAW].getValue();
            float freqSaw = dsp::FREQ_C4 * std::pow(2.f, pitchSaw + fmAmount * inputs[FM_INPUT].getVoltage());
            phaseSaw += freqSaw * args.sampleTime;
            if (phaseSaw >= 1.f)
                phaseSaw -= 1.f;
            float psychedelicCVsaw = inputs[PSYCHEDELIC_CV_INPUT_SAW].getVoltage() + inputs[PSYCHEDELIC_CV_INPUT_FOR_All].getVoltage() * params[PSYCHEDELIC_CV_KNOB_PARAM].getValue();
            float sawWaveshaperAmount = params[SAW_WAVESHAPER_PARAM].getValue() + psychedelicCVsaw;

            float sawValue = 2.f * (phaseSaw - std::floor(phaseSaw));
            // Apply the sawWaveshaper effect to the Saw oscillator using SAW_WAVESHAPER_PARAM
    
            sawValue = (sawWaveshaperAmount == 0.0) ? sawValue : (1.0 - sawWaveshaperAmount) * sawValue + sawWaveshaperAmount * 0.5f * sawWaveshaper(sawValue);
            outputs[SAW_OUTPUT].setVoltage(5.f * volumeSaw * sawValue);

            // Square Oscillator (Modified for Standard Digital Wavefolding)
            float pitchSquare = params[PITCH_PARAM_SQUARE].getValue() + inputs[PITCH_INPUT_SQUARE].getVoltage();
            float volumeSquare = params[VOLUME_PARAM_SQUARE].getValue();
            float threshold = 0.5f;

            phaseSquare += dsp::FREQ_C4 * std::pow(2.f, pitchSquare + fmAmount * inputs[FM_INPUT].getVoltage()) * args.sampleTime;
            if (phaseSquare >= 1.f)
                phaseSquare -= 1.f;

            // Simple Square Wave Generation with Digital Wavefolding
            float modulation = std::sin(2.0f * M_PI * phaseSquare); // Example: Using a sine wave for modulation
            float square = (phaseSquare < threshold) ? -1.0f : 1.0f;
            square = wavefolding(square, params[WAVEFOLD_PARAM].getValue() + squareWavefoldCV, modulation);

            outputs[SQUARE_OUTPUT].setVoltage(5.f * volumeSquare * square);


            // Sum the modified outputs for the final sound
            float finalOutput = outputs[TRIANGLE_OUTPUT].getVoltage() +
                                outputs[SINE_OUTPUT].getVoltage() +
                                outputs[SAW_OUTPUT].getVoltage() +
                                outputs[SQUARE_OUTPUT].getVoltage();
                               
            outputs[FINAL_OUTPUT].setVoltage(finalOutput);
            
            } catch (const std::exception &e) {
                // Handle the exception here
                // You can log the error, display a message, or take appropriate action
                // For now, let's print the error message to the console
                std::cerr << "Error in FmOperator::process: " << e.what() << std::endl;
            } catch (...) {
                // Catch any other unexpected exceptions
                std::cerr << "Unknown error in FmOperator::process" << std::endl;
            }
    }
};

struct FmOperatorWidget : ModuleWidget {
    FmOperatorWidget(FmOperator* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/HutaraFm.svg")));    


        // Add SvgLight for the mandala
        SvgLight* svgLight = createWidget<SvgLight>(mm2px(Vec(42, 50)));  // Adjust the position as needed

        // Load the SVG file for the light
        std::string svgPathLight = asset::plugin(pluginInstance, "res/mandala.svg");
        svgLight->setSvg(APP->window->loadSvg(svgPathLight));
        addChild(svgLight);

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(43.24, 30.233)), module, FmOperator::PSYCHEDELIC_CV_KNOB_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.24, 38.00)), module, FmOperator::PITCH_PARAM_SINE));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8, 51.063)), module, FmOperator::PITCH_PARAM_SAW));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.24, 23.063)), module, FmOperator::PITCH_PARAM_TRIANGLE));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 64.063)), module, FmOperator::PITCH_PARAM_SQUARE));
        addParam(createParamCentered<SynthTechAlco>(mm2px(Vec(28.24, 92.055)), module, FmOperator::FM_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(75.24, 40.063)), module, FmOperator::SINE_WAVESHAPER_PARAM)); // Changed parameter name
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.24, 55.063)), module, FmOperator::SAW_WAVESHAPER_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.24, 20.063)), module, FmOperator::PSYCHEDELIC_PARAM_TRIANGLE));
        addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(60.24, 85.400)), module, FmOperator::FM_AMOUNT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.24, 108.713)), module, FmOperator::VOLUME_PARAM_SINE));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.24, 108.400)), module, FmOperator::VOLUME_PARAM_SAW));
        addParam(createParamCentered<Rogan2PWhite>(mm2px(Vec(70.24, 74.063)), module, FmOperator::WAVEFOLD_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 108.400)), module, FmOperator::VOLUME_PARAM_TRIANGLE));
        
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(61, 77)), module, FmOperator::SQUARE_WAVEFOLD_CV_INPUT));  // Adjust position
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(54.24, 108.400)), module, FmOperator::VOLUME_PARAM_SQUARE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.24, 25.0)), module, FmOperator::PITCH_INPUT_TRIANGLE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.24, 51.0)), module, FmOperator::PITCH_INPUT_SAW));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.24, 40.0)), module, FmOperator::PITCH_INPUT_SINE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.24, 40.263)), module, FmOperator::PSYCHEDELIC_CV_INPUT_FOR_All));  // Add the new CV input
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.24, 62.0)), module, FmOperator::PITCH_INPUT_SQUARE));
        addInput(createInputCentered<CL1362Port>(mm2px(Vec(43.24, 96.400)), module, FmOperator::FM_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.24, 96.400)), module, FmOperator::FM_AMOUNT_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(76.5, 58.713)), module, FmOperator::PSYCHEDELIC_CV_INPUT_SAW));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(76.5, 20.713)), module, FmOperator::PSYCHEDELIC_CV_INPUT_TRIANGLE));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(28.24, 118.713)), module, FmOperator::SINE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(41.24, 118.713)), module, FmOperator::SAW_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 118.713)), module, FmOperator::TRIANGLE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.24, 118.713)), module, FmOperator::SQUARE_OUTPUT));
        // Add an Oscilloscope for the final output
        addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(68.24, 115.713)), module, FmOperator::FINAL_OUTPUT));
    }

};
Model* modelFmOperator = createModel<FmOperator, FmOperatorWidget>("FmOperator");
