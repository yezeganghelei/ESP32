## LED indicator light

LED indicator light是最简单of输出外设之一，The current working status of the system can be indicated by flashing in different forms.。ESP-IoT-Solution 提供of LED indicator light组件具有以下功能：

* Supports defining multiple groups of flashing types
* Support defining flash type priority
* Supports creating multiple indicators

## ## How to use

## ### Predefined flashing types

Blink step structure blink_step_t defines the type of step、indicator light状态和状态持续时间。Multiple steps combined into one flash type，Different flash types can be used to identify different system states。The flicker type is defined as follows：

Example 1. Define a cycle of flashing: on for 0.05 S, off for 0.1 S, and continues to cycle after starting.

```
const blink_step_t test_blink_loop[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 50},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step2: turn off LED 100 ms
    {LED_BLINK_LOOP, 0, 0},                           // step3: loop from step1
};
```

example 2 . Define a cycle of flashing：Bright 0.05 S，destroy 0.1 S，Bright 0.15 S，destroy 0.1 S，执行完毕灯熄destroy。

```
const blink_step_t test_blink_one_time[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 50},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step2: turn off LED 100 ms
    {LED_BLINK_HOLD, LED_STATE_ON, 150},              // step3: turn on LED 150 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step4: turn off LED 100 ms
    {LED_BLINK_STOP, 0, 0},                           // step5: stop blink (off)
};
```

After defining the flicker type，need to be in `led_indicator_blink_type_t` Add the enumeration member corresponding to this type，Then add it to the blink type list `led_indicator_blink_lists`，示For example下：

```
typedef enum {
    BLINK_TEST_BLINK_ONE_TIME, /**< test_blink_one_time */
    BLINK_TEST_BLINK_LOOP,     /**< test_blink_loop */
    BLINK_MAX,                 /**< INVALIED type */ 
} led_indicator_blink_type_t;

blink_step_t const * led_indicator_blink_lists[] = {
    [BLINK_TEST_BLINK_ONE_TIME] = test_blink_one_time,
    [BLINK_TEST_BLINK_LOOP] = test_blink_loop,
    [BLINK_MAX] = NULL,
};
```

#### Predefined flash priorities

对于同一个indicator light，A high priority flash can interrupt an ongoing low priority flash，When high priority flashing ends，Low priority flash resume execution。You can adjust the flicker type by `led_indicator_blink_type_t` The order of enumeration members adjusts the priority of flashing，The smaller the value, the higher the execution priority of the member.。

For example, in the following example, blinking test_blink_one_time has a higher priority than test_blink_loop and can be blinked first.

```
typedef enum {
    BLINK_TEST_BLINK_ONE_TIME, /**< test_blink_one_time */
    BLINK_TEST_BLINK_LOOP,     /**< test_blink_loop */
    BLINK_MAX,                 /**< INVALIED type */ 
} led_indicator_blink_type_t;
```

#### 控制indicator light闪烁

创建一个indicator light：specify a IO 和一组配置信息创建一个indicator light

```
led_indicator_config_t config = {
    .off_level = 0,                              // attach led positive side to esp32 gpio pin
    .mode = LED_GPIO_MODE,
};
led_indicator_handle_t led_handle = led_indicator_create(8, &config); // attach to gpio 8
```

start/stop flashing：控制indicator light开启/Stop specifying flash type，Return immediately after function call，The flashing process is controlled internally by a timer。同一个indicator light可以开启多种类型of闪烁，Will be executed based on flash priority。

```
led_indicator_start(led_handle, BLINK_TEST_BLINK_LOOP); // call to start, the function not block

/*
*......
*/

led_indicator_stop(led_handle, BLINK_TEST_BLINK_LOOP); // call stop
```

删除indicator light：You can also do this when no further action is required，删除indicator light以释放资源

```
led_indicator_delete(&led_handle);
```

> This component supports thread-safe operations，You can use global variables to share LED indicator lightof操作句柄 led_indicator_handle_t，Can also be used led_indicator_get_handle Passed in other threads LED of IO Get the handle to perform the operation。