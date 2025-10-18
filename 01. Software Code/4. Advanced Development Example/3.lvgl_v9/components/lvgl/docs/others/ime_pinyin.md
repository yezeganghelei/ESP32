# Pinyin IME

Pinyin IME provides API to provide Chinese Pinyin input method (Chinese input) for keyboard object, which supports 26 key and 9 key input modes. You can think of `lv_ime_pinyin` as a Pinyin input method plug-in for keyboard objects.

Normally, an environment where [lv_keyboard](/widgets/keyboard) can run can also run `lv_ime_pinyin`. There are two main influencing factors: the size of the font file and the size of the dictionary.

<details>
<summary>Chinese</summary>
<p>

`lv_ime_pinyin`for[key盘](/widgets/keyboard)The component provides Chinese Pinyin input method（Chinese输入）function(Hereinafter referred to as Pinyin input method)，support26key和9key输入模式。您可以Will `lv_ime_pinyin` Chinese Pinyin input method plug-in regarded as a keyboard component。

Normally，As long as it is[key盘](/widgets/keyboard)组件能运行of环境 `lv_ime_pinyin` Also works。There are two influencing factors：Font size and vocabulary size。

</p>
</details>

## Usage

Enable `LV_USE_IME_PINYIN` in `lv_conf.h`.

First use `lv_ime_pinyin_create(lv_scr_act())` to create a Pinyin input method plug-in, then use `lv_ime_pinyin_set_keyboard(pinyin_ime, kb)` to add the `keyboard` you created to the Pinyin input method plug-in.
You can use `lv_ime_pinyin_set_dict(pinyin_ime, your_dict)` to use a custom dictionary (if you don't want to use the built-in dictionary at first, you can disable `LV_IME_PINYIN_USE_DEFAULT_DICT` in `lv_conf.h`, which can save a lot of memory space).

The built-in thesaurus is customized based on the **LV_FONT_SIMSUN_16_CJK** font library, which currently only has more than `1,000` most common CJK radicals, so it is recommended to use custom fonts and thesaurus.

In the process of using the Pinyin input method plug-in, you can change the keyboard and dictionary at any time.

<details>
<summary>Chinese</summary>
<p>

exist `lv_conf.h` Open in `LV_USE_IME_PINYIN`。

First, use the `lv_ime_pinyin_create(lv_scr_act())` function to create a Pinyin input method plug-in,
Thenuse `lv_ime_pinyin_set_keyboard(pinyin_ime, kb)` 函数Will您创建ofkey盘组件添加到插件中。

The built-in vocabulary is based on LVGL of **LV_FONT_SIMSUN_16_CJK** Font customization，This font currently only has `1000` 多个最常见of CJK radical，所以建议use自定义字库和词库。

You can use the `lv_ime_pinyin_set_dict(pinyin_ime, your_dict)` function to set up a custom lexicon. If you do not plan to use the built-in lexicon from the beginning, it is recommended that you turn off `LV_IME_PINYIN_USE_DEFAULT_DICT` in `lv_conf.h`, which can save some memory space.

</p>
</details>

## Custom dictionary

If you don't want to use the built-in Pinyin dictionary, you can use the custom dictionary.
Or if you think that the built-in phonetic dictionary consumes a lot of memory, you can also use a custom dictionary.

Customizing the dictionary is very simple.

First, set `LV_IME_PINYIN_USE_DEFAULT_DICT` to `0` in `lv_conf.h`

Then, write a dictionary in the following format.

<details>
<summary>Chinese</summary>
<p>

If you don't want to use the built-in lexicon, you can customize the lexicon as follows.

Customizing a dictionary is very easy。
first，exist `lv_conf.h` Will `LV_IME_PINYIN_USE_DEFAULT_DICT` set to 0。
Then write the vocabulary in the following format.

</p>
</details>

### Dictionary format

The arrangement order of each pinyin syllable is very important. You need to customize your own thesaurus according to the Hanyu Pinyin syllable table. You can read [here](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981) to learn about the Hanyu Pinyin syllables and the syllable table.

Then, write your own dictionary according to the following format:

<details>
<summary>Chinese</summary>
<p>

**Notice**，The order of the pinyin syllables is very important，您需要按照汉语Pinyinsyllabary定制自己of词库，can read[here](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981)learn[Chinese pinyin syllables](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981)as well as[syllabary](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981#1)。

Then，Write your own lexicon according to the following format：

</p>
</details>

```c
lv_100ask_pinyin_dict_t your_pinyin_dict[] = {
            { "a", "Ah ah acridine" },
            { "ai", "爱咿嗿困嗿嗿爱尗疬翑ai" },
            { "an", "An'an'an'an An'an An'an Ammonia Amine Factory Guang'an Luo'an Ammonium Eucalyptus Acquaintance Quail'an" },
            { "ang", "High-spirited" },
            { "ao", "Ao Ao Ao Ao Ao Chao Ao" },
            { "ba", "Babababababababababababababababababababababababadi rake" },
            { "bai", "A hundred defeats in vain" },
            /* ...... */
            { "zuo", "Yesterday, Zuo Zuo pretended to sit down and pick up the trees."},
            {NULL, NULL}

```

**The last item** must end with `{null, null}` , or it will not work properly.

### Apply new dictionary

After writing a dictionary according to the above dictionary format, you only need to call this function to set up and use your dictionary:

<details>
<summary>Chinese</summary>
<p>

After writing your own lexicon according to the above lexicon format,，Refer to the usage below，call `lv_100ask_pinyin_ime_set_dict(pinyin_ime, your_pinyin_dict)` 函数即可设置和use新词库：

</p>
</details>

```c
    lv_obj_t * pinyin_ime = lv_100ask_pinyin_ime_create(lv_scr_act());
    lv_100ask_pinyin_ime_set_dict(pinyin_ime, your_pinyin_dict);
```

## Modes

The lv_ime_pinyin have the following modes:

- `LV_IME_PINYIN_MODE_K26` Pinyin 26 key input mode
- `LV_IME_PINYIN_MODE_K9` Pinyin 9 key input mode
- `LV_IME_PINYIN_MODE_K9_NUMBER` Numeric keypad mode

The `TEXT` modes' layout contains buttons to change mode.

To set the mode manually, use `lv_ime_pinyin_set_mode(pinyin_ime, mode)` . The default mode is `LV_IME_PINYIN_MODE_K26` .

<details>
<summary>Chinese</summary>
<p>

lv_ime_pinyin There are following modes：

- `LV_IME_PINYIN_MODE_K26` Pinyin 26 keys
- `LV_IME_PINYIN_MODE_K9` Pinyin9key(Jiugongge)
- `LV_IME_PINYIN_MODE_K9_NUMBER` Numeric keyboard with nine-square grid layout

Each mode's layout contains buttons to change to other modes.

You can pass `lv_keyboard_set_mode(kb, mode)` Function manual setting mode。默认of模式是 `LV_IME_PINYIN_MODE_K26` 。

</p>
</details>

## Example

```eval_rst

.. include:: ../../examples/others/ime/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_ime_pinyin.h
  :project: lvgl

```