# DeepCars

AI learns to drive a car using the NeuroEvolution algorithm.
![](https://raw.githubusercontent.com/Coestaris/DeepCars/master/screenshots/header.jpg)
![](https://raw.githubusercontent.com/Coestaris/DeepCars/master/screenshots/1.gif)
![](https://raw.githubusercontent.com/Coestaris/DeepCars/master/screenshots/2.gif)

#### Setup
```bash
git clone --recurse-submodules https://github.com/Coestaris/DeepCars
cd DeepCars
sudo apt install mesa-utils libx11-dev libxi-dev imagemagick-6.q16 libfreetype6-dev libgtk-3-dev
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
And also you need to install [STB](https://github.com/nothings/stb):
```bash
git clone https://github.com/nothings/stb.git
sudo cp stb/stb_image.h /usr/local/include/
```
#### Build project
To build project simply use:
```bash
cmake CMakeLists.txt 
make all
```
#### Application resources
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
 - mesa-utils - [MIT License](https://www.mesa3d.org/license.html)
 - libX11 - [X11 License](http://www.xfree86.org/3.3.6/COPYRIGHT2.html)
 - OIL - [MIT License](https://github.com/Coestaris/oil/blob/master/LICENSE)
 - fontbm - [MIT License](https://github.com/vladimirgamalyan/fontbm/blob/master/LICENSE)
 - Chipmunk2D - [MIT License](https://github.com/slembcke/Chipmunk2D/blob/master/LICENSE.txt)
 - freetype - [Free Type License (FTL)](https://git.savannah.gnu.org/cgit/freetype/freetype2.git/tree/docs/FTL.TXT)
 - osdialog - [CC0 License](https://github.com/AndrewBelt/osdialog/blob/master/LICENSE.txt)
 - stb - [MIT License](https://github.com/nothings/stb/blob/master/LICENSE)
