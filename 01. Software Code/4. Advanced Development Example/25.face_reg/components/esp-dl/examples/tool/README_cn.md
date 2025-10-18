# tool [[English]](./README.md)

ESP-DL It is a warehouse that does not contain peripheral drivers.，Writing a model library[Example](../../examples)时使用了数组保存像素value来表示图片，运行结果只能显示在终端middle。To enable you to experience more fully ESP-DL，我们提供了以下tool用于图片的转换和显示。

## 图片转换tool convert_to_u8.py

该转换tool可将自定义图片转换成 C/C++ array form。Configuration instructions are as follows：

| parameter           | type |value                     |
| :------------- | :-: | :-------------------- |
| -i \| --input  | string | Enter the path to the image |
| -o \| --output | string | Output file path |

**Example:**

Assume,

- 自定义The picture path is my_album/my_image.png
- The output file is stored in the face detection project folder esp-dl/examples/human_face_detect/main middle

but，

```shell
python convert_to_u8.py -i my_album/my_image.png -o ESP-DL/examples/human_face_detect/main/image.hpp
```

> Note: The above code is only an example and is not a valid code.

## # Display tool display_image.py

该显示tool可在图片上绘制用于检测的框和point。Configuration instructions are as follows：

| parameter              | type | value                                                           |
| :---------------- | :-: | :----------------------------------------------------------- |
| -i \| --image | string | Image path |
| -b \| --box       | string | Enter according to the format (x1, y1, x2, y2)，其middle (x1, y1) Represents the coordinates of the upper left corner of the box，(x2, y2) 表示The coordinates of the lower right corner of the box<br />Not configured： No box drawn |
| -k \| --keypoints | string | Enter according to the format (x1, y1, x2, y2, ... , xn, yn)，where each pair (x, y) 表示一个point<br />Not configured： 不绘制point |

**Example:**

Assume,

- The picture path is my_album/my_image.jpg
- Coordinates of the upper left corner of the box: (137, 75)
- The coordinates of the lower right corner of the box： (246, 215)
- point 1 coordinates： (157, 131)
- point 2 coordinates： (158, 177)
- point 3 coordinates： (170, 163)

but，

```shell
python display_image.py -i my_album/my_image.jpg -b "(137, 75, 246, 215)" -k "(157, 131, 158, 177, 170, 163)"
```

> Note: The above code is only an example and is not a valid code.