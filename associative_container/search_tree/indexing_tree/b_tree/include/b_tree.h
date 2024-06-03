#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_TEMPLATE_REPO_B_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_TEMPLATE_REPO_B_TREE_H

#include <search_tree.h>

template<typename tkey, typename tvalue>
class b_tree final: public search_tree<tkey, tvalue>
{

private:

    void insertion(typename associative_container<tkey, tvalue>::key_value_pair &&pair);

public:

    void insert(tkey const &key, tvalue const &value) override;

    void insert(tkey const &key, tvalue &&value) override;

    tvalue const &obtain(tkey const &key) override;

    tvalue dispose(tkey const &key) override;

    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> obtain_between(
        tkey const &lower_bound,
        tkey const &upper_bound,
        bool lower_bound_inclusive,
        bool upper_bound_inclusive) override;

public:

    class infix_iterator final
    {

        friend class b_tree<tkey, tvalue>;
    
    private:

        std::stack<std::pair<typename search_tree<tkey, tvalue>::common_node *, int>> _path;

    public:

        explicit infix_iterator(typename search_tree<tkey, tvalue>::common_node *subtree_root);

        explicit infix_iterator(std::stack<std::pair<typename search_tree<tkey, tvalue>::common_node *, int>> &path);

        bool operator==(infix_iterator const &other) const noexcept;

        bool operator!=(infix_iterator const &other) const noexcept;

        infix_iterator &operator++();

        infix_iterator operator++(int not_used);

        std::tuple<size_t, size_t, tkey const &, tvalue &> operator*() const;

    };

    class infix_const_iterator final
    {

    private:

        typename b_tree<tkey, tvalue>::infix_iterator _iterator;

    public:

        explicit infix_const_iterator(typename search_tree<tkey, tvalue>::common_node *subtree_root);

        bool operator==(infix_const_iterator const &other) const noexcept;

        bool operator!=(infix_const_iterator const &other) const noexcept;

        infix_const_iterator &operator++();

        infix_const_iterator operator++(int not_used);

        std::tuple<size_t, size_t, tkey const &, tvalue const &> operator*() const;

    };
private:

    size_t _t;

public:

    explicit b_tree(
        size_t t,
        std::function<int(tkey const &, tkey const &)> keys_comparer = std::less<tkey>(),
        allocator *allocator = nullptr,
        logger *logger = nullptr);

    b_tree(b_tree<tkey, tvalue> const &other);

    b_tree<tkey, tvalue> &operator=(b_tree<tkey, tvalue> const &other);

    b_tree(b_tree<tkey, tvalue> &&other) noexcept;

    b_tree<tkey, tvalue> &operator=(b_tree<tkey, tvalue> &&other) noexcept;

    ~b_tree();

public:

    infix_iterator begin_infix() const noexcept;

    infix_iterator end_infix() const noexcept;

    infix_const_iterator cbegin_infix() const noexcept;

    infix_const_iterator cend_infix() const noexcept;

private:

    void clear_node(typename search_tree<tkey, tvalue>::common_node *node) noexcept;

    void clear() noexcept;

    typename search_tree<tkey, tvalue>::common_node * copy_node(typename search_tree<tkey, tvalue>::common_node *node);

    typename search_tree<tkey, tvalue>::common_node * copy();

    inline size_t get_min_keys_count() const noexcept;

    inline size_t get_max_keys_count() const noexcept;

};

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::infix_iterator::infix_iterator(std::stack<std::pair<typename search_tree<tkey, tvalue>::common_node *, int>> &path) : _path(path)
{}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::infix_iterator::infix_iterator(typename search_tree<tkey, tvalue>::common_node *subtree_root)
{
    while (subtree_root) {
        _path.push(std::make_pair(subtree_root, 0));
        subtree_root = subtree_root->subtrees[0];
    }
}

template<typename tkey, typename tvalue>
bool b_tree<tkey, tvalue>::infix_iterator::operator==(typename b_tree::infix_iterator const &other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }

    if (_path.empty() ^ other._path.empty()) {
        return false;
    }

    return _path.top() == other._path.top();
}

