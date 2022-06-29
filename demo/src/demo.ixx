import <iostream>;

import codie.json;

class point
{
public:
    int x;
    int y;
    point(int x, int y) : x(x), y(y) {}

    point(const codie::json::object &obj)
        : x(static_cast<int>(obj["x"].as<double>())),
          y(static_cast<int>(obj["y"].as<double>()))
    {
    }
};

int main(int argc, char **args)
{
    std::cout << "Hello world\n";

    std::string json_text = R"({
        "x": 1,
        "y": 2
    })";

    try
    {
        auto json = codie::json::decode<codie::json::object>(json_text);

        point p = json;

        std::cout << "x: " << p.x << ", y: " << p.y << "\n";
        std::cout << codie::json::encode(json) << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}