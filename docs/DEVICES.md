# Devices in Soaplin
Soaplin follow UNIX's "Everything is a file" principe, and so devices are located in a special temporary file system called "devfs"

## Device names
Devices in the devfs should have a name, as they are files.

### Character devices

| Device Name     | Type  | Description |
|-----------------|-------|-------------|
| fb(num)         | Char. | Used for accessing a framebuffer |
| tty             | Char. | Used for accessing the TTY for the current process |
| tty(num)         | Char. | Used for accessing a software TTY |
| ttyS(num)         | Char. | Used for accessing a serial/UART TTY |

### Block devices

| Device Name     | Type  | Description |
|-----------------|-------|-------------|
| sd(letter)         | Block | Used to access a SATA disk's data |
| vd(letter)         | Block | Used to access a VirtIO block device's data |