# DeepCars

AI learns to drive a car using the NeuroEvolution algorithm.

#### Setup
```bash
git clone --recurse-submodules https://github.com/Coestaris/DeepCars
cd DeepCars
sudo apt install mesa-utils libx11-dev libxi-dev imagemagick-6.q16
sudo apt install make cmake # If needed
```
Also you need to install [Chipmunk2D](https://github.com/slembcke/Chipmunk2D) by following it's instalation instructions like:
```
git clone https://github.com/slembcke/Chipmunk2D
cd Chipmunk2D
cmake CMakeLists.txt 
make 
sudo make install
```

To build project:
```bash
cmake CMakeLists.txt 
make all
```

You need to pack resources before running the application:
```bash
sudo apt install python3 # If needed
cd packer
python3 -m pip install -r requirements.txt # Setup python requirements
python3 packer.py --out_file=<path to folder>/resources.bin --in_dir=../resources/
```
The resource file must be in the same folder as the application executable


## License

[MIT License](https://github.com/Coestaris/DeepCars/blob/master/LICENSE)

The project also bundles third party software under its own licenses:
 - mesa-utils - [MIT license](https://www.mesa3d.org/license.html)
 - libX11 - [X11 license](http://www.xfree86.org/3.3.6/COPYRIGHT2.html)
 - OIL - [MIT license](https://github.com/Coestaris/oil/blob/master/LICENSE)
 - fontbm - [MIT license](https://github.com/vladimirgamalyan/fontbm/blob/master/LICENSE)
 - Chipmunk2D - [MIT license](https://github.com/slembcke/Chipmunk2D/blob/master/LICENSE.txt)
 - freetype - [Free Type License (FTL)](https://git.savannah.gnu.org/cgit/freetype/freetype2.git/tree/docs/FTL.TXT)
