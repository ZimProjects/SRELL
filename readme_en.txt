How to Use

  Put the following three files in one directory, and include srell.hpp.
  1. srell.hpp
  2. srell_ucfdata2.h (data for case folding)
  3. srell_updata3.h (data for Unicode properties)

The files in the following directories are supplements. As SRELL does not use
them, it is safe to remove them.

* misc
  Contains a source code file for a simple test and benchmark program.

* single-header
  Contains a standalone version of srell.hpp into which srell_ucfdata2.hpp
  and srell_updata2.hpp have been merged.

* unicode
  Contains source code files for programs that generate srell_ucfdata2.hpp
  and srell_update2.hpp from latest Unicode data text files.

