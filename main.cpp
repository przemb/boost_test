#include <iostream>
#include <boost/array.hpp>

int main(){
  boost::array<int, 4> arr = {{1,2,3,4}};
  
  std::cout << "hi" << arr[0] << "\n";
  
  return 0;
}
