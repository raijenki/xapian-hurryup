SET(prefix "/home/cc/tailbench-v0.9/xapian/xapian-core-1.2.25/install")
SET(exec_prefix "${prefix}")
SET(XAPIAN_LIBRARIES "${exec_prefix}/lib/libxapian.so" CACHE FILEPATH "Libraries for Xapian")
SET(XAPIAN_INCLUDE_DIR "${prefix}/include" CACHE PATH "Include path for Xapian")
SET(XAPIAN_FOUND "TRUE")
