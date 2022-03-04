#include <iostream>
#include <string>
#include <vector>

class BigInt {
private:
    std::vector<unsigned int> value;
    bool negative = false;

    template<class T>
    static void reverse(T& a) {
        for (int l = 0, r = static_cast<int>(a.size()) - 1; l < r;) {
            std::swap(a[l++], a[r--]);
        }
    }

    static void delete_leading_zeros(std::vector<unsigned int>& a) {
        int i = static_cast<int>(a.size()) - 1;
        for (; i >= 0 && a[i] == 0; i--);
        a.resize(i + 1);
    }

    static bool less(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b) {
        int a_size = a.size(), b_size = b.size();
        if (a_size != b_size) {
            return a_size < b_size;
        }
        for (int i = a_size - 1; i >= 0; i--) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return false;
    }

    static void add(std::vector<unsigned int>& a, const std::vector<unsigned int>& b) {
        if (a.size() < b.size()) {
            a.resize(b.size());
        }
        unsigned long long digit = 0;
        for (int i = 0, size = b.size(); i < size; i++) {
            digit += a[i];
            digit += b[i];
            a[i] = digit;
            digit >>= 32;
        }
        for (int i = b.size(), size = a.size(); i < size && digit > 0; i++) {
            digit += a[i];
            a[i] = digit;
            digit >>= 32;
        }
        if (digit > 0) {
            a.push_back(digit);
        }
    }

    static void invert(std::vector<unsigned int>& a) {
        bool overflow = true;
        for (int i = 0, size = a.size(); i < size; i++) {
            a[i] = ~a[i] + overflow;
            overflow = a[i] == 0;
        }
    }

    static void sub(std::vector<unsigned int>& a, const std::vector<unsigned int>& b) {
        if (a.size() < b.size()) {
            a.resize(b.size());
        }
        long long digit = 0;
        for (int i = 0, size = b.size(); i < size; i++) {
            digit += (1ll << 32) + a[i] - b[i];
            a[i] = digit;
            digit >>= 32;
            digit--;
        }
        for (int i = b.size(), size = a.size(); i < size && digit == -1; i++) {
            digit += (1ll << 32) + a[i];
            a[i] = digit;
            digit >>= 32;
            digit--;
        }
        if (digit == -1) {
            invert(a);
        }
        delete_leading_zeros(a);
    }

    static void mult(std::vector<unsigned int>& a, unsigned int k) {
        unsigned long long digit = 0;
        for (int i = 0, size = a.size(); i < size; i++) {
            digit += static_cast<unsigned long long>(a[i]) * k;
            a[i] = digit;
            digit >>= 32;
        }
        if (digit > 0) {
            a.push_back(digit);
        }
    }

    static std::vector<unsigned int> mult(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b) {
        std::vector<unsigned int> r;
        for (int i = 0, size = a.size(); i < size; i++) {
            std::vector<unsigned int> t(i);
            t.insert(t.end(), b.begin(), b.end());
            mult(t, a[i]);
            add(r, t);
        }
        return r;
    }

    static std::pair<std::vector<unsigned int>, std::vector<unsigned int>> div(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b) {
        std::vector<unsigned int> r;
        if (less(a, b)) {
            return {r, a};
        }
        std::vector<unsigned int> p(a.end() - b.size() + 1, a.end());
        for (int i = a.size() - b.size(); i >= 0; i--) {
            p.insert(p.begin(), a[i]);
            delete_leading_zeros(p);
            unsigned int k = 0;
            std::vector<unsigned int> t;
            for (unsigned long long max_k = (1ll << 32), mid_k; k < max_k - 1;) {
                mid_k = (max_k + k) / 2;
                t = b;
                mult(t, mid_k);
                if (less(p, t)) {
                    max_k = mid_k;
                }
                else {
                    k = mid_k;
                }
            }
            r.push_back(k);
            t = b;
            mult(t, k);
            sub(p, t);
        }
        reverse(r);
        delete_leading_zeros(r);
        return {r, p};
    }

public:
    void debug() const {
        std::cerr << "[debug] " << (negative ? "-" : "") << "value[" << value.size() << "]";
        for (int i = 0, size = value.size(); i < size; i++) {
            std::cerr << ' ' << value[i];
        }
        std::cerr << std::endl;
    }

    BigInt() = default;

    explicit BigInt(const std::vector<unsigned int>& value_, bool negative_ = false) : value(value_), negative(negative_) {
        delete_leading_zeros(value);
        if (value.size() == 0) {
            negative = false;
        }
    }

