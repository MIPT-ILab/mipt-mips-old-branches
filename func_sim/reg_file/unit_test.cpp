// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <reg_file.h>

TEST( RegFile, invalid_access)
{   
  ASSERT_DEATH({ RegFile regfile( 128); regfile.read( 128); }, ".*");
  ASSERT_DEATH({ RegFile regfile( 128); regfile.write( 128, 0); }, ".*");
} 

TEST( RegFile, read_write)
{   
  RegFile regfile(1);
  regfile.write(0, 0xDEAD);
  EXPECT_EQ(0xDEAD, regfile.read(0));

}