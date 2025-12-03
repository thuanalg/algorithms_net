#include <stdio.h>
#include <stdlib.h>
#include<string>
#include<iostream>
#include <vector>

#include <onnxruntime/core/session/onnxruntime_c_api.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
using namespace std;
#if 0
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
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//  resize nearest neighbor - a simple function
std::vector<unsigned char>
ResizeImage(const unsigned char *src, int src_w, int src_h, int dst_w,
    int dst_h, int channels)
{
	std::vector<unsigned char> dst(dst_w * dst_h * channels);
	for (int y = 0; y < dst_h; ++y) {
		int sy = y * src_h / dst_h;
		for (int x = 0; x < dst_w; ++x) {
			int sx = x * src_w / dst_w;
			for (int c = 0; c < channels; ++c) {
				dst[(y * dst_w + x) * channels + c] =
				    src[(sy * src_w + sx) * channels + c];
			}
		}
	}
	return dst;
}


int
main()
{
	int width, height, channels;
	stbi_uc *binary_img = 0;
	int size_bin = 0;
	//unsigned char *img =
	//    stbi_load("face.png", &width, &height, &channels, 3); // RGB
	//if (!img) {
	//	std::cerr << "Failed to load image" << std::endl;
	//	return -1;
	//}
	unsigned char *img =
	    stbi_load_from_memory(binary_img, // pointer PNG binary
			size_bin, // size of  PNG binary
			&width, &height, &channels,
			3 //  3 channels (RGB)
	    );

	Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "face-detect");
	Ort::SessionOptions session_options;
	session_options.SetGraphOptimizationLevel(
	    GraphOptimizationLevel::ORT_ENABLE_BASIC);

	Ort::Session session(env, 
		L"D:/x/algorithms_net/xffmpeg/python-3.11.8/faceA_model.onnx", 
		session_options);

	Ort::AllocatorWithDefaultOptions allocator;

	// Giả sử model input: [1,3,224,224]
	std::vector<int64_t> input_shape{1, 3, 224, 224};
	std::vector<float> input_tensor_values(
	    1 * 3 * 224 * 224, 0.0f); // Thay bằng data ảnh thật

	const char *input_name =
	    session.GetInputNameAllocated(0, allocator).get();

	Ort::MemoryInfo memory_info =
	    Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

	Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info,
	    input_tensor_values.data(), input_tensor_values.size(),
	    input_shape.data(), input_shape.size());

	// Output
	const char *output_name =
	    session.GetOutputNameAllocated(0, allocator).get();

	auto output_tensors = session.Run(Ort::RunOptions{nullptr}, &input_name,
	    &input_tensor, 1, &output_name, 1);

	// Lấy kết quả đầu ra
	float *output_data =
	    output_tensors.front().GetTensorMutableData<float>();

	std::cout << "Inference done. Output[0]: " << output_data[0]
		  << std::endl;

	return 0;
}
