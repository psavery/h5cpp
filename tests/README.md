

To install gtest on ubuntu 18.04:

```
sudo apt-get install libgtest-dev cmake

cd /usr/src/gtest

sudo cmake CMakeLists.txt
sudo make

sudo cp *.a /usr/lib

sudo mkdir /usr/local/lib/gtest
sudo ln -s /usr/lib/libgtest.a /usr/local/lib/gtest/libgtest.a
sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/gtest/libgtest_main.a
```
