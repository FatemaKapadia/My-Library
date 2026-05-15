#include <vector>
#include <ranges>
#include <expected>
#include <print>

int main() {
    std::vector<int> v = {1, 2, 3};
    auto filtered = v | std::views::filter([](int i) { return i > 1; });
    // auto v2 = std::ranges::to<std::vector<int>>(filtered);
    std::expected<int, int> e = 5;
    return 0;
}
