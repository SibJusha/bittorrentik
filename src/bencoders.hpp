#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

struct j_pair {
    json json;
    size_t len;
};

auto to_tuple(j_pair&& j_pair) noexcept;

j_pair decode_bencoded_integer(const std::string_view& encoded_value);
j_pair decode_bencoded_str(const std::string_view& encoded_value);
j_pair decode_bencoded_list(const std::string_view& encoded_value);
j_pair decode_bencoded_dict(const std::string_view& encoded_value);

json decode_bencoded_value(const std::string_view& encoded_value);