# config.json Configuration specifications [[English]](./specification_of_config_json.md)

> config.json for saving coefficient.npy Quantization configuration of floating point numbers in file。

## Configuration

config.json Each item in represents the configuration of a layer。Take the following code as an example：

```json
{
    "l1": {"/* the configuration of layer l1 */"},
    "l2": {"/* the configuration of layer l2 */"},
    "l3": {"/* the configuration of layer l3 */"},
    ...
}
```

key for each item (key) yes**layer name**。conversion tools ``convert.py`` Search for the corresponding layer name .npy document。for example，layer name “l1”，The conversion tool will be "l1_filter.npy" document中搜索 l1 of过滤器系数。**config.json 中oflayer name需and .npy document名中oflayer name保持一致。**

每项ofvalueyes**Layer configuration**。Please fill in the form 1 The layer configuration arguments listed in：

<div align=center>Table 1: Layer configuration parameters</div>

| key | type | value |
|---|:---:|---|
| "operation" | string | - "conv2d"<br>- "depthwise_conv2d"<br>- "fully_connected" |
| "feature_type" | string | - "s16" represents 16-bit integer quantification, element_width is 16<br> - "s8" represents 8-bit integer quantization, element_width is 8 |
| "filter_exponent" | integer | - If filled in，Then the filter is quantized according to the formula：value_float = value_int * 2^index<sup>[1](#note1)</sup> <br>- If vacant<sup>[2](#note2)</sup>，则indexfor log2(max(abs(value_float)) / 2^(element_width - 1))，The filter is quantized according to the formula：value_float = value_int * 2^index|
| "bias" | string | - "True" Represents added deviation<br>- "False" andVacant代surface不use偏差 |
| "output_exponent" | integer | The output and deviation are quantified according to the formula：value_float = value_int * 2^index。<br>at present，"output_exponent" Only valid when converting deviation coefficients。当useQuantify by layer时, required"output_exponent"。If a specific layer has no bias or when using per-channel quantization，"output_exponent" 可Vacant。 |
| "input_exponent" | integer | 当useQuantize by channel时, 偏差ofindex位与输入and过滤器ofindex位相关。<br>Must be provided if there is any deviation "input_exponent" used to convert deviation coefficients。如果特定层没有偏差或useQuantify by layer时，"input_exponent" 可Vacant。|
| "activation" | dict | - If filled in，详见surface 2<br>- If vacant，Then no activation function is used |

<div align=center>Table 2: Activation function configuration parameters</div>

| key | type | value |
|---|:---:|---|
| "type" | string | - "ReLU"<br>- "LeakyReLU"<br>- "PReLU" |
| "exponent" | integer | - If filled in，Then the activation function is quantized according to the formula： value_float = value_int  * 2^index<br>- If vacant，则indexfor log2(max(abs(value_float)) / 2^(element_width - 1)) |

> <a name="note1">1</a>: **index**：The number of base multiplications during quantization。to better understand，Please read[Quantitative specifications](./quantization_specification.md)。
>
> <a name="note2">2</a>: **Vacant**：Do not fill in specific actual parameters。

## # Example

Suppose there is a one-layer model：

##### 1. use int16 Quantify by layer:
- Layer name: "mylayer"
- operation：Conv2D(input, filter) + bias
- output_exponent：-10
- feature_type: s16, 16-bit integer quantification
- Activation function type: PReLU 

config.json should read:

```json
{
	"mylayer": {
		"operation": "conv2d",
		"feature_type": "s16",
        "bias": "True",
        "output_exponent": -10,
        "activation": {
            "type": "PReLU"
        }
	}
}
```
> The "exponent" of "filter_exponent" and "activation" are left empty. <br/>
> required "output_exponent" used to transform this layer bias 

##### 2. use int8 Quantify by layer:
- Layer name: "mylayer"
- operation：Conv2D(input, filter) + bias
- output_exponent：-7, 该卷积层结果ofindex位
- feature_type：s8
- Activation function type: PReLU 

config.json should read:

```json
{
	"mylayer": {
		"operation": "conv2d",
		"feature_type": "s8",
        "bias": "True",
        "output_exponent": -7,
        "activation": {
            "type": "PReLU"
        }
	}
}
```
> required "output_exponent" used to transform this layer bias 

##### 3. use int8 Quantize by channel:
- Layer name: "mylayer"
- operation：Conv2D(input, filter) + bias
- input_exponent：-7, 该卷积层输入ofindex位
- feature_type：s8
- Activation function type: PReLU 

config.json should read:

```json
{
	"mylayer": {
		"operation": "conv2d",
		"feature_type": "s8",
        "bias": "True",
        "input_exponent": -7,
        "activation": {
            "type": "PReLU"
        }
	}
}
```
> required "input_exponent" used to transform this layer bias 

At the same time, `mylayer_filter.npy`, `mylayer_bias.npy` and `mylayer_activation.npy` need to be prepared.