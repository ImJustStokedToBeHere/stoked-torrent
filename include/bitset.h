#pragma once
#include <utils.h>
#include <gsl/span>

#include <memory>
#include <stdexcept>
#include <utility>
#include <assert.h>
#include <cassert>
#include <limits>
#include <sstream>
#include <optional>

namespace stoked {
    class bitset {
    public:
        bitset() { }
        bitset(const bitset& other) = default;
        bitset(bitset&& other) = default;

        explicit bitset(gsl::span<uint8_t> values, size_t bit_length)
            : bitset{bit_length} {
            assert(values.size() > -1 && "bitset(gsl::span<uint8_t> values, size_t bit_length): values.size() must be larger than -1");
            from_array(values, 0, static_cast<size_t>(values.size()) );
        }

        explicit bitset(size_t bit_length)
            : _bit_len{bit_length},
            _array_store_size{(bit_length + 31) / 32},
            _array_store{std::unique_ptr<uint32_t[]>(new uint32_t[_array_store_size])},
            _array{gsl::make_span<uint32_t>(_array_store.get(), _array_store.get() + static_cast<ptrdiff_t>(_array_store_size))} {

            assert((length() > 0) && (length() < PTRDIFF_MAX) && "bitset._len: _len must be less than PTRDIFF_T_MAX");
            assert((length() > 0) && (length() < PTRDIFF_MAX) && "bitset(size_t bit_length): bit_length must be less than PTRDIFF_T_MAX");
            assert((_array.size() > -1) && "bitset._array.size() must be larger than -1");
            ZeroMemory(_array_store.get(), _array_store_size * sizeof(uint32_t));
            /*for (size_t i = 0; i < length(); i++) {
                set(i, false);
            }*/
        }

        explicit bitset(gsl::span<bool> values)
            : bitset(values.size()) {
            /*_bit_len {static_cast<size_t>(values.size())},
            _array_store_size {(_bit_len + 31) / 32},
            _array_store {std::unique_ptr<uint32_t[]>(new uint32_t[_array_store_size])},
            _array {gsl::make_span<uint32_t>(_array_store.get(), _array_store.get() + static_cast<ptrdiff_t>(_array_store_size))} {*/

            assert(values.size() > -1 && "bitset(gsl::span<bool> values): values.size() must be larger than -1");

            for (ptrdiff_t i = 0; i < values.size(); i++) {
                set(i, values[i]);
            }
        }

        bitset& operator=(const bitset& other) = default;
        bitset& operator=(bitset&& other) = default;

        inline bool operator==(const bitset& other) const {
            if (_array.size() != other._array.size() || _true_count != other._true_count) {
                return false;
            }

            for (ptrdiff_t i = 0; i < _array.size(); i++) {
                if (_array[i] != other._array[i]) {
                    return false; 
                }
            }

            return true;
        }

        inline bool operator!=(const bitset& other) const {
            return !(*this == other);
        }

        std::optional<size_t> first_true() const {
            return first_true(0, length());
        }

        std::optional<size_t> first_true(size_t start_idx, size_t count) const {
            /*size_t start {0}, end {0};
            size_t loop_end {(std::min)((end_idx / 32), static_cast<size_t>(_array.size() - 1))};
            for (size_t i = (start_idx / 32); i <= loop_end; i++) {
                if (_array[i] == 0)
                    continue;
                start = i * 32;
                end = start + 32;
                start = (start < start_idx) ? start_idx : start;
                end = (end > length()) ? length() : end;
                end = (end > end_idx) ? end_idx : end;
                if (end == length() && end > 0)
                    end--;

                for (size_t j = start; j <= end; j++) {
                    if (get(j)) {
                        return j;
                    }
                }
            }*/

            for (size_t i = start_idx; i < start_idx + count; i++) {
                if (get(i))
                    return i;
            }

            return std::nullopt;
        }

        std::optional<size_t> first_false() const {
            return first_false(0, length());
        }

        std::optional<size_t> first_false(size_t start_idx, size_t count) const {
            // size_t start {0}, end {0};
            // size_t loop_end {(std::min)((end_idx / 32), static_cast<size_t>(_array.size() - 1))};
            /*for (size_t i = (start_idx / 32); i <= loop_end; i++) {
                if (_array[i] == 0)
                    continue;
                start = i * 32;
                end = start + 32;
                start = (start < start_idx) ? start_idx : start;
                end = (end > length()) ? length() : end;
                end = (end > end_idx) ? end_idx : end;
                if (end == length() && end > 0)
                    end--;

                for (size_t j = start; j <= end; j++) {
                    if (!get(j)) {
                        return j;
                    }
                }
            }*/

            for (size_t i = start_idx; i < start_idx + count; i++) {
                if (!get(i))
                    return i;
            }

            return std::nullopt;
        }

        bitset& set_true(std::initializer_list<size_t> list) {
            for (auto& idx : list) {
                set(idx, true);
            }

            return *this;
        }

        bitset& set_false(std::initializer_list<size_t> list) {
            for (auto& idx : list) {
                set(idx, false);
            }

            return *this;
        }

        bitset& set_all(bool val) {
            if (val) {
                for (size_t i = 0; i < _array_store_size; i++) {
                    _array[i] = ~0;
                }

                validate();
            } else {
                for (size_t i = 0; i < _array_store_size; i++) {
                    _array[i] = 0;
                }
                _true_count = 0;
            }

            return *this;
        }

