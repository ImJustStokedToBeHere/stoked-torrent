#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <charconv>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace storrent
{
    struct bencode_empty;
    struct bencode_string;
    struct bencode_list;
    struct bencode_dictionary;
    struct bencode_integer;

    enum class bencode_element_type : unsigned short
    {
        unknown = 0x0000,
        empty = 0x0100,
        string = 0x0001,
        integer = 0x0002,
        list = 0x0003,
        dictionary = 0x0004
    };

    enum class bencode_parse_state : unsigned char
    {
        unk,
        parsing,
        parsed,
        errored
    };

    struct bencode_element
    {
    public:
        constexpr bencode_element() : _type{bencode_element_type::unknown} {}

        explicit constexpr bencode_element(bencode_element_type type) : _type{type} {}

        bencode_element(bencode_element&&) = default;
        bencode_element(const bencode_element&) = delete;

        virtual ~bencode_element() {}

        bencode_element& operator=(bencode_element&&) = default;
        bencode_element& operator=(const bencode_element&) = delete;

        virtual std::string str_val() const = 0;
        virtual intptr_t int_val() const = 0;
        virtual const std::vector<std::shared_ptr<bencode_element>>& list_val() const = 0;
        virtual const std::map<std::string, std::shared_ptr<bencode_element>>& dict_val() const = 0;
        virtual std::string encoded_val() const = 0;

        virtual bencode_element_type type() const { return _type; }

    private:
        bencode_element_type _type;
    };

    struct bencode_string : public bencode_element
    {
    public:
        bencode_string() : bencode_string("") {}
        bencode_string(std::string val) : bencode_element(bencode_element_type::string), _val{std::move(val)} {}

        bencode_string(bencode_string&&) = default;
        bencode_string(const bencode_string&) = delete;

        virtual ~bencode_string() {}

        bencode_string& operator=(bencode_string&&) = default;
        bencode_string& operator=(const bencode_string&) = delete;

        void set_val(std::string val) { _val = std::move(val); }

        std::string& value() { return _val; }

        const std::string& value() const { return _val; }

        virtual std::string str_val() const override { return _val; }

        virtual intptr_t int_val() const override
        {
            intptr_t result = 0;
            if (auto convert_result = std::from_chars(_val.c_str(), _val.c_str() + _val.length(), result);
                convert_result.ec == std::errc())
            {
                return result;
            }
            else
            {
                throw std::logic_error("bencode_string cannot be converted to int!");
            }
        }

        virtual const std::vector<std::shared_ptr<bencode_element>>& list_val() const override
        {
            throw std::logic_error("list_val not implemented for bencode_string!");
        }

        virtual const std::map<std::string, std::shared_ptr<bencode_element>>& dict_val() const override
        {
            throw std::logic_error("dict_val not implemented for bencode_string!");
        }

        virtual std::string encoded_val() const override
        {
            return std::to_string(_val.length()).append(":").append(_val);
        }

    private:
        std::string _val;
    };

    struct bencode_integer : public bencode_element
    {
    public:
        bencode_integer() : bencode_integer(0) {}

        bencode_integer(intptr_t val) : bencode_element(bencode_element_type::integer), _val{val} {}

        bencode_integer(bencode_integer&&) = default;
        bencode_integer(const bencode_integer&) = delete;

        virtual ~bencode_integer() {}

        bencode_integer& operator=(bencode_integer&&) = default;
        bencode_integer& operator=(const bencode_integer&) = delete;

        void set_val(intptr_t val) { _val = val; }

        intptr_t& value() { return _val; }

        const intptr_t& value() const { return _val; }

        virtual std::string str_val() const override { return std::to_string(_val); }
        virtual intptr_t int_val() const override { return _val; }
        virtual const std::vector<std::shared_ptr<bencode_element>>& list_val() const override
        {
            throw std::logic_error("list_val not implemented for bencode_string!");
        }
        virtual const std::map<std::string, std::shared_ptr<bencode_element>>& dict_val() const override
        {
            throw std::logic_error("dict_val not implemented for bencode_string!");
        }

        virtual std::string encoded_val() const override { return std::string("i") + std::to_string(_val).append("e"); }

    private:
        intptr_t _val;
    };

    struct bencode_list : public bencode_element
    {
    public:
        bencode_list() : bencode_element(bencode_element_type::list), _val{} {}

        bencode_list(bencode_list&&) = default;
        bencode_list(const bencode_list&) = delete;

        virtual ~bencode_list() {}

        bencode_list& operator=(bencode_list&&) = default;
        bencode_list& operator=(const bencode_list&) = delete;

        const bencode_element* const operator[](size_t idx) { return _val[idx].get(); }

        void append_val(bencode_element* element) { _val.push_back(std::shared_ptr<bencode_element>(element)); }

        void append_val(std::shared_ptr<bencode_element> element) { _val.push_back(std::move(element)); }

        std::vector<std::shared_ptr<bencode_element>>& value() { return _val; }

        const std::vector<std::shared_ptr<bencode_element>>& value() const { return _val; }

        virtual std::string str_val() const override
        {
            throw std::logic_error("str_val not implemented for bencode_string!");
        }
        virtual intptr_t int_val() const override
        {
            throw std::logic_error("int_val not implemented for bencode_string!");
        }
        virtual const std::vector<std::shared_ptr<bencode_element>>& list_val() const override { return value(); }
        virtual const std::map<std::string, std::shared_ptr<bencode_element>>& dict_val() const override
        {
            throw std::logic_error("dict_val not implemented for bencode_string!");
        }

        virtual std::string encoded_val() const override
        {
            std::string result("l");

            for (const auto& elem : _val)
            {
                switch (elem->type())
                {
                    case bencode_element_type::dictionary:
                        result.append(elem->encoded_val());
                        break;
                    case bencode_element_type::list:
                        result.append(elem->encoded_val());
                        break;
                    case bencode_element_type::string:
                        result.append(elem->encoded_val());
                        break;
                    case bencode_element_type::integer:
                        result.append(elem->encoded_val());
                        break;
                }
            }

            return result.append("e");
        }

    private:
        std::vector<std::shared_ptr<bencode_element>> _val;
    };

    struct bencode_dictionary : public bencode_element
    {
    public:
        bencode_dictionary() : bencode_element(bencode_element_type::dictionary), _val{} {}

        bencode_dictionary(bencode_dictionary&&) = default;
        bencode_dictionary(const bencode_dictionary&) = delete;

        virtual ~bencode_dictionary() {}

        bencode_dictionary& operator=(bencode_dictionary&&) = default;
        bencode_dictionary& operator=(const bencode_dictionary&) = delete;

        const bencode_element* const operator[](const std::string& key)
        {
            if (auto itr = _val.find(key); itr != _val.end())
            {
                return itr->second.get();
            }
            else
            {
                return nullptr;
            }
        }

        void append_val(std::string key, std::shared_ptr<bencode_element> element)
        {
            _val.emplace(std::move(key), std::move(element));
        }

        std::map<std::string, std::shared_ptr<bencode_element>>& value() { return _val; }

        const std::map<std::string, std::shared_ptr<bencode_element>>& value() const { return _val; }

        virtual std::string str_val() const override
        {
            throw std::logic_error("str_val not implemented for bencode_string!");
        }
        virtual intptr_t int_val() const override
        {
            throw std::logic_error("int_val not implemented for bencode_string!");
        }
        virtual const std::vector<std::shared_ptr<bencode_element>>& list_val() const override
        {
            throw std::logic_error("list_val not implemented for bencode_string!");
        }
        virtual const std::map<std::string, std::shared_ptr<bencode_element>>& dict_val() const override
        {
            return value();
        }

        virtual std::string encoded_val() const override
        {
            std::string result("d");
            for (const auto& entry : _val)
            {
                result.append(std::to_string(entry.first.length())
                                  .append(":")
                                  .append(entry.first)
                                  .append(entry.second->encoded_val()));
            }
            return result.append("e");
        }

    private:
        std::map<std::string, std::shared_ptr<bencode_element>> _val;
    };

    struct bencode_doc
    {
    public:
        static bencode_parse_state parse_content(const std::string& content,
                                                 std::vector<std::shared_ptr<bencode_element>>& elements,
                                                 std::string& err_msg)
        {
            err_msg.clear();
            if (content.length() > 1)
            {
                size_t idx = 0;
                size_t len = content.length();

                while (idx < len)
                {
                    if (auto elem = read_element(content, idx, err_msg); elem != nullptr)
                    {
                        elements.emplace_back(elem);
                    }
                    else
                    {
                        return bencode_parse_state::errored;
                    }
                }
            }

            return bencode_parse_state::parsed;
        }

        static std::unique_ptr<bencode_doc> parse_doc(const std::string& filename)
        {
            if (std::ifstream file(filename, std::ios::binary); file.good() && file.is_open())
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                auto doc = std::make_unique<bencode_doc>();
                doc->_state = parse_content(content, doc->_elements, doc->_parse_err);
                return doc;
            }
            else
            {
                return nullptr;
            }
        }

        bencode_doc() : _filename{}, _state{bencode_parse_state::unk}, _parse_err{}, _elements{} {}

        virtual ~bencode_doc() {}

        std::string filename() const { return _filename; }

        std::string parse_err() const { return _parse_err; }

        const bencode_element& get_element(size_t idx) const { return *_elements.at(idx).get(); }

        bencode_parse_state state() const { return _state; }

        std::string encoded_val() const
        {
            std::string result;
            for (const auto& elem : _elements)
            {
                result.append(elem->encoded_val());
            }

            return result;
        }

    private:
        static std::shared_ptr<bencode_element> read_element(const std::string& content,
                                                             size_t& idx,
                                                             std::string& err_msg)
        {
            switch (content[idx])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    return read_string(content, idx, err_msg);
                }
                case 'i':
                {
                    return read_integer(content, idx, err_msg);
                }
                case 'l':
                {
                    return read_list(content, idx, err_msg);
                }
                case 'd':
                {
                    return read_dictionary(content, idx, err_msg);
                }
                default:
                    return nullptr;
            }
        }

        static std::shared_ptr<bencode_element> read_string(const std::string& content,
                                                            size_t& idx,
                                                            std::string& err_msg)
        {
            if (size_t pos = content.find(':', idx); pos != std::string::npos)
            {
                size_t parsed_len = 0;

                if (auto parsed_result = std::from_chars(content.c_str() + idx, content.c_str() + pos, parsed_len);
                    parsed_result.ec == std::errc())
                {
                    idx = parsed_len + 1 + pos;

                    std::string v = content.substr(pos + 1, parsed_len);
                    return std::make_shared<bencode_string>(content.substr(pos + 1, parsed_len));
                }
                else
                {
                    err_msg = std::string{"bencode: "} + std::error_condition(parsed_result.ec).message();
                }
            }
            else
            {
                err_msg = "bencode: cannot find closing char: :, idx: " + std::to_string(idx);
            }

            return nullptr;
        }

        static std::shared_ptr<bencode_element> read_integer(const std::string& content,
                                                             size_t& idx,
                                                             std::string& err_msg)
        {
            idx++;
            if (size_t pos = content.find('e', idx); pos != std::string::npos)
            {
                intptr_t val = -1;
                if (auto parsed_result = std::from_chars(content.c_str() + idx, content.c_str() + pos, val);
                    parsed_result.ec == std::errc())
                {
                    idx = pos + 1;
                    return std::make_shared<bencode_integer>(val);
                }
                else
                {
                    err_msg = std::string{"bencode: "} + std::error_condition(parsed_result.ec).message();
                }
            }
            else
            {
                err_msg = "bencode: cannot find closing char: e, bencode file idx: " + std::to_string(idx);
            }

            idx--;
            return nullptr;
        }

        static std::shared_ptr<bencode_element> read_list(const std::string& content, size_t& idx, std::string& err_msg)
        {
            idx++;
            auto list = std::make_shared<bencode_list>();
            while (content[idx] != 'e')
            {
                if (auto elem = read_element(content, idx, err_msg); elem != nullptr)
                {
                    list->append_val(std::move(elem));
                }
                else
                {
                    return nullptr;
                }
            }
            idx++;
            return list;
        }

        static std::shared_ptr<bencode_element> read_dictionary(const std::string& content,
                                                                size_t& idx,
                                                                std::string& err_msg)
        {

            size_t begin_idx = idx++;
            auto dict = std::make_shared<bencode_dictionary>();
            while (content[idx] != 'e')
            {
                if (auto key = read_element(content, idx, err_msg); key != nullptr)
                {
                    if (key->type() == bencode_element_type::string)
                    {
                        if (auto val = read_element(content, idx, err_msg); val != nullptr)
                        {
                            dict->append_val(key.get()->str_val(), std::move(val));
                        }
                        else
                        {
                            dict->append_val(key.get()->str_val(), nullptr);
                        }
                    }
                    else
                    {
                        err_msg = "bencode: dictionary key is not a string, bencode file idx: " + std::to_string(idx);
                        idx = begin_idx;
                        return nullptr;
                    }
                }
                else
                {
                    err_msg = "bencode: dictionary key is cannot be null, bencode file idx: " + std::to_string(idx);
                    idx = begin_idx;
                    return nullptr;
                }
            }

            idx++;
            return dict;
        }

        std::vector<std::shared_ptr<bencode_element>> _elements;
        std::string _filename;
        std::string _parse_err;
        bencode_parse_state _state;
    };
} // namespace storrent
