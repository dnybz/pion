# circular_buffer_stl
Boost's circular_buffer module converted to use all C++11 STL primitives, making it free of Boost dependencies.

This requires a C++11 capable compiler, and so far has been tested on VS2015. Unfortunately, there's not currently any "fallback to Boost" code (i.e. like ASIO standalone), but I may try to add it someday.
