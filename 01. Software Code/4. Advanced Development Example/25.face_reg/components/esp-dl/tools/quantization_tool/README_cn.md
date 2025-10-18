# Quantitative Toolkit [[English]](./README.md)

Quantization toolkit helps you quantify your model，Using Espressif Systems for Inference。The toolkit is open source AI ModelFormat [ONNX](https://github.com/onnx/onnx) run。

The toolkit includes three separate tools:

- [optimizer](#optimizer)，Used to optimize computational graphs
- [calibrator](#calibrator)，for post-training quantization，No retraining required
- [Evaluate器](#Evaluate器)，Used to evaluate the performance of the quantized model

This document describes the specifications for each tool。API Please see the introduction[Quantitative Toolkit API](quantization_tool_api_cn.md)。

> Please make sure before using the tool kit，您已WillModelConvert to ONNX Format，Please refer to relevant information[resource](#resource)。

## optimizer

计算图optimizer [optimizer.py](optimizer.py) You can remove redundant nodes by、Simplify model structure、Model融合等方式提高Model性能。该optimizer基于 [ONNX optimizer](https://github.com/onnx/optimizer)of[Optimize delivery (pass)](https://github.com/onnx/optimizer/tree/master/onnxoptimizer/passes)，and our additional delivery。

在Quantify前开启计算图融合很重要，Especially for batch normalization (batch normalization) fusion。Therefore we recommend that before using the calibrator and evaluator，先用optimizer优化Model。You can use [Netron](https://github.com/lutzroeder/netron) 查看Model结构。

**Python API Example**

```cpp
// load your ONNX model from given path
model_proto = onnx.load('mnsit.onnx')

// fuse batch normalization layers and convolution layers, and fuse biases and convolution layers
model_proto = onnxoptimizer.optimize(model_proto, ['fuse_bn_into_conv', 'fuse_add_bias_into_conv'])

// set input batch size as dynamic
optimized_model = convert_model_batch_to_dynamic(model_proto)

// save optimized model to given path
optimized_model_path = 'mnist_optimized.onnx'
onnx.save(new_model, optimized_model_path)
```

## calibrator

calibrator可Quantify浮点Model，Make it compliant for inference on Espressif chips。About the quantization forms supported by the calibrator，Please check[Quantify规范](../../docs/zh_CN/quantization_specification.md)。

要Will一个 32 bit floating point (FP32) ModelConvert to一个 8 bit integer (int8) or 16 bit integer (int16) Model，工作流程like下：
- Prepare FP32 model
- Prepare calibration data set
- configuration quantification
- 获取Quantization parameters

### FP32 Model

The prepared FP32 model must be adapted to the ESP-DL library. If the model contains operations that are not supported by the library, the calibrator will not accept them and generate an error message.

Modelof适配性可在获取Quantization parameters时examine，It can also be called in advance by calling *check_model* examine。

The input to the model should be normalized data。If your normalization process is included in the computational graph of your model，To ensure quantitative performance，请删除图middleof相关节点并提前做好归一化。

### calibration data set

Choosing an appropriate calibration data set is important for quantification。A good calibration data set should be representative。You can try different calibration data sets，Compare model performance after quantization using different parameters。

## ## Quantitative configuration

Calibrator support int8 and int16 Quantify。int8 and int16 各自of配置like下：

int8：
- granularity：'per-tensor'、'per-channel'
- calibration_method：'entropy'、'minmax'

int16：
- granularity：'per-tensor'
- calibration_method：'minmax'

### Quantization parameters

like[Quantify规范](../../docs/zh_CN/quantization_specification.md)described，ESP-DL middle 8 bit or 16 Bit quantization uses the following formula to approximate floating point values：

```math
real\_value = int\_value * 2^{\ exponent}
```

where 2^exponent is the scale.

The returned quantification table lists the quantification scales of all data in the model.，These data include：
  - constant：weight、Bias and activation function；
  - variable：Tensor，likemiddle间层（activation function）input and output。

**Python API Example**

```cpp
// load your ONNX model from given path
model_proto = onnx.load(optimized_model_path)

// initialize an calibrator to quantize the optimized MNIST model to an int8 model per channel using entropy method
calib = Calibrator('int8', 'per-channel', 'entropy')

// set ONNX Runtime execution provider to CPU
calib.set_providers(['CPUExecutionProvider'])

// use calib_dataset as the calibration dataset, and save quantization parameters to the pickle file
pickle_file_path = 'mnist_calib.pickle'
calib.generate_quantization_table(model_proto, calib_dataset, pickle_file_path)
```

## # evaluator

Evaluator for simulating quantitative solutions for Espressif chips，帮助EvaluateQuantify后Modelof性能。

If there are unsupported operations in the model, the calibrator will not accept them and generate an error message.

like果Quantify后Modelof性能无法满足需求，Consider quantified perception training。

**Python API Example**

```cpp
// initialize an evaluator to generate an MNIST using int8 per-channel quantization model running on ESP32-S3 SoC
eva = Evaluator('int8', 'per-channel', 'esp32s3')

// use quantization parameters in the pickle file to generate the int8 model
eva.generate_quantized_model(model_proto, pickle_file_path)

// return results in floating-point values
outputs = eva.evaluate_quantized_model(test_images, to_float = True)
res = np.argmax(outputs[0])
```

## # Example

For a complete code example for quantifying and evaluating MNIST models, please refer to [example.py](examples/example.py).

Will TensorFlow MNIST ModelConvert to ONNX Code examples for models，Please refer to [mnist_tf.py](examples/tensorflow_to_onnx/mnist_tf.py)。

Will MXNet MNIST ModelConvert to ONNX Code examples for models，Please refer to [mnist_mxnet.py](examples/mxnet_to_onnx/mnist_mxnet.py)。

For code examples that convert PyTorch MNIST models to ONNX models, please refer to [mnist_pytorch.py](examples/pytorch_to_onnx/mnist_pytorch.py).

## # resource

The following tools can help you convert your model to ONNX Format。

- TensorFlow、Keras and Tflite Convert to ONNX：[tf2onnx](https://github.com/onnx/tensorflow-onnx) 
- MXNet Convert to ONNX：[MXNet-ONNX](https://mxnet.apache.org/versions/1.8.0/api/python/docs/tutorials/deploy/export/onnx.html) 
- PyTorch Convert to ONNX：[torch.onnx](https://pytorch.org/docs/stable/onnx.html)

Environmental requirements:
- Python == 3.7
- [Numba](https://github.com/numba/numba) == 0.53.1
- [ONNX](https://github.com/onnx/onnx) == 1.9.0
- [ONNX Runtime](https://github.com/microsoft/onnxruntime) == 1.7.0
- [ONNX Optimizer](https://github.com/onnx/optimizer) == 0.2.6

You can use requirement.txt to install related Python Dependency package：
```cpp
pip install -r requirement.txt
```