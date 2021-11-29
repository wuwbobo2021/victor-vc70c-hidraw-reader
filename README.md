# victor-vc70c-hidraw-reader
A decoding program for VICTOR VC70C/86C/86D Multimeter as a computer device.
More information is included in `vc_hidraw.h`.

## Testing
Use these commands to make sure this program works on your computer with your multimeter connected.
```
git clone https://github.com/wuwbobo2021/victor-vc70c-hidraw-reader/ victor-vc70c-hidraw-reader/
cd victor-vc70c-hidraw-reader
sudo make
```

## Known Problems
1. Currently this library is only for Linux platform;
2. Root authority is required to access the VC70C hidraw device;
3. `read_vc_multimeter_hidraw(vc_multimeter_reading*)` will block the current thread until next data is received. This may cause inconvenience of using this library.
4. (minor) When the multimeter is not measuring anything, AC/DC indication in the reading string may be incorrect.
