# Wait For Input

Wait for input; It's a primary tool to configure GPIO and poll events.

Describe GPIO Poll like button trigger very quickly, easily and clearly.

## Why I wrote it ?

I wrote this tool to avoid the bash spread of export/direction/edge. When I create a yocto solution for embedded purposes, I need to write a simple file to describe buttons and basic events quickly, easily and clearly.

## Dependencies

- LibCJSON
- API GPIO, sysfs

## Example

Create a simple hello_world event on push button raising the GPIO.

### GPIO Configuration

See more examples  `./example`

#### Basic
```json
{
    "gpios": [
        {
            "name": "btn_helloworld",
            "gpio_number": 22,
            "sh": "echo btn helloworld"
        }
    ]
}
```

#### Advanced

Advanced example with curl and avanced options.

```json
{
	"poll": {
		"maxfd": 128,
		"timeout": -1
	},
	"gpios": [
		{
			"name": "btn_reset_inc",
			"gpio_number": 22,
			"sh": "curl -d '+1' -XPATCH https://kvdb.io/${API_TOKEN}/hits",
			"edge": "falling",
			"direction": "in",
			"debounce": 200,
			"allow_already_exported": true
		},
		{
			"name": "btn_dec",
			"gpio_number": 27,
			"sh": "curl -d '-1' -XPATCH https://kvdb.io/${API_TOKEN}/hits",
			"edge": "falling",
			"direction": "in",
			"debounce": 2000,
			"allow_already_exported": true
		}
	]
}
```

__NOTE__ : debounce unit **ms**

### Simple

```sh
wait_for_input ~/rpi.json
```

### ToDo
- Describe non-poll gpios
