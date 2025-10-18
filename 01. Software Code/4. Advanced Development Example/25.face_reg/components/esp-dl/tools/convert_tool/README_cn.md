# convert.py Instructions for use [[English]](./README.md)

[convert.py](./convert.py) The script will .npy The floating point coefficients in the file are quantized as C/C++ code，store to .cpp and .hpp in file。The script also converts the element order of the coefficients，thereby speeding up operations。

convert.py according to config.json documentrun。This file is a necessary configuration file for the model。about how to write config.json file document，Please refer to [config.json Configuration specifications](./specification_of_config_json_cn.md)。

Notice，convert.py Need to be in Python 3.7 or higher version。

## #actual parameter description

run convert.py You need to fill in the following actual parameters when：

| Actual parameters            | value                                        |
| :------------------ | :------------------------------------------- |
| -t \| --target_chip | esp32 \| esp32s2 \|esp32s3 \| esp32c3        |
| -i \| --input_root  | npy files and json document所在Table of contents         |
| -j \| --json_file_name  | json document名 <br/>(default: config.json)         |
| -n \| --name        | Output file name        |
| -o \| --output_root | The directory where the output file is located |
| -q \| --quant | Quantification granularity <br/> 0(default) Represents quantification by layer, 1 Represents quantization by channel |

## # Example

hypothesis：

- convert.py The relative path is **./convert.py**
- Target chip is **esp32s3**
- npy files and config.json The file is in **./my_input_directory** Table of contents中
- Output file name为 **my_coefficient**
- The output file will be stored in **./my_output_directory** Table of contents

Run the following command:

```sh
python ./convert.py -t esp32s3 -i ./my_input_directory -n my_coefficient -o ./my_output_directory
```

will then generate `my_coefficient.cpp` and `my_coefficient.hpp` document，stored in `./my_output_directory` Table of contents中。