# DeepCars

AI learns to drive a car using the NeuroEvolution algorithm.

#### Setup
```bash
git clone --recurse-submodules https://github.com/Coestaris/DeepCars
cd DeepCars
sudo apt install mesa-utils libx11-dev libxi-dev
sudo apt install make cmake # If needed
```
To build
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
