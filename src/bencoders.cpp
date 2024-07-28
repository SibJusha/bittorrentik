#include "bencoders.hpp"
#include <iostream>
#include <cstdint>

auto to_tuple(j_pair&& j_pair) noexcept
{
    return std::make_tuple(j_pair.json, j_pair.len);
}

j_pair decode_bencoded_integer(const std::string_view& encoded_value)
{
    // i-42e -> -42
    std::string_view encoded_int_str =
            encoded_value.substr(1, encoded_value.find('e') - 1);
    return {json(std::atoll(encoded_int_str.data())),
            2 + encoded_int_str.length()};
}

j_pair decode_bencoded_str(const std::string_view& encoded_value)
{
    // 3:abc -> "abc"
    size_t colon_index = encoded_value.find(':');
    if (colon_index != std::string::npos) {
        std::string_view number_string(encoded_value.substr(0, colon_index));
        // returns 0 in case it's not a number
        int64_t     number = std::atoll(number_string.data());
        std::string str(encoded_value.substr(colon_index + 1, number));
        return {json(str), number + 1 + colon_index};
    } else {
        throw std::runtime_error(
                "Invalid encoded string value: " + std::string(encoded_value));
    }
}

j_pair decode_bencoded_list(const std::string_view& encoded_value)
{
    // l5:helloi52ee -> [“hello”,52]
    json res_list;

    size_t i = 1;
    while (i < encoded_value.length()) {
        if (std::isdigit(encoded_value[i])) {
            auto [j_value, i_add] =
                    decode_bencoded_str(encoded_value.substr(i));
            res_list.push_back(j_value);
            i += i_add;
            std::cout << j_value.dump() << " " << i_add << std::endl;
        } else if (encoded_value[i] == 'i') {
            auto [j_value, i_add] =
                    decode_bencoded_integer(encoded_value.substr(i));
            res_list.push_back(j_value);
            i += i_add;
            std::cout << j_value.dump() << " " << i_add << std::endl;
        } else if (encoded_value[i] == 'l') {
            auto [j_value, i_add] =
                    decode_bencoded_list(encoded_value.substr(i));
            res_list.push_back(j_value);
            i += i_add;
            std::cout << j_value.dump() << std::endl;
        } else if (encoded_value[i] == 'd') {
            auto [j_value, i_add] = decode_bencoded_dict(encoded_value);
            res_list.push_back(j_value);
            i += i_add;
        } else if (encoded_value[i] == 'e') {
            break;
        } else {
            throw std::runtime_error("Unhandled encoded value in list: "
                                     + std::string(encoded_value));
        }
    }

    return {res_list, i};
}

j_pair decode_bencoded_dict(const std::string_view& encoded_value)
{
    // d<bencoded string><bencoded element>e
    // ! Note that the keys must be bencoded strings
    // d3:foo3:bar5:helloi52ee -> {"foo":"bar","hello":52}
    json res_dict;

    size_t i = 1;
    while (i < encoded_value.length()) {
        json   key, value;
        size_t i_add = 0;

        if (encoded_value[i] == 'e') {
            break;
        } else if (std::isdigit(encoded_value[i])) {
            std::tie(key, i_add) =
                    to_tuple(decode_bencoded_str(encoded_value.substr(i)));
        } else if (encoded_value[i] == 'i') {
            std::tie(key, i_add) =
                    to_tuple(decode_bencoded_integer(encoded_value.substr(i)));
        } else if (encoded_value[i] == 'l') {
            std::tie(key, i_add) =
                    to_tuple(decode_bencoded_list(encoded_value.substr(i)));
        } else if (encoded_value[i] == 'd') {
            std::tie(key, i_add) =
                    to_tuple(decode_bencoded_dict(encoded_value.substr(i)));
        } else {
            throw std::runtime_error("Unhandled encoded key in dict: "
                                     + std::string(encoded_value));
        }

        std::cout << key.dump() << std::endl;
        i += i_add;

        if (std::isdigit(encoded_value[i])) {
            std::tie(value, i_add) =
                    to_tuple(decode_bencoded_str(encoded_value.substr(i)));
        } else if (encoded_value[i] == 'i') {
            std::tie(value, i_add) =
                    to_tuple(decode_bencoded_integer(encoded_value.substr(i)));
        } else if (encoded_value[i] == 'l') {
            std::tie(value, i_add) =
                    to_tuple(decode_bencoded_list(encoded_value.substr(i)));
        } else if (encoded_value[i] == 'd') {
            std::tie(value, i_add) =
                    to_tuple(decode_bencoded_dict(encoded_value.substr(i)));
        } else {
            throw std::runtime_error("Unhandled encoded value in dict: "
                                     + std::string(encoded_value));
        }

        std::cout << value.dump() << std::endl;
        i += i_add;

        res_dict[key] = value;
    }

    return {res_dict, i};
}

json decode_bencoded_value(const std::string_view& encoded_value)
{
    if (std::isdigit(encoded_value[0])) {
        return decode_bencoded_str(encoded_value).json;
    } else if (encoded_value.front() == 'i') {
        return decode_bencoded_integer(encoded_value).json;
    } else if (encoded_value.front() == 'l') {
        return decode_bencoded_list(encoded_value).json;
    } else if (encoded_value.front() == 'd') {
        return decode_bencoded_dict(encoded_value).json;
    } else {
        throw std::runtime_error(
                "Unhandled encoded value: " + std::string(encoded_value));
    }
}
