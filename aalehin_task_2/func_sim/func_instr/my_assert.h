
#define MY_ASSERT(condition, message) {if (condition == false) {std::cerr << "In file: " << __FILE__ << " line " << __LINE__ << ": \n"; std::cerr << "ERROR: " << message << "\n"; abort(); }};
