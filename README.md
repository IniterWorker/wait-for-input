# wait-for-input
Simple Poll Wait For Input

## Example

Create a simple hello_world event on push button raising the GPIO.

### GPIO Configuration

```bash
echo 5      >/sys/class/gpio/export
echo in     >/sys/class/gpio/gpio5/direction
echo rising >/sys/class/gpio/gpio5/edge
```

### Simple

```bash
wait_for_input /sys/class/gpio/gpio5/value echo "hello world"
```