#ifndef COLLECTION_UTILS_H
#define COLLECTION_UTILS_H

// a function that can print out standard library containers, fit for vector, list, also fit for map, set
template <typename T, typename U>
std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& p) {
    out << "[" << p.first << " -> " << p.second << "]";
    return out;
}
template <template <typename, typename> class ContainerType, typename ValueType, typename AllocType>
void print_container(const ContainerType<ValueType, AllocType>& c) {
    for (const auto& v : c)
        cout << v << ' ';
    cout << endl;
}
template <template <typename, typename...> class ContainerType, typename ValueType, typename... Args>
void print_container(const ContainerType<ValueType, Args...>& c) {
    for (const auto& v : c)
        cout << v << ' ';
    cout << endl;
}

#endif