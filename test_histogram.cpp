#include "display.hpp"


int main()
{
    using namespace boost::histogram;

    // make 1d histogram with 5 regular bins from -0.5 to 2
    auto h = make_histogram(axis::regular<>(5, -0.5, 2.0));
    // push some values into the histogram
    for (auto value : {0.5, 0.5, 0.3, -0.2, 1.6, 0.0, 0.1, 0.1, 0.6, 0.4})
        h(value);

    display::display(h);

    return 0;
}
