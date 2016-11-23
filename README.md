# test_cs

To build the project, you'll need boost.lockfree lib. 

After the build you'll find two binaries in the project's root dir - 'engine-exec' and 'tests-exec'.

You can run the engine and input orders manually, or you can (should) 
use generate_file.py (see in the root dir) script for generating test file with orders.
E.g. if you want to run the engine with 1000000 orders you can do:

```
$> ./generate_file.py 1000000 > orders.txt
$> ./engine-exec < orders.txt > log.txt
```

Cheers!
