#include <json.hpp>

using namespace nlohmann;

int main()
{
    // create JSON array
    json array = {"first", "2nd", "third", "fourth"};

    // output element at index 2 (third element)
    std::cout << array.at(2) << '\n';

    // change element at index 1 (second element) to "second"
    array.at(1) = "second";

    // output changed array
    std::cout << array << '\n';

    // try to write beyond the array limit
    try
    {
        array.at(5) = "sixth";
    }
    catch (std::out_of_range)
    {
        std::cout << "out of range" << '\n';
    }
}
