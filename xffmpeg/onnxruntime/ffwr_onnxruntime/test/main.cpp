#include <stdio.h>
#include <stdlib.h>
#include<string>
#include<iostream>
#include <onnxruntime/core/session/onnxruntime_c_api.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
using namespace std;

int
main(int argc, char* argv[])
{
	int a = 0;
	try {
		// 1. Create env
		Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "minimal-load");

		// 2. Tạo session options
		Ort::SessionOptions session_options;
		session_options.SetIntraOpNumThreads(1);
		session_options.SetGraphOptimizationLevel(
		    GraphOptimizationLevel::ORT_ENABLE_BASIC);

		// 3. Load ONNX model
		const wchar_t *model_path = L"D:/x/algorithms_net/xffmpeg/python-3.11.8/faceA_model.onnx";
		//std::string path = "face_model.onnx";
		Ort::Session session(env, model_path, session_options);

		std::cout << "Model loaded successfully: " << model_path
			  << std::endl;

		// 4. print info input/output
		Ort::AllocatorWithDefaultOptions allocator;

		// Input
		size_t num_inputs = session.GetInputCount();
		std::cout << "Number of inputs: " << num_inputs << std::endl;
		for (size_t i = 0; i < num_inputs; i++) {
		#if 0
			char *name = session.GetInputName(i, allocator);
			std::cout << "Input[" << i << "] name: " << name
				  << std::endl;
		#endif
		}

		// Output
		size_t num_outputs = session.GetOutputCount();
		std::cout << "Number of outputs: " << num_outputs << std::endl;
		for (size_t i = 0; i < num_outputs; i++) {
#if 0
			char *name = session.GetOutputName(i, allocator);
			std::cout << "Output[" << i << "] name: " << name
				  << std::endl;
#endif
		}

	} catch (const Ort::Exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
