BitShares OS X Build Instructions
===============================

1. Install XCode and its command line tools by following the instructions here: https://guide.macports.org/#installing.xcode. 
   In OS X 10.11 (El Capitan) and newer, you will be prompted to install developer tools when running a devloper command in the terminal. This step may not be needed.


2. Install Homebrew by following the instructions here: http://brew.sh/

3. Initialize Homebrew:
   ```
   brew doctor
   brew update
   ```

4. Install dependencies:
   ```
   brew install boost cmake git openssl autoconf automake 
   brew link --force openssl 
   ```

5. *Optional.* To support importing Bitcoin wallet files:
   ```
   brew install berkeley-db
   ```

6. *Optional.* To use TCMalloc in LevelDB:
   ```
   brew install google-perftools
   ```

   Install LibTool

   ```
   brew install libtool
   ```

7. Clone the ledger repository:
   ```
   git clone https://github.com/artwook/ledger.git
   cd ledger
   ```

8. Build BitShares:
   ```
   git submodule update --init --recursive
   cmake .
   make
   ````
   
   In OSX 10.11(EI Capitan) and newer, you will be prompted with warning when force linking openssl. In this case, if openssl are installed using brew, cmake using 
   ```
   cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib .
   ```
