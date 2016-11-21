# test_cs

To build the project, you'll need cmake and boost (only lockfree is used) installed:

```
$> mkdir build
$> cd build
$> cmake ..
$> make
```

Binary with tests can be found in 'your-path-to-project/build/test' directory.
You can run the application and input orders manually, or you can (should) 
use generate_file.py (see in the root dir) script for generating test file with orders.

Due to the nature of the problem parallelization is in fact useless, since matching is too 
lightweight compared to std in/out operations.

P.S. In case you don't use cmake please let me know, I'll add plain makefiles. 

Cheers!