template<typename tkey, typename tvalue>
bool b_tree<tkey, tvalue>::infix_iterator::operator!=(typename b_tree::infix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_iterator &b_tree<tkey, tvalue>::infix_iterator::operator++()
{
    if (_path.empty()) {
        return *this;
    }
    auto *node = _path.top().first;
    auto index = _path.top().second;

    if (node->subtrees[0]) {
        index = ++_path.top().second;
        while (node->subtrees[index]) {
            node = node->subtrees[index];
            index = 0;
            _path.push(std::make_pair(node, 0));
        }
        return *this;
    }

    if (index != node->virtual_size - 1) {
        ++_path.top().second;
        return *this;
    }

    do {
        _path.pop();
        if (!_path.empty()) {
            node = _path.top().first;
            index = _path.top().second;
        }
    } while (!_path.empty() && index == node->virtual_size - (!(node->subtrees[0]) ? 1 : 0));

    return *this;
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_iterator b_tree<tkey, tvalue>::infix_iterator::operator++(int not_used)
{
    auto iterator = *this;
    ++*this;
    return iterator;
}

template<typename tkey, typename tvalue>
std::tuple<size_t, size_t, tkey const &, tvalue &> b_tree<tkey, tvalue>::infix_iterator::operator*() const
{
    auto &pair = _path.top().first->keys_and_values[_path.top().second];
    return std::tuple<size_t, size_t, tkey const &, tvalue &>(_path.size() - 1, _path.top().second, std::cref(pair.key), std::ref(pair.value));
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(typename search_tree<tkey, tvalue>::common_node *subtree_root) : _iterator(subtree_root)
{}

template<typename tkey, typename tvalue>
bool b_tree<tkey, tvalue>::infix_const_iterator::operator==(b_tree::infix_const_iterator const &other) const noexcept
{
    return _iterator == other._iterator;
}

template<typename tkey,typename tvalue>
bool b_tree<tkey, tvalue>::infix_const_iterator::operator!=(b_tree::infix_const_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename b_tree<tkey, tvalue>::infix_const_iterator &b_tree<tkey, tvalue>::infix_const_iterator::operator++()
{
    ++_iterator;
    return *this;
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_const_iterator b_tree<tkey, tvalue>::infix_const_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey,typename tvalue>
std::tuple<size_t, size_t, tkey const &, tvalue const &> b_tree<tkey, tvalue>::infix_const_iterator::operator*() const
{
    auto data = *_iterator;
    return std::tuple<size_t, size_t, tkey const &, tvalue const &>(std::get<0>(data), std::get<1>(data), std::cref(std::get<2>(data)), std::cref(std::get<3>(data)));
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_iterator b_tree<tkey, tvalue>::begin_infix() const noexcept
{
    return b_tree<tkey, tvalue>::infix_iterator(this->_root);
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_iterator b_tree<tkey, tvalue>::end_infix() const noexcept
{
    return b_tree<tkey, tvalue>::infix_iterator(nullptr);
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_const_iterator b_tree<tkey, tvalue>::cbegin_infix() const noexcept
{
    return b_tree<tkey, tvalue>::infix_const_iterator(this->_root);
}

template<typename tkey, typename tvalue>
typename b_tree<tkey, tvalue>::infix_const_iterator b_tree<tkey, tvalue>::cend_infix() const noexcept
{
    return b_tree<tkey, tvalue>::infix_const_iterator(nullptr);
}

template<typename tkey, typename tvalue>
inline size_t b_tree<tkey, tvalue>::get_min_keys_count() const noexcept
{
    return _t - 1;
}

template<typename tkey, typename tvalue>
inline size_t b_tree<tkey, tvalue>::get_max_keys_count() const noexcept
{
    return 2 * _t - 1;
}

template<typename tkey, typename tvalue>
void b_tree<tkey, tvalue>::insertion(typename associative_container<tkey, tvalue>::key_value_pair &&pair)
{
    // Получаем путь до узла, в который нужно вставить новый элемент.
    auto path = this->find_path(pair.key);
    auto* node = *path.top().first;
    
    // Если узел пуст и это корень дерева, создаем новый узел и вставляем в него пару.
    if (!node && path.size() == 1) {
        typename search_tree<tkey ,tvalue>::common_node * new_node;
        *path.top().first = new_node = this->create_node(_t);
        allocator::construct(new_node->keys_and_values, std::move(pair));
        ++new_node->virtual_size;
        return;
    }

    // Проверяем на дублирование ключа.
    if (path.top().second >= 0) {
        throw std::logic_error("Key duplication\n");
    }

    size_t subtree_index = -path.top().second - 1;
    typename search_tree<tkey, tvalue>::common_node *right_subtree = nullptr;

    while (true) {
        // Если в узле достаточно места, вставляем новую пару.
        if (node->virtual_size < get_max_keys_count()) {
            this->node_insert(node, std::move(pair), subtree_index, right_subtree);
            return;
        }

        // Если узел переполнен, делим его.
        auto res = this->node_split(node, std::move(pair), subtree_index, right_subtree);
        right_subtree = res.first;
        pair = std::move(res.second);

        // Если узел является корнем, создаем новый корень и обновляем структуру дерева.
        if (path.size() == 1) {
            typename search_tree<tkey, tvalue>::common_node *new_root = this->create_node(_t);
            new_root->virtual_size = 1;
            allocator::construct(new_root->keys_and_values, std::move(pair));
            new_root->subtrees[0] = node;
            new_root->subtrees[1] = right_subtree;
            *path.top().first = new_root;
            return;
        }

        // Продолжаем вставку в родительский узел.
        path.pop();
        node = *path.top().first;
        subtree_index = -path.top().second - 1;
    }
}

template<typename tkey, typename tvalue>
void b_tree<tkey, tvalue>::insert(tkey const &key, tvalue const &value)
{
    insertion(std::move(typename associative_container<tkey, tvalue>::key_value_pair(key, value)));
}

template<typename tkey,typename tvalue>
void b_tree<tkey, tvalue>::insert(tkey const &key, tvalue &&value)
{
    insertion(std::move(typename associative_container<tkey, tvalue>::key_value_pair(key, std::move(value))));
}

template<typename tkey, typename tvalue>
tvalue const &b_tree<tkey, tvalue>::obtain(tkey const &key)
{
    auto path = this->find_path(key);
    if (path.top().second < 0) {
        throw std::logic_error("Key is not in tree\n");
    }

    return (*path.top().first)->keys_and_values[path.top().second].value;
}

template<typename tkey, typename tvalue>
tvalue b_tree<tkey, tvalue>::dispose(tkey const &key)
{
    // Получаем путь до узла, в котором находится удаляемый ключ.
    auto path = this->find_path(key);
    
    // Если ключ не найден, выбрасываем исключение.
    if (path.top().second < 0) {
        throw std::logic_error("Key is not in tree\n");
    }

    // Если узел имеет поддеревья, ищем узел с заменой.
    if ((*path.top().first)->subtrees[0]) {
        auto node = path.top();
        path.pop();
        typename search_tree<tkey, tvalue>::common_node **iterator = node.first;

        // Ищем узел для замены.
        while (*iterator) {
            auto index = *iterator == *node.first ? node.second : (*iterator)->virtual_size;
            path.push(std::make_pair(iterator, -index - 1));
            iterator = (*iterator)->subtrees + index;
        }
        
        // Меняем местами удаляемый элемент и элемент в листовом узле.
        search_tree<tkey, tvalue>::swap(std::move((*node.first)->keys_and_values[node.second]), std::move((*path.top().first)->keys_and_values[(*path.top().first)->virtual_size - 1]));
        path.top().second = -path.top().second - 2;
    }

    auto target = *path.top().first;
    auto pair_index = path.top().second;
    path.pop();

    // Удаляем элемент из узла и сдвигаем оставшиеся элементы.
    for (size_t i = pair_index + 1; i < target->virtual_size; ++i) {
        search_tree<tkey, tvalue>::swap(std::move(target->keys_and_values[i - 1]), std::move(target->keys_and_values[i]));
    }
    
    tvalue value = std::move(target->keys_and_values[--target->virtual_size].value);
    allocator::destruct(target->keys_and_values + target->virtual_size);

    // Восстанавливаем баланс дерева.
    while (true) {
        // Если в узле достаточно элементов, возвращаем значение.
        if (target->virtual_size >= get_min_keys_count()) {
            return value;
        }

        // Если путь пуст, удаляем корень, если он пустой.
        if (!path.size()) {
            if (!target->virtual_size) {
                this->_root = target->subtrees[0];
                this->destroy_node(target);
            }
            return value;
        }

        // Иначе получаем родительский узел.
        typename search_tree<tkey, tvalue>::common_node * parent = *path.top().first;
        size_t position = -path.top().second - 1;
        path.pop();

        bool has_left_brother = position != 0;
        bool has_right_brother = position != parent->virtual_size;
        
        // Если у левого брата достаточно элементов, перемещаем элемент из левого брата в текущий узел.
        if (has_left_brother && parent->subtrees[position - 1]->virtual_size > get_min_keys_count()) {
            auto * left_brother = parent->subtrees[position - 1];
            search_tree<tkey, tvalue>::swap(std::move(parent->keys_and_values[position - 1]), std::move(left_brother->keys_and_values[left_brother->virtual_size - 1]));

            allocator::construct(target->keys_and_values + target->virtual_size, std::move(left_brother->keys_and_values[left_brother->virtual_size - 1]));
            search_tree<tkey, tvalue>::swap(std::move(left_brother->subtrees[left_brother->virtual_size]), std::move(target->subtrees[target->virtual_size]));
            target->subtrees[++target->virtual_size] = left_brother->subtrees[left_brother->virtual_size];

            for (auto i = target->virtual_size - 1; i > 0; --i) {
                search_tree<tkey, tvalue>::swap(std::move(target->keys_and_values[i]), std::move(target->keys_and_values[i - 1]));
                search_tree<tkey, tvalue>::swap(std::move(target->subtrees[i + 1]), std::move(target->subtrees[i]));
            }
            allocator::destruct(left_brother->keys_and_values + --left_brother->virtual_size);

            return value;
        }

        // Если у правого брата достаточно элементов, перемещаем элемент из правого брата в текущий узел.
        if (has_right_brother && parent->subtrees[position + 1]->virtual_size > get_min_keys_count()) {
            auto *right_brother = parent->subtrees[position + 1];
            search_tree<tkey, tvalue>::swap(std::move(parent->keys_and_values[position]), std::move(right_brother->keys_and_values[0]));

            allocator::construct(target->keys_and_values + target->virtual_size, std::move(right_brother->keys_and_values[0]));
            target->subtrees[++target->virtual_size] = right_brother->subtrees[0];

            for (size_t i = 0; i < right_brother->virtual_size - 1; ++i) {
                search_tree<tkey, tvalue>::swap(std::move(right_brother->keys_and_values[i]), std::move(right_brother->keys_and_values[i + 1]));
                search_tree<tkey, tvalue>::swap(std::move(right_brother->subtrees[i]), std::move(right_brother->subtrees[i + 1]));
            }
            right_brother->subtrees[right_brother->virtual_size - 1] = right_brother->subtrees[right_brother->virtual_size];

            allocator::destruct(right_brother->keys_and_values + --right_brother->virtual_size);

            return value;
        }

        // Если ни у одного из братьев нет достаточно элементов, сливаем текущий узел с одним из братьев.
        this->merge_nodes(parent, position - (has_left_brother ? 1 : 0));
        target = parent;
    }
}


template<typename tkey, typename tvalue>
std::vector<typename associative_container<tkey, tvalue>::key_value_pair> b_tree<tkey, tvalue>::obtain_between(
    tkey const &lower_bound,
    tkey const &upper_bound,
    bool lower_bound_inclusive,
    bool upper_bound_inclusive)
{
    std::stack<std::pair<typename search_tree<tkey, tvalue>::common_node *, int>> path;
    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> range;

    int index = -1;
    if (this->_root == nullptr) {
        path.push(std::make_pair(this->_root, index));
    }

    typename search_tree<tkey, tvalue>::common_node ** iterator = &(this->_root);
    while (*iterator && index < 0) {   
        size_t left_bound = 0;
        size_t right_bound = (*iterator)->virtual_size - 1;
        
        bool found = true;
        while (true) {
            index = (left_bound + right_bound) / 2;
            auto comparison_result = this->_keys_comparer(lower_bound, (*iterator)->keys_and_values[index].key);
            if (!comparison_result) {
                if (!lower_bound_inclusive) {
                    if (index + 1 == (*iterator)->virtual_size) index = -(index + (comparison_result < 0 ? 0 : 1) + 1);
                    else index += 1;
                }
                break;
            }

            if (left_bound == right_bound) {
                found = false;
                index = -(index + (comparison_result < 0 ? 0 : 1) + 1);
                break;
            }

            if (comparison_result < 0) {
                right_bound = index;
            } else {
                left_bound = index + 1;
            }
        }

        path.push(std::make_pair(*iterator, index < 0 ? -index - 1 : index));

        if (found) {
            break;
        }

        if (index < 0) {
            iterator = (*iterator)->subtrees - index - 1;   
        } 
        
        if (!(*iterator) && this->_keys_comparer((path.top().first)->keys_and_values[index].key, upper_bound) < (lower_bound_inclusive ? 0 : 1)) {
            path = std::move(std::stack<std::pair<typename search_tree<tkey, tvalue>::common_node *, int>>());
        }
    }
    
    auto it = infix_iterator(path);
    while (it != this->end_infix() && this->_keys_comparer(upper_bound, std::get<2>(*it)) > (upper_bound_inclusive ? -1 : 0)) {
        range.push_back(std::move(typename associative_container<tkey, tvalue>::key_value_pair(std::get<2>(*it), std::get<3>(*it))));
        ++it; 
    }

    return range;
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::b_tree(
    size_t t,
    std::function<int(tkey const &, tkey const &)> keys_comparer,
    allocator *allocator,
    logger *logger) :  search_tree<tkey, tvalue>(keys_comparer, logger, allocator), _t(t)
{
    if (t < 2) {
        throw std::logic_error("Invalid value of t parameter");
    }
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::b_tree(
    b_tree<tkey, tvalue> const &other) :
    search_tree<tkey, tvalue>(other._keys_comparer, other.get_logger(), other.get_allocator()),
    _t(other._t)
{
    this->_root = copy(other._root);
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue> &b_tree<tkey, tvalue>::operator=(b_tree<tkey, tvalue> const &other)
{
    if (this == &other) {
        return *this;
    }
    clear();
    this->_logger = other.get_logger();
    this->_allocator = other.get_allocator();
    this->_root = copy(other._root);

    return *this;
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::b_tree(
    b_tree<tkey, tvalue> &&other) noexcept :
    search_tree<tkey, tvalue>(other._keys_comparer, other.get_logger(), other.get_allocator()),
    _t(other._t)
{
    other._logger = nullptr;
    other._allocator = nullptr;

    this->_root = other._root;
    other._root = nullptr;
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue> &b_tree<tkey, tvalue>::operator=(b_tree<tkey, tvalue> &&other) noexcept
{
    if (this == &other) {
        return *this;
    }
    clear();
    this->_keys_comparer = other._keys_comparer;

    this->_logger = other._logger;
    other._logger = nullptr;

    this->_allocator = other._allocator;
    other._allocator = nullptr;
    
    this->_root = other._root;
    other._root = nullptr;

    return *this;
}

template<typename tkey, typename tvalue>
void b_tree<tkey, tvalue>::clear_node(typename search_tree<tkey, tvalue>::common_node * node) noexcept
{
    if (!node) {
        return;
    }
    for (size_t i = 0; i <= node->virtual_size; ++i) {
        clear_node(node->subtrees[i]);
    }
    search_tree<tkey, tvalue>::destroy_node(node);
}

template<typename tkey, typename tvalue>
void b_tree<tkey, tvalue>::clear() noexcept
{
    clear_node(this->_root);
}

template<typename tkey, typename tvalue>
b_tree<tkey, tvalue>::~b_tree()
{
    clear();
}

template<typename tkey, typename tvalue>
typename search_tree<tkey, tvalue>::common_node *b_tree<tkey, tvalue>::copy_node(typename search_tree<tkey, tvalue>::common_node * node)
{
    if (!node) {
        return nullptr;
    }

    typename search_tree<tkey, tvalue>::common_node * copied_node = this->create_node(_t);
    copied_node->virtual_size = node->virtual_size;

    for (size_t i = 0; i < node->virtual_size; ++i) {
        allocator::construct(copied_node->keys_and_values + i, node->keys_and_values[i]);
    }
    for (size_t i = 0; i <= node->virtual_size; ++i) {
        copied_node->subtrees[i] = copy(node->subtrees[i]);
    }

    return copied_node;
}

template<typename tkey, typename tvalue>
typename search_tree<tkey, tvalue>::common_node *b_tree<tkey, tvalue>::copy()
{
    return copy_node(this->_root);
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_TEMPLATE_REPO_B_TREE_H