#include "plugin.hpp"
#include <random>

// Define the module class
struct Hutara_Random_CV : Module {
    static constexpr float DEFAULT_FREQ = 1.0f;
    static constexpr float DEFAULT_GATE_LENGTH = 0.5f;
    static constexpr float CLOCK_ACTIVE_THRESHOLD = 0.1f;

    float phase = 0.f;
    float freq = DEFAULT_FREQ;
    float outputVoltage = 0.0f;
    bool sampleNext = false;
    bool bipolarOutput = true;
    std::mt19937 randomGenerator;

    enum ParamIds {
        OUTPUT_VOLTAGE_PARAM,
        BIPOLAR_PARAM,
        STRENGTH_PARAM,
        RANDOM_GATE_SPEED_PARAM,
        GATE_LENGTH_PARAM,
        TRIGGER_DIVISION_DIVIDER_PARAM,
        S_H_OFFSET_PARAM,  // Changed from S&H_OFFSET_PARAM to S_H_OFFSET_PARAM
        NUM_PARAMS
    };

    enum InputIds {
        ON_INPUT, // Combined input for clock and trigger
        NUM_INPUTS
    };

    enum OutputIds {
        GATE_OUTPUT,
        OUTPUT,
        GATE_OUTPUT_INVERTED,
        NUM_OUTPUTS
    };

    // Constructor
    Hutara_Random_CV() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
        configParam(OUTPUT_VOLTAGE_PARAM, 0.25f, 10.f, 5.f, "S&H Voltage");
        configParam(BIPOLAR_PARAM, 0.f, 1.f, 1.f, "1 Bipolar/0 Unipolar");
        configParam(RANDOM_GATE_SPEED_PARAM, 1, 3, 2, "Random Gate Speed");
        configParam(GATE_LENGTH_PARAM, 0.01f, 1.0f, DEFAULT_GATE_LENGTH, "Gate Length");
        configParam(STRENGTH_PARAM, 1.f, 10.f, 5.f, "Modulation Strength");
        configParam(TRIGGER_DIVISION_DIVIDER_PARAM, 1, 3, 1, "Trigger Division");
        configParam(S_H_OFFSET_PARAM, -5.f, 5.f, 0.f, "S&H Offset"); // Configure S&H Offset
        configOutput(GATE_OUTPUT, "Gate");
        configOutput(OUTPUT, "S&H");
        configOutput(GATE_OUTPUT_INVERTED, "Inverted Gate");
        configInput(ON_INPUT, "On Input");

