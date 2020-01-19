# DeepCars

AI learns to drive a car using the NeuroEvolution algorithm.

#### Setup
```bash
git clone --recurse-submodules https://github.com/Coestaris/DeepCars
cd DeepCars
sudo apt install mesa-utils libx11-dev libxi-dev imagemagick-6.q16
sudo apt install make cmake # If needed
```

You need to pack resources before running the application:
```bash
sudo apt install python3 # If needed
cd packer
python3 -m pip install -r requirements.txt # Setup python requirements
python3 packer.py --out_file=<path to folder>/resources.bin --in_dir=../resources/
```
The resource file must be in the same folder as the application executable

To build:
```bash
cmake CMakeLists.txt 
make all
```

## License

[MIT License](https://github.com/Coestaris/DeepCars/blob/master/LICENSE)

The project also bundles third party software under its own licenses:
 - mesa-utils - [MIT license](https://www.mesa3d.org/license.html)
 - libX11 - [X11 license](http://www.xfree86.org/3.3.6/COPYRIGHT2.html)
 - OIL - [MIT license](https://github.com/Coestaris/oil/blob/master/LICENSE)
 - fontbm - [MIT license](https://github.com/vladimirgamalyan/fontbm/blob/master/LICENSE)
