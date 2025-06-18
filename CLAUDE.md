# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is the Bluetooth Low Energy Fundamentals Course repository from Nordic Developer Academy. It contains hands-on exercises for learning Bluetooth LE development using Nordic Semiconductor devices and the nRF Connect SDK.

## Development Commands

### Building an Exercise
```bash
# Navigate to specific exercise directory
cd l2/l2_e1

# Build for nRF52840 DK
west build -b nrf52840dk/nrf52840 --pristine

# Build for nRF52840 Dongle  
west build -b nrf52840dongle/nrf52840 --pristine

# Build for nRF5340 DK (application core)
west build -b nrf5340dk/nrf5340/cpuapp --pristine

# Build for nRF54L15 DK
west build -b nrf54l15dk/nrf54l15/cpuapp --pristine
```

### Flashing
```bash
# Flash to connected device
west flash

# Flash with specific runner
west flash --runner nrfjprog
```

### Common Development Tasks
```bash
# Clean build directory
rm -rf build/

# View serial output
west device-monitor

# Debug with GDB
west debug
```

## Code Architecture

### Exercise Structure
Each lesson (l2-l6) contains numbered exercises following this pattern:
- `lX_eY/` - Exercise skeleton with STEP comments for implementation
- `lX_eY_sol/` - Complete solution

### Key Components

1. **Configuration Files**
   - `prj.conf` - Main project configuration (Bluetooth settings, logging, device name)
   - `Kconfig.sysbuild` - System build configuration (multi-core support)
   - `boards/*.conf` - Board-specific configurations
   - `boards/*.overlay` - Device tree overlays for hardware configuration

2. **Source Organization**
   - `src/main.c` - Application entry point and main logic
   - `src/my_lbs.c/h` (L4) or `src/lbs.c/h` (L5-L6) - LED Button Service implementation

3. **Service Implementation Pattern**
   ```c
   // UUID definition
   #define BT_UUID_LBS_VAL BT_UUID_128_ENCODE(...)
   
   // Characteristic callbacks
   static struct bt_gatt_cb lbs_cb = {
       .read = read_button,
       .write = write_led,
   };
   
   // Service registration
   BT_GATT_SERVICE_DEFINE(lbs_svc, ...);
   ```

### Lesson Progression

1. **L2 - Basic Advertising**: Non-connectable beacons, advertising data
2. **L3 - Connections**: Connection parameters, multi-board support
3. **L4 - Custom GATT Services**: LED Button Service basics
4. **L5 - Notifications**: Advanced GATT features
5. **L6 - Security**: Pairing, bonding, encrypted connections

### Multi-Core Support
For nRF5340:
- Application core configurations in main directory
- Network core configurations in `child_image/` subdirectory
- IPC configurations: `hci_ipc.conf`, `hci_rpmsg.conf`

## Important Notes

- This repository requires nRF Connect SDK v3.0.0 or compatible versions
- West tool must be available from nRF Connect SDK installation
- Each exercise builds upon previous ones - complete them in order
- Solutions contain Japanese comments explaining implementation details
- The build system uses Zephyr RTOS and CMake