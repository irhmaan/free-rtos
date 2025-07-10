| Supported Targets | ESP32 | -------- |

# Blink Example

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

- A development board with normal LED or addressable LED on-board (e.g., ESP32-Devkit, etc.)
- A USB cable for Power supply and programming

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type `Ctrl-]`.)

## Example Output

As you run the example, you will see the LED blinking, according to the previously defined period.

```text
I (315) INFO: Switching GPIO 5: ON
I (325) INFO: Switching GPIO 5: OFF
I (1325) INFO: Switching GPIO 10: ON
I (2325) INFO: Switching GPIO 10: OFF
```

## Troubleshooting

- If the LED isn't blinking, check the GPIO or the LED type selection in the `Example Configuration` menu.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