        void to_byte_array(gsl::span<uint8_t> buffer, size_t offset) {
            assert(buffer.data() && "bitset.to_byte_array(gsl::span<uint8_t> buffer, size_t offset): buffer must not be null");
            zero_unused_bits();
            for (size_t i = 0; i < _array_store_size; i++) {
                buffer[offset++] = static_cast<uint8_t>(_array[i] >> 24);
                buffer[offset++] = static_cast<uint8_t>(_array[i] >> 16);
                buffer[offset++] = static_cast<uint8_t>(_array[i] >> 8);
                buffer[offset++] = static_cast<uint8_t>(_array[i] >> 0);
            }

            uint32_t shift{24};
            for (size_t i = _array_store_size * 32; i < length(); i += 8) {
                buffer[offset++] = static_cast<uint8_t>(_array[_array_store_size - 1] >> shift);
                shift -= 8;
            }
        }

        std::string to_string() {
            std::stringstream ss;
            for (size_t i = 0; i < length(); i++) {
                ss << (get(i) ? 'T' : 'F');
            }
            return ss.str();
        }

        std::wstring to_wstring() {
            std::wstringstream ss;
            for (size_t i = 0; i < length(); i++) {
                ss << (get(i) ? L'T' : L'F');
            }
            return ss.str();
        }


        bitset& set(size_t idx, bool val) {
            assert(!(idx < 0 || idx >= length()) && "bitset.get(size_t idx): idx out of range");

            if (val) {
                if ((_array[idx >> 5] & (1 << (31 - (idx & 31)))) == 0) {
                    _true_count++;
                }

                _array[idx >> 5] |= (1 << (31 - (idx & 31)));

            } else {
                if ((_array[idx >> 5] & (1 << (31 - (idx & 31)))) != 0) {
                    _true_count++;
                }

                _array[idx >> 5] &= ~(1 << (31 - (idx & 31)));
            }

            return *this;
        }

        bool get(size_t idx) const {
            assert(!(idx < 0 || idx >= length()) && "bitset.get(size_t idx): idx out of range");
            return (_array[idx >> 5] & (1 << (31 - (idx & 31)))) != 0;
        }

        bitset& not_val(const bitset& value) {
            check(value);

            for (ptrdiff_t i = 0; i < _array.size(); i++) {
                _array[i] = ~value._array[i];
            }
            _true_count = length() - _true_count;
            return *this;
        }

        bitset& and_val(const bitset& value) {
            check(value);

            for (ptrdiff_t i = 0; i < _array.size(); i++) {
                _array[i] &= value._array[i];
            }

            validate();
            return *this;
        }

        bitset& nand_val(const bitset& value) {
            check(value);

            for (ptrdiff_t i = 0; i < _array.size(); i++) {
                _array[i] &= ~value._array[i];
            }

            validate();
            return *this;
        }

        bitset& or_val(const bitset& value) {
            check(value);

            for (ptrdiff_t i = 0; i < _array.size(); i++) {
                _array[i] |= value._array[i];
            }

            validate();
            return *this;
        }

        bitset& xor_val(const bitset& value) {
            check(value);

            for (ptrdiff_t i = 0; i < _array.size(); i++) {
                _array[i] ^= value._array[i];
            }

            validate();
            return *this;
        }

        void check(const bitset& value) const {
            if (value.length() != length()) {
                throw std::logic_error("bitfields are different lengths");
            }
        }

        bool all_false() const {
            return _true_count == 0;
        }

        bool all_true() const {
            return _true_count == length();
        }

        size_t length() const {
            return _bit_len;
            // gsl::span s;
        }

        double true_percent() const {
            return static_cast<double>(_true_count / length() * 100.0);
        }

        size_t byte_len() const {
            return (length() + 7) / 8;
        }

        size_t true_count() const {
            return _true_count;
        }

        size_t false_count() const {
            return length() - _true_count;
        }
    private:
        void from_array(gsl::span<uint8_t> values, size_t start_idx, size_t array_length) {

            // size_t end {_bit_len / 32};
            for (size_t i = 0; i < _array_store_size; i++) {
                uint8_t first = ((static_cast<size_t>(values.size()) >= start_idx + 1) ? values[start_idx++] : 0);
                uint8_t second = ((static_cast<size_t>(values.size()) >= start_idx + 1) ? values[start_idx++] : 0);
                uint8_t third = ((static_cast<size_t>(values.size()) >= start_idx + 1) ? values[start_idx++] : 0);
                uint8_t fourth = ((static_cast<size_t>(values.size()) >= start_idx + 1) ? values[start_idx++] : 0);
                uint32_t val = (first << 24 | second << 16 | third << 8 | fourth << 0);
                _array[i] = val;
            }

            uint32_t shift{24};
            for (size_t i = _array_store_size * 32; i < length(); i += 8) {
                _array[_array.size() - 1] |= values[start_idx++] << shift;
                shift -= 8;
            }

            validate();
        }

        void validate() {
            zero_unused_bits();
            size_t count{0};
            for (size_t i{0}; i < _array_store_size; i++) {
                uint32_t v{_array[i]};
                v = v - ((v >> 1) & 0x55555555);
                v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
                count += (((v + (v >> 4) & 0xF0F0F0F) * 0x1010101)) >> 24;
            }
            _true_count = count;
        }

        void zero_unused_bits() {
            if (_array_store_size == 0) {
                return;
            }

            uint32_t shift{32 - length() % 32};
            if (shift != 0) {
                _array[_array_store_size - 1] &= (-1 << shift);
            }

        }

        size_t _array_store_size{0};
        std::unique_ptr<uint32_t[]> _array_store{nullptr};

        size_t _bit_len{0};
        size_t _true_count{0};
        gsl::span<uint32_t> _array{nullptr, nullptr};
    };
}

