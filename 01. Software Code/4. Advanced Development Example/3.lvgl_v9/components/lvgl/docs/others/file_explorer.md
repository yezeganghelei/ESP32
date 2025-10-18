# File Explorer

`lv_file_explorer` provides an API to browse the contents of the file system. `lv_file_explorer` only provides the file browsing function, but does not provide the actual file operation function. In other words, you can't click a picture file to open and view the picture like a PC. `lv_file_explorer` will tell you the full path and name of the currently clicked file. The file operation function needs to be implemented by the user.

The file list in `lv_file_explorer` is based on [lv_table](/widgets/table), and the quick access bar is based on [lv_list](/widgets/list). Therefore, care should be taken to ensure that [lv_table](/widgets/table) and [lv_list](/widgets/list) are enabled.

<details>
<summary>Chinese</summary>
<p>

`lv_file_explorer` supplyAPIAllows us to browse the contents of the file system。`lv_file_explorer` 只supply了document浏览功能，并不supply实际的document操作功能，That is to say，Can't be likePC那样点击一个图片document就可以打开查看该图片。`lv_file_explorer` 会告诉您当前点击的document的完整pathand名称，document操作功能需要用户自己accomplish。

`lv_file_explorer` The file list in is based on [lv_table](/widgets/table) accomplish，The quick access bar is based on [lv_list](/widgets/list) accomplish。therefore，Be careful to make sure it is enabled `lv_table` and `lv_list`。

</p>
</details>

## Usage

Enable `LV_USE_FILE_EXPLORER` in `lv_conf.h`.

First use `lv_file_explorer_create(lv_scr_act())` to create a file explorer, The default size is the screen size. After that, you can customize the style like widget.

<details>
<summary>Chinese</summary>
<p>

Turn on `LV_USE_FILE_EXPLORER` in `lv_conf.h`.

first，use `lv_file_explorer_create(lv_scr_act())` Function creates a file browser，Default size is screen size，You can then customize the style like a component。

</p>
</details>

### Quick access

The quick access bar is optional. You can turn off `LV_FILE_EXPLORER_QUICK_ACCESS` in `lv_conf.h` so that the quick access bar will not be created. This can save some memory, but not much. After the quick access bar is created, it can be hidden by clicking the button at the top left corner of the browsing area, which is very useful for small screen devices.

You can use `lv_file_explorer_set_quick_access_path(file_explorer, LV_FILE_EXPLORER_QA_XX, "path")` to set the path of the quick access bar. The items of the quick access bar are fixed. Currently, there are the following items:

- `LV_FILE_EXPLORER_QA_HOME`
- `LV_FILE_EXPLORER_QA_MUSIC`
- `LV_FILE_EXPLORER_QA_PICTURES`
- `LV_FILE_EXPLORER_QA_VIDEO`
- `LV_FILE_EXPLORER_QA_DOCS`
- `LV_FILE_EXPLORER_QA_MNT`
- `LV_FILE_EXPLORER_QA_FS`

<details>
<summary>Chinese</summary>
<p>

The quick access bar is optional, you can turn off `LV_FILE_EXPLORER_QUICK_ACCESS` in `lv_conf.h` so that the quick access bar is not created, which saves some memory, but not a lot. After the quick access bar is created, it can be hidden by clicking the button in the upper left corner of the top of the browsing area. This is very useful for small screen devices.

can pass `lv_file_explorer_set_quick_access_path(file_explorer, LV_FILE_EXPLORER_QA_XX, "path")` 设置快速访问栏的path，Quick access bar items are fixed，Currently there are the following projects：

- `LV_FILE_EXPLORER_QA_HOME`
- `LV_FILE_EXPLORER_QA_MUSIC`
- `LV_FILE_EXPLORER_QA_PICTURES`
- `LV_FILE_EXPLORER_QA_VIDEO`
- `LV_FILE_EXPLORER_QA_DOCS`
- `LV_FILE_EXPLORER_QA_MNT`
- `LV_FILE_EXPLORER_QA_FS`

</p>
</details>

### Sort

You can use `lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_XX)` to set sorting method. There are the following sorting methods:

- `LV_EXPLORER_SORT_NONE`
- `LV_EXPLORER_SORT_KIND`

You can customize the sorting. Before custom sort, please set the default sorting to `LV_EXPLORER_SORT_NONE`. The default is `LV_EXPLORER_SORT_NONE`.

<details>
<summary>Chinese</summary>
<p>

You can set the sorting method through `lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_XX)`. The following sorting methods are available:

- `LV_EXPLORER_SORT_NONE`
- `LV_EXPLORER_SORT_KIND`

You can customize the sorting rules. Before doing this, please set the sorting rules to `LV_EXPLORER_SORT_NONE` and then handle it in the `LV_EVENT_READY` event. The default sorting rule is `LV_EXPLORER_SORT_NONE`

</p>
</details>

## Event

- `LV_EVENT_READY` sent shen a directory is opened. You can customize the sort.

- `LV_EVENT_VALUE_CHANGED` sent when an item(file) in the file list is clicked.

You can use `lv_file_explorer_get_cur_path` to get the current path and `lv_file_explorer_get_sel_fn` to get the name of the currently selected file in the event processing function. For example:

```c
static void file_explorer_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        char * cur_path =  lv_file_explorer_get_cur_path(obj);
        char * sel_fn = lv_file_explorer_get_sel_fn(obj);
        LV_LOG_USER("%s%s", cur_path, sel_fn);
    }
}
```

You can also save the obtained **path** and **file** name into an array through functions such as *strcpy* and *strcat* for later use.

<details>
<summary>Chinese</summary>
<p>

- Sent when a directory is opened `LV_EVENT_READY` event。您可以exist这里自定义排序规则。
- When an item in the file list（document）Sent when clicked `LV_EVENT_VALUE_CHANGED` event。

You can pass it in the event handler function `lv_file_explorer_get_cur_path` 获取当前所exist的path，pass `lv_file_explorer_get_sel_fn` Get the name of the currently selected file。for example：

```c
static void file_explorer_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        char * cur_path =  lv_file_explorer_get_cur_path(obj);
        char * sel_fn = lv_file_explorer_get_sel_fn(obj);
        LV_LOG_USER("%s%s", cur_path, sel_fn);
    }
}
```

You can also get the **path** and **File name** pass例如 strcpy and strcat function saved into an array，方便后续use。 

</p>
</details>

## Example

```eval_rst

.. include:: ../../examples/others/file_explorer/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_file_explorer.h
  :project: lvgl

```