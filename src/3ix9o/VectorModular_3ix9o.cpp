#include "plugin.hpp"
#include <cmath> //for logf()

// Utility function to map voltage to a brightness value (0.0 to 1.0 range)
float mapVoltageToBrightness(float voltage, float minVoltage, float maxVoltage) {
    // Normalize voltage to range [0, 1]
    float normalized = (voltage - minVoltage) / (maxVoltage - minVoltage);
    
    // Apply logarithmic scaling
    float brightness = log10(1 + normalized * 9) / log10(10);
    
    return brightness;
}


struct VectorModular_3ix9o : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		INPUT1_INPUT,
		INPUT2_INPUT,
		INPUT3_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT1_OUTPUT,
		OUTPUT2_OUTPUT,
		OUTPUT3_OUTPUT,
		OUTPUT4_OUTPUT,
		OUTPUT5_OUTPUT,
		OUTPUT6_OUTPUT,
		OUTPUT7_OUTPUT,
		OUTPUT8_OUTPUT,
		OUTPUT9_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightIds {
		  ENUMS(LED1_RGB, 3), // Reserves 3 IDs for the first RGB light (Red, Green, Blue)
    	  ENUMS(LED2_RGB, 3), // 3 IDs for the second RGB light
    	  ENUMS(LED3_RGB, 3), // 3 IDs for the third RGB light
    	  NUM_LIGHTS
	};

	VectorModular_3ix9o() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, NUM_LIGHTS);
		configInput(INPUT1_INPUT, "");
		configInput(INPUT2_INPUT, "");
		configInput(INPUT3_INPUT, "");
		configOutput(OUTPUT1_OUTPUT, "");
		configOutput(OUTPUT2_OUTPUT, "");
		configOutput(OUTPUT3_OUTPUT, "");
		configOutput(OUTPUT4_OUTPUT, "");
		configOutput(OUTPUT5_OUTPUT, "");
		configOutput(OUTPUT6_OUTPUT, "");
		configOutput(OUTPUT7_OUTPUT, "");
		configOutput(OUTPUT8_OUTPUT, "");
		configOutput(OUTPUT9_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		  // Get input signals
        float input1 = inputs[INPUT1_INPUT].getVoltage();
        float input2 = inputs[INPUT2_INPUT].isConnected() ? inputs[INPUT2_INPUT].getVoltage() : outputs[OUTPUT3_OUTPUT].getVoltage();
        float input3 = inputs[INPUT3_INPUT].isConnected() ? inputs[INPUT3_INPUT].getVoltage() : outputs[OUTPUT6_OUTPUT].getVoltage();
		
        
        // Set outputs
        outputs[OUTPUT1_OUTPUT].setVoltage(input1);
        outputs[OUTPUT2_OUTPUT].setVoltage(input1);
        outputs[OUTPUT3_OUTPUT].setVoltage(input1);
        outputs[OUTPUT4_OUTPUT].setVoltage(input2);
        outputs[OUTPUT5_OUTPUT].setVoltage(input2);
        outputs[OUTPUT6_OUTPUT].setVoltage(input2);
        outputs[OUTPUT7_OUTPUT].setVoltage(input3);
        outputs[OUTPUT8_OUTPUT].setVoltage(input3);
        outputs[OUTPUT9_OUTPUT].setVoltage(input3);
        
// Linear scaling function for Red LED
auto mapToRed = [](float voltage) {
    if (voltage < -5.0f) {
        return 1.0f; // Full brightness for voltages < -5V
    } else if (voltage >= -5.0f && voltage < 0.0f) {
        // Interpolate linearly between 1 (full brightness) and 0 (off) for -5V <= voltage < 0V
        return (0.0f - voltage) / 5.0f;
    } else {
        return 0.0f; // Off for voltages >= 0V
    }
};

// Linear scaling function for Blue LED
auto mapToBlue = [](float voltage) {
    if (voltage > 5.0f) {
        return 1.0f; // Full brightness for voltages > 5V
    } else if (voltage <= 5.0f && voltage > 0.0f) {
        // Interpolate linearly between 0 (off) and 1 (full brightness) for 0V < voltage <= 5V
        return (voltage - 0.0f) / 5.0f;
    } else {
        return 0.0f; // Off for voltages <= 0V
    }
};

// If frequency is above threshold, set LEDs to purple
    if (frequency > frequencyThreshold) {
        lights[LED1_RGB + 0].setBrightness(1.0f); // Red
        lights[LED1_RGB + 1].setBrightness(0.0f); // Green
        lights[LED1_RGB + 2].setBrightness(1.0f); // Blue

        lights[LED2_RGB + 0].setBrightness(1.0f); // Red
        lights[LED2_RGB + 1].setBrightness(0.0f); // Green
        lights[LED2_RGB + 2].setBrightness(1.0f); // Blue

        lights[LED3_RGB + 0].setBrightness(1.0f); // Red
        lights[LED3_RGB + 1].setBrightness(0.0f); // Green
        lights[LED3_RGB + 2].setBrightness(1.0f); // Blue
    } else {
    // Compute brightness values for each LED
    float led1Red = mapToRed(input1);
    float led1Blue = mapToBlue(input1);
    float led2Red = mapToRed(input2);
    float led2Blue = mapToBlue(input2);
    float led3Red = mapToRed(input3);
    float led3Blue = mapToBlue(input3);

	// Set LED colors (Red, Green, Blue)
    lights[LED1_RGB + 0].setBrightness(led1Red);   // Red
    lights[LED1_RGB + 1].setBrightness(0.0f);      // Green (set to zero)
    lights[LED1_RGB + 2].setBrightness(led1Blue);  // Blue

    lights[LED2_RGB + 0].setBrightness(led2Red);   // Red
    lights[LED2_RGB + 1].setBrightness(0.0f);      // Green
    lights[LED2_RGB + 2].setBrightness(led2Blue);  // Blue

    lights[LED3_RGB + 0].setBrightness(led3Red);   // Red
    lights[LED3_RGB + 1].setBrightness(0.0f);      // Green
    lights[LED3_RGB + 2].setBrightness(led3Blue);  // Blue
	}
	}
};


struct VectorModular_3ix9oWidget : ModuleWidget {
	VectorModular_3ix9oWidget(VectorModular_3ix9o* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/VectorModular_3ix9o.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.663, 16.907)), module, VectorModular_3ix9o::INPUT1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.663, 54.114)), module, VectorModular_3ix9o::INPUT2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.663, 91.322)), module, VectorModular_3ix9o::INPUT3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.687, 26.707)), module, VectorModular_3ix9o::OUTPUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.796, 32.014)), module, VectorModular_3ix9o::OUTPUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.708, 37.231)), module, VectorModular_3ix9o::OUTPUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.687, 63.915)), module, VectorModular_3ix9o::OUTPUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.796, 69.222)), module, VectorModular_3ix9o::OUTPUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.708, 74.439)), module, VectorModular_3ix9o::OUTPUT6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.687, 101.123)), module, VectorModular_3ix9o::OUTPUT7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.796, 106.429)), module, VectorModular_3ix9o::OUTPUT8_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.708, 111.647)), module, VectorModular_3ix9o::OUTPUT9_OUTPUT));

		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(14.687, 16.907)), module, VectorModular_3ix9o::LED1_RGB));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(14.687, 54.114)), module, VectorModular_3ix9o::LED2_RGB));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(14.687, 91.322)), module, VectorModular_3ix9o::LED3_RGB));
	}
};


Model* modelVectorModular_3ix9o = createModel<VectorModular_3ix9o, VectorModular_3ix9oWidget>("VectorModular_3ix9o");