# Quantization specification [[English]](../en/quantization_specification.md)

[Post-training quantization](https://www.tensorflow.org/lite/performance/post_training_quantization)Convert floating point model to fixed point model。This conversion technique can reduce model size，reduce CPU and hardware accelerator latency，without compromising accuracy。

like ESP32-S3 chip，Storage space is relatively limited，exist 240 MHz of情况下每秒乘加累计运算次数 (MACs) Only up to 75 billion times。exist这样ofchip上必须要用Quantify后of模型做推理。You can use我们提供of[Quantitative Toolkit](../../tools/quantization_tool/README_cn.md)Quantized floating point model，or based on [convert.py Instructions for use](../../tools/convert_tool/README_cn.md)Steps in Deploying a Fixed-Point Model。

## full integer quantification

All data in the model needs to be quantified as 8 bit or 16 bit integer。All data includes
  - constant：weight、Bias and activation function
  - Variables: tensors, such as input and output of the intermediate layer (activation function)

8 bit or 16 Bit quantization uses the following formula to approximate floating point values：

```math
real\_value = int\_value * 2^{\ exponent}
```

### signed integer

8 bit quantized `int_value` represents a **int8** signed number, Its scope is [-128, 127]。16 bit quantized `int_value` represents a **int16** signed number, Its scope is [-32768, 32767]。

### symmetry

All quantified data are**symmetry**of，That is to say, there is no zero point（deviation），This saves the computational time of multiplying zero points with other values.。

## ## Granularity

**by tensor（aka by layer）Quantify**means that each complete tensor has only one exponent bit，该Tensor内of所有值都按照该指数位Quantify。

**Quantize by channel**means that each channel of the convolution kernel corresponds to a different index bit。

与by tensorQuantify相比，Quantize by channel通常对于部分模型来说精确度会更高，But it will also take more time。You can use[Quantitative Toolkit](../../tools/quantization_tool/README_cn.md)中of*Evaluator*模拟existchip上进行Quantify推理of性能，Then decide which quantization form to use。

16 Bit quantization ensures faster operation speed，目前仅支持by tensorQuantify。8 位Quantify支持by tensor和按通道两种形式，可让您exist性能和速度之间折中。

## # Quantization operator specification

The following is API quantitative requirements：
```
Add2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

AvgPool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

Concat
  Input ...:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Inputs and output must have the same exponent

Conv2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Weight):
    data_type  : int8 / int16
    range      : [-127, 127] / [-32767, 32767]
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
  Input 2 (Bias):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
    restriction: exponent = output_exponent
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

DepthwiseConv2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Weight):
    data_type  : int8 / int16
    range      : [-127, 127] / [-32767, 32767]
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
  Input 2 (Bias):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
    restriction: exponent = output_exponent
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

ExpandDims
 Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Flatten
 Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

FullyConnected
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Weight):
    data_type  : int8 / int16
    range      : [-127, 127] / [-32767, 32767]
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
  Input 2 (Bias):
    data_type  : int8 / int16
    range      : {[-32768, 32767] for int8 per-channel / [-128, 127] for int8 per-tensor} / {[-32768, 32767] for int16}
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
    restriction: {exponent = input_exponent + weight_exponent + 4 for per-channel / exponent = output_exponent for per-tensor}
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

GlobalAveragePool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

GlobalMaxPool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

LeakyReLU
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Alpha):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Max2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
MaxPool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Min2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
Mul2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

PReLU
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Alpha):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
ReLU
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Reshape
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Squeeze
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
Sub2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

Transpose
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
```