    explicit BigInt(std::string s) {
        if (s == "") {
            return;
        }
        if (s[0] == '-') {
            negative = true;
        }
        for (int i = negative, size = s.size(); i < size; i++) {
            mult(value, 10);
            if (s[i] != '0') {
                add(value, {static_cast<unsigned int>(s[i] - '0')});
            }
        }
        if (value.size() == 0) {
            negative = false;
        }
    }

    template <class T>
    BigInt(const T& t) : BigInt(std::to_string(t)) {}

    explicit operator std::string() const {
        if (value.size() == 0) {
            return "0";
        }
        std::string s;
        std::pair<std::vector<unsigned int>, std::vector<unsigned int>> p = {value, {}};
        for (; p.first.size() > 0;) {
            p = div(p.first, {10});
            s += '0' + (p.second.size() > 0 ? p.second[0] : 0);
        }
        if (negative) {
            s += '-';
        }
        reverse(s);
        return s;
    }

    std::string toString() const {
        return std::string(*this);
    }

    explicit operator bool() const {
        return value.size() > 0;
    }

    bool operator<(const BigInt& other) const {
        if (negative != other.negative) {
            return negative;
        }
        return (!negative && less(value, other.value)) || (negative && less(other.value, value));
    }

    bool operator>(const BigInt& other) const {
        return other < *this;
    }

    bool operator==(const BigInt& other) const {
        return !(*this < other) && !(other < *this);
    }

    bool operator!=(const BigInt& other) const {
        return *this < other || other < *this;
    }

    bool operator<=(const BigInt& other) const {
        return *this < other || !(other < *this);
    }

    bool operator>=(const BigInt& other) const {
        return other < *this || !(*this < other);
    }

    BigInt operator+() const {
        return std::move(*this);
    }

    BigInt operator-() const {
        return std::move(BigInt(value, !negative));
    }

    BigInt& operator+=(const BigInt& other) {
        if (negative == other.negative) {
            add(value, other.value);
        }
        else {
            negative = less(value, other.value) ? !negative : negative;
            sub(value, other.value);
        }
        return *this;
    }

    BigInt& operator-=(const BigInt& other) {
        if (negative == other.negative) {
            negative = less(value, other.value) ? !negative : negative;
            sub(value, other.value);
        }
        else {
            add(value, other.value);
        }
        return *this;
    }

    BigInt& operator*=(const BigInt& other) {
        negative = negative != other.negative;
        value = mult(value, other.value);
        return *this;
    }

    BigInt& operator/=(const BigInt& other) {
        negative = negative != other.negative;
        value = div(value, other.value).first;
        return *this;
    }

    BigInt& operator%=(const BigInt& other) {
        value = div(value, other.value).second;
        return *this;
    }

    BigInt& operator++() {
        return *this += 1;
    }

    BigInt operator++(int) {
        auto result = *this;
        operator++();
        return result;
    }

    BigInt& operator--() {
        return *this -= 1;
    }

    BigInt operator--(int) {
        auto result = *this;
        operator--();
        return result;
    }

    BigInt operator+(const BigInt& other) const {
        return BigInt(*this) += other;
    }

    BigInt operator-(const BigInt& other) const {
        return BigInt(*this) -= other;
    }

    BigInt operator*(const BigInt& other) const {
        return BigInt(*this) *= other;
    }

    BigInt operator/(const BigInt& other) const {
        return BigInt(*this) /= other;
    }

    BigInt operator%(const BigInt& other) const {
        return BigInt(*this) %= other;
    }
};

template <class T>
bool operator<(const T& t, const BigInt& x) {
    return BigInt(t) < x;
}

template <class T>
bool operator>(const T& t, const BigInt& x) {
    return BigInt(t) > x;
}

template <class T>
bool operator==(const T& t, const BigInt& x) {
    return BigInt(t) == x;
}

template <class T>
bool operator!=(const T& t, const BigInt& x) {
    return BigInt(t) != x;
}

template <class T>
bool operator<=(const T& t, const BigInt& x) {
    return BigInt(t) <= x;
}

template <class T>
bool operator>=(const T& t, const BigInt& x) {
    return BigInt(t) >= x;
}

template <class T>
BigInt operator+(const T& t, const BigInt& x) {
    return BigInt(t) + x;
}

template <class T>
BigInt operator-(const T& t, const BigInt& x) {
    return BigInt(t) - x;
}

template <class T>
BigInt operator*(const T& t, const BigInt& x) {
    return BigInt(t) * x;
}

template <class T>
BigInt operator/(const T& t, const BigInt& x) {
    return BigInt(t) / x;
}

template <class T>
BigInt operator%(const T& t, const BigInt& x) {
    return BigInt(t) % x;
}

std::istream& operator>>(std::istream& stream, BigInt& x) {
    std::string s;
    stream >> s;
    x = BigInt(s);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const BigInt& x) {
    return stream << std::string(x);
}
