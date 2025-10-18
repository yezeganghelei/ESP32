# 定制层的step介绍 [[English]](../en/implement_custom_layer.md)

The layers implemented by ESP-DL such as Conv2D and DepthwiseConv2D are derived from the base layer **Layer** in [`./include/layer/dl_layer_base.hpp`](../../include/layer/dl_layer_base.hpp). The Layer class has only one member variable, the name `name`. If `name` is not used, there is no need to customize the derived layer of the Layer class, but in order to keep the code consistent we recommend deriving.

The examples in this document are not runnable，For reference only。For a working example，Please refer to [./include/layer/](../../include/layer/) header files in folder，These include Conv2D、DepthwiseConv2D、Concat2D equal layer。

Since the input and output of the layer are both tensors,，**Please be sure to read[Introduction to variables and constants](./about_type_define.md/#Tensor)，Learn about tensors**。

Let’s start customizing the layers！

## step 1：from Layer class derived layer

from Layer class derives a new layer（In the example named `MyLayer`），and define member variables as required、Constructor and destructor。Don’t forget to initialize the constructor of the base class。

```c++
class MyLayer : public Layer
{
private:
    /* private member variables */
public:
    /* public member variables */
    Tensor<int16_t> output; /*<! output of this layer */

    MyLayer(/* arguments */) : Layer(name)
    {
        // initialize anything frozen
    }

    ~MyLayer()
    {
        // destroy
    }
};
```

## step 2：accomplish `build()`

Typically a layer will have one or more inputs and one output. `build()` currently has the following functions:

- **Update output shape**:
    
    The output shape is determined by the input shape，Sometimes it is also affected by the shape of the coefficients。for example，Conv2D The output shape is determined by the input shape、filter shape、stride length和扩张决定，But the input shape may change。Once the input shape changes，The output shape should also change accordingly。`build()` 的第一个作用是根据输入形状Update output shape。

- **Update input padding**: 

    Conv2D、DepthwiseConv2D wait二维卷积层中，Input tensors may need padding。Just like the output shape，Input padding is also determined by the input shape，Sometimes affected by the shape of the coefficients。for example，Conv2D The layer's input fill consists of the input shape、filter shape、stride length、Expansion and filling type determines。`build()` The second function is to update the input filling according to the shape of the input tensor to be filled.。

`build()` Not limited to the above two functions。**All updates based on input can be made by build() accomplish**。

```c++
class MyLayer : public Layer
{
    // ellipsis member variables
    // ellipsis constructor and destructor

    void build(Tensor<int16_t> &input)
    {
        /* get output_shape according to input shape and other configuration */
        this->output.set_shape(output_shape); // update output_shape

        /* get padding according to input shape and other configuration */
        input.set_padding(this->padding);
    }
};
```

## step 3：accomplish call()

exist `call()` mid-implementation level reasoning。please note：

- pass [`Tensor.apply_element()`](../../include/typedef/dl_variable.hpp)、[`Tensor.malloc_element()`](../../include/typedef/dl_variable.hpp) or [`Tensor.calloc_element()`](../../include/typedef/dl_variable.hpp/#122) Give `output.element` **Allocate storage space**；
- **[Introduction to variables and constants](./about_type_define.md/#Tensor)The tensor dimension order described in**，Because both input and output are [`dl::Tensor`](../../include/typedef/dl_variable.hpp)。

```c++
class MyLayer : public Layer
{
    // ellipsis member variables
    // ellipsis constructor and destructor
    // ellipsis build(...)

    Tensor<feature_t> &call(Tensor<int16_t> &input, /* other arguments */)
    {
        this->output.calloc_element(); // calloc memory for output.element

        /* implement operation */
        
        return this->output;
    }
};
```