        // Seed the random number generator
        std::random_device rd;
        randomGenerator.seed(rd());
    }

    // Process function to handle the module's behavior
    void process(const ProcessArgs& args) override {
        outputVoltage = params[OUTPUT_VOLTAGE_PARAM].getValue();
        bipolarOutput = params[BIPOLAR_PARAM].getValue() > 0.5f;
        float strength = params[STRENGTH_PARAM].getValue();
        float gateLength = params[GATE_LENGTH_PARAM].getValue();
        int gateSpeedParam = round(params[RANDOM_GATE_SPEED_PARAM].getValue());
        int triggerDivisionDivider = round(params[TRIGGER_DIVISION_DIVIDER_PARAM].getValue());
        float offset = params[S_H_OFFSET_PARAM].getValue(); // Retrieve the S&H offset

        float gateSpeed = 1.0f;

        if (inputs[ON_INPUT].isConnected()) {
            float onInputVoltage = inputs[ON_INPUT].getVoltage();
            if (std::abs(onInputVoltage) > CLOCK_ACTIVE_THRESHOLD) {
                gateSpeed = clamp(1.0f - std::abs(clamp(1.0f / onInputVoltage, 0.0f, 10.0f)), 0.1f, 1.0f);
            }

            // Apply trigger division here
            gateSpeed /= triggerDivisionDivider;
            gateSpeed = clamp(gateSpeed, 0.1f, 1.0f);

            switch (gateSpeedParam) {
                case 1:
                    gateSpeed *= onInputVoltage / 4.0f;
                    break;
                case 2:
                    gateSpeed *= onInputVoltage / 2.0f;
                    break;
                case 3:
                    gateSpeed = onInputVoltage;
                    break;
            }
        }

        if (inputs[ON_INPUT].getVoltage() >= 1.0f && !sampleNext) {
            float randomValue = generateRandomFloat() * strength;
            if (bipolarOutput) {
                randomValue *= outputVoltage;
            } else {
                randomValue = (randomValue + 1.0f) * 0.5f * outputVoltage;
            }
            randomValue = clamp(randomValue, bipolarOutput ? -outputVoltage : 0.0f, outputVoltage);
            randomValue += offset; // Apply the offset to the sampled value
            sampleNext = true;
            outputs[OUTPUT].setVoltage(randomValue);

            float randomGate = (generateRandomFloat() < gateLength) ? 10.0f : 0.0f;
            switch (gateSpeedParam) {
                case 1:
                    randomGate = (generateRandomFloat() < gateLength * 0.25f) ? 10.0f : 0.0f;
                    break;
                case 2:
                    randomGate = (generateRandomFloat() < gateLength * 0.125f) ? 10.0f : 0.0f;
                    break;
                case 3:
                    randomGate = (generateRandomFloat() < gateLength * 0.625f) ? 10.0f : 0.0f;
                    break;
            }

            outputs[GATE_OUTPUT].setVoltage(randomGate);
            outputs[GATE_OUTPUT_INVERTED].setVoltage(10.0f - randomGate);
        }

        if (inputs[ON_INPUT].getVoltage() < 1.0f && sampleNext) {
            sampleNext = false;
        }
    }

private:
    // Clamp a value between a minimum and maximum
    float clamp(float value, float min, float max) {
        return std::min(std::max(value, min), max);
    }

    // Generate a random float between -1.0 and 1.0
    float generateRandomFloat() {
        std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
        return distribution(randomGenerator);
    }
};

// Define the module widget class
struct Hutara_Random_CVWidget : ModuleWidget {
    Hutara_Random_CVWidget(Hutara_Random_CV* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Hutaraa.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParam<CKSSThree>(Vec(20, 330), module, Hutara_Random_CV::RANDOM_GATE_SPEED_PARAM));
        addParam(createParam<BefacoTinyKnob>(Vec(47, 215), module, Hutara_Random_CV::OUTPUT_VOLTAGE_PARAM));
        addParam(createParam<BefacoSwitch>(Vec(10, 175), module, Hutara_Random_CV::BIPOLAR_PARAM));
        addParam(createParam<RoundBlackSnapKnob>(Vec(45, 70), module, Hutara_Random_CV::STRENGTH_PARAM));

        addOutput(createOutput<PJ3410Port>(Vec(85, 290), module, Hutara_Random_CV::GATE_OUTPUT));
        addParam(createParam<RoundSmallBlackKnob>(Vec(47, 280), module, Hutara_Random_CV::GATE_LENGTH_PARAM));
        addParam(createParam<RoundBlackSnapKnob>(Vec(47, 320), module, Hutara_Random_CV::TRIGGER_DIVISION_DIVIDER_PARAM));
        
        // Add the S&H Offset parameter to the widget
        addParam(createParam<LEDSliderBlue>(Vec(84, 150), module, Hutara_Random_CV::S_H_OFFSET_PARAM));

        addInput(createInput<PJ301MPort>(Vec(10, 120), module, Hutara_Random_CV::ON_INPUT));
        addOutput(createOutput<PJ3410Port>(Vec(46, 175), module, Hutara_Random_CV::OUTPUT));
        addOutput(createOutput<PJ3410Port>(Vec(85, 330), module, Hutara_Random_CV::GATE_OUTPUT_INVERTED));
    }
};

// Define the model
Model* modelHutara_Random_CV = createModel<Hutara_Random_CV, Hutara_Random_CVWidget>("Hutara_Random_CV");

