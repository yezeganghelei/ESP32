# Quantitative Toolkit API [[English]](./quantization_tool_api.md)

## # Calibrator class

### initialization

```
Calibrator(quantization_bit, granularity='per-tensor', calib_method='minmax')
```

**actual parameters**
- **quantization_bit** _(string)_：
  - 'int8' represents a full 8-bit integer quantification.
  - 'int16' Represent all 16 Bit integer quantification。
- **granularity** _(string)_：
  - If granularity = 'per-tensor' (default), the entire tensor has only one index.
  - like granularity = 'per-channel'，则卷积层的每个通道都有one指数。
- **calib_method** _(string)_：   
  - like calib_method = 'minmax'（default），Then the threshold value comes from校准数据集每层输出的最小值and最大值。
  - like calib_method = 'entropy'，Then the threshold value comes from KL divergence。

## ## *check_model* method

```
Calibrator.check_model(model_proto)
```
Check model suitability。

**actual parameters**
- **model_proto** _(ModelProto)_：one FP32 ONNX Model。

**return value**
- **-1**: The model is not suitable.

## ## *set_method* method
```
Calibrator.set_method(granularity, calib_method)
```
configuration quantification。

**actual parameters**
- **granularity** _(string)_：
  - like granularity = 'per-tensor'，则整个张量只有one指数。
  - like granularity = 'per-channel'，则卷积层的每个通道都有one指数。
- **calib_method** _(string)_：   
  - like calib_method = 'minmax'，Then the threshold value comes from校准数据集每层输出的最小值and最大值。
  - like calib_method = 'entropy'，Then the threshold value comes from KL divergence。

## ## *set_providers* method
```
Calibrator.set_providers(providers)
```
Configure the runtime environment provider for ONNX Runtime.

**actual parameters**
- **providers** _(list of strings)_：[list](https://onnxruntime.ai/docs/reference/execution-providers/)Runtime environment provider in，like 'CPUExecutionProvider'、'CUDAExecutionProvider'。

## ## *generate_quantization_table* method
```
Calibrator.generate_quantization_table(model_proto, calib_dataset, pickle_file_path)
```
Generate quantification table.

**actual parameters**
- **model_proto** _(ModelProto)_：one FP32 ONNX Model。
- **calib_dataset** _(ndarray)_: Calibration dataset used to calculate thresholds. The larger the data set, the longer it takes to generate the quantification table.
- **pickle_file_path** _(string)_：Store quantization parameters pickle file path。

### *export_coefficient_to_cpp* method
```
Calibrator.export_coefficient_to_cpp(model_proto, pickle_file_path, target_chip, output_path, file_name, print_model_info=False)
```
Export quantization parameters。

**Arguments**
- **model_proto** _(ModelProto)_: one FP32 ONNX Model。
- **pickle_file_path** _(string)_: Store quantization parameters pickle file path。
- **target_chip** _(string)_: Currently supported 'esp32'、'esp32s2'、'esp32c3' 、'esp32s3'。 
- **output_path** _(string)_: Path to store output files。
- **file_name** _(string)_: The name of the output file。
- **print_model_info**_(bool)_: 
  - False (default): Don't print any information。
  - True: Print model-related information.

## Evaluator class

### initialization

```
Evaluator(quantization_bit, granularity, target_chip)
```
**actual parameters**
- **quantization_bit** _(string)_：
  - 'int8' represents a full 8-bit integer quantification.
  - 'int16' Represent all 16 Bit integer quantification。
- **granularity** _(string)_：
  - like granularity = 'per-tensor'，则整个张量只有one指数。
  - like granularity = 'per-channel'，则卷积层的每个通道都有one指数。
- **target_chip** _(string)_：default是 'esp32s3'。

## ## *check_model* method
```
Evaluator.check_model(model_proto)
```
Check model suitability。

**actual parameters**
- **model_proto** _(ModelProto)_：one FP32 ONNX Model。

**Return**
- **-1**: The model is not suitable.

## ## *set_target_chip* method
```
Evaluator.set_target_chip(target_chip)
```
Configure the analog chip environment。

**actual parameters**
- **target_chip** _(string)_：Currently only supports 'esp32s3'。

## ## *set_providers* method
```
Evaluator.set_providers(providers)
```

Configure the runtime environment provider for ONNX Runtime.

**actual parameters**
- **providers** _(list of strings)_：[list](https://onnxruntime.ai/docs/reference/execution-providers/)Runtime environment provider in，like 'CPUExecutionProvider'、'CUDAExecutionProvider'。

### *generate_quantized_model* method
```
Evaluator.generate_quantized_model(model_proto, pickle_file_path)
```
生成量化后的Model。

**actual parameters**
- **model_proto** _(ModelProto)_：one FP32 ONNX Model。
- **pickle_file_path** _(string)_：storage FP32 ONXX All quantitative parameters of the model pickle file path。Should pickle The file must contain the quantified parameters of all input and output nodes of the model calculation graph.。

### *evaluate_quantized_model* method
```
Evaluator.evaluate_quantized_model(batch_fp_input, to_float=False)
```
Get the output of a quantized model。

**actual parameters**
- **batch_fp_input** _(ndarray)_: Batch floating point input.
- **to_float** _(bool)_： 
  - False (default): Return output directly.
  - True：Output converted to floating point value。

**return value**

outputs and output_names tuple：
- **outputs** _(list of ndarray)_：Quantify the model’s output。
- **output_names** _(list of strings)_: Output names.