#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H

#include <binary_search_tree.h>

template<typename tkey, typename tvalue>
class AVL_tree final:
    public binary_search_tree<tkey, tvalue>
{

private:
    
    struct node final:
        binary_search_tree<tkey, tvalue>::node
    {

    public:

        unsigned char _height;

    public:
    
        explicit node(tkey const &key, tvalue const &value) : binary_search_tree<tkey, tvalue>::node(key, value), _height(1)
        {}
        explicit node(tkey const &key, tvalue &&value)  : binary_search_tree<tkey, tvalue>::node(key, value), _height(1)
        {}

        unsigned char get_height() override
        {
            return _height;
        }

        void set_height(unsigned char height) override
        {
            _height = height;
        }

    };

public:

    size_t get_node_size() const override
    {
        return sizeof(AVL_tree<tkey, tvalue>::node);
    }

    void call_node_constructor(typename binary_search_tree<tkey, tvalue>::node * tmp, tkey const &key, tvalue const &value) const override
    {
        allocator::construct(reinterpret_cast<AVL_tree<tkey, tvalue>::node*>(tmp), key, value);
    }

    void call_node_constructor(typename binary_search_tree<tkey, tvalue>::node * tmp, tkey const &key, tvalue &&value) const override
    {
        allocator::construct(reinterpret_cast<AVL_tree<tkey, tvalue>::node*>(tmp), key, std::move(value));
    }

public:
    
    struct iterator_data final: public binary_search_tree<tkey, tvalue>::iterator_data
    {
    
    public:
        
        size_t _subtree_height;
    
    public:
        
        explicit iterator_data(unsigned int depth, tkey const &key, tvalue const &value, size_t subtree_height);
        
    };

    void call_iterator_data_construct(typename binary_search_tree<tkey, tvalue>::iterator_data * tmp, 
        typename binary_search_tree<tkey, tvalue>::node * subtree_root, unsigned int depth) const override
    {
        allocator::construct(reinterpret_cast<AVL_tree<tkey, tvalue>::iterator_data*>(tmp), depth,
            subtree_root->key, subtree_root->value, reinterpret_cast<AVL_tree<tkey, tvalue>::node*>(subtree_root)->_height);
    }

    size_t get_iterator_data_size() const noexcept override
    {
        return sizeof(AVL_tree<tkey, tvalue>::iterator_data);
    }

private:
    
    class insertion_template_method final: public binary_search_tree<tkey, tvalue>::insertion_template_method
    {
    
    public:
        
        explicit insertion_template_method(
            AVL_tree<tkey, tvalue> *tree,
            typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy);

    protected:

        int get_balance_height(typename binary_search_tree<tkey, tvalue>::node * subtree_root) {
            if (!subtree_root) {
                return 0;
            }
            int left_height = 0;
            int right_height = 0;
            if (subtree_root->left_subtree) {
                left_height = subtree_root->left_subtree->get_height();
            }
            if (subtree_root->right_subtree) {
                right_height = subtree_root->right_subtree->get_height();
            }
            return left_height - right_height;
        }

        void fix_height(typename binary_search_tree<tkey, tvalue>::node * subtree_root) {
            if (!subtree_root) {
                return;
            }
            typename AVL_tree<tkey, tvalue>::node * ptr = reinterpret_cast<typename AVL_tree<tkey, tvalue>::node *>(subtree_root);
            unsigned char left_height = 0;
            unsigned char right_height = 0;
            if (ptr->left_subtree) {
                left_height = ptr->left_subtree->get_height();
            }
            if (ptr->right_subtree) {
                right_height = ptr->right_subtree->get_height();
            }
            unsigned char result = (left_height > right_height ? left_height : right_height) + 1;
            ptr->set_height(result);
        }

        void balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path) override {
            if (path.empty()) {
                return;
            }

            typename binary_search_tree<tkey, tvalue>::node ** current = path.top();
            path.pop();

            int _balance = get_balance_height(*current);

            if (_balance > 1 && get_balance_height((*current)->left_subtree) >= 0) {
                this->_tree->small_right_rotation(*current);
            }

            if (_balance > 1 && get_balance_height((*current)->left_subtree) < 0) {
                this->_tree->big_right_rotation(*current);
            }

            if (_balance < -1 && get_balance_height((*current)->right_subtree) <= 0) {
                this->_tree->small_left_rotation(*current);
            }

            if (_balance < -1 && get_balance_height((*current)->right_subtree) > 0) {
                this->_tree->big_left_rotation(*current);
            }

            if (*current) {
                fix_height((*current)->left_subtree);
                fix_height((*current)->right_subtree);
            }
            fix_height(*current);
            balance(path);
        }

        
    };
    
    class obtaining_template_method final:
        public binary_search_tree<tkey, tvalue>::obtaining_template_method
    {
    
    public:
        
        explicit obtaining_template_method(
            AVL_tree<tkey, tvalue> *tree);
        
        // TODO: think about it!
        
    };
    
    class disposal_template_method final:
        public binary_search_tree<tkey, tvalue>::disposal_template_method
    {
    
    public:
        
        explicit disposal_template_method(
            AVL_tree<tkey, tvalue> *tree,
            typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy);

    protected:

        int get_balance_height(typename binary_search_tree<tkey, tvalue>::node * subtree_root) {
            if (!subtree_root) {
                return 0;
            }
            int left_height = 0;
            int right_height = 0;
            if (subtree_root->left_subtree) {
                left_height = subtree_root->left_subtree->get_height();
            }
            if (subtree_root->right_subtree) {
                right_height = subtree_root->right_subtree->get_height();
            }
            return left_height - right_height;
        }

        void fix_height(typename binary_search_tree<tkey, tvalue>::node * subtree_root) {
            if (!subtree_root) {
                return;
            }
            typename AVL_tree<tkey, tvalue>::node * ptr = reinterpret_cast<typename AVL_tree<tkey, tvalue>::node *>(subtree_root);
            unsigned char left_height = 0;
            unsigned char right_height = 0;
            if (ptr->left_subtree) {
                left_height = ptr->left_subtree->get_height();
            }
            if (ptr->right_subtree) {
                right_height = ptr->right_subtree->get_height();
            }
            unsigned char result = (left_height > right_height ? left_height : right_height) + 1;
            ptr->set_height(result);
        }

        void balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path) override {
            if (path.empty()) {
                return;
            }

            typename binary_search_tree<tkey, tvalue>::node** current = path.top();
            path.pop();

            int _balance = get_balance_height(*current);

            if (_balance > 1 && get_balance_height((*current)->left_subtree) >= 0) {
                this->_tree->small_right_rotation(*current);
            }

            if (_balance > 1 && get_balance_height((*current)->left_subtree) < 0) {
                this->_tree->big_right_rotation(*current);
            }

            if (_balance < -1 && get_balance_height((*current)->right_subtree) <= 0) {
                this->_tree->small_left_rotation(*current);
            }

            if (_balance < -1 && get_balance_height((*current)->right_subtree) > 0) {
                this->_tree->big_left_rotation(*current);
            }

            if (*current) {
                fix_height((*current)->left_subtree);
                fix_height((*current)->right_subtree);
            }
            fix_height(*current);
            balance(path);
        }

    };

public:
    
    explicit AVL_tree(
        std::function<int(tkey const &, tkey const &)> comparer = std::less<tkey>(),
        allocator *allocator = nullptr,
        logger *logger = nullptr,
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception,
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

public:
    
    ~AVL_tree() noexcept final;
    
    AVL_tree(AVL_tree<tkey, tvalue> const &other);
    
    AVL_tree<tkey, tvalue> &operator=(AVL_tree<tkey, tvalue> const &other);
    
    AVL_tree(AVL_tree<tkey, tvalue> &&other) noexcept;
    
    AVL_tree<tkey, tvalue> &operator=(AVL_tree<tkey, tvalue> &&other) noexcept;
    
};

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::iterator_data::iterator_data(
    unsigned int depth,
    tkey const &key,
    tvalue const &value,
    size_t subtree_height):
    binary_search_tree<tkey, tvalue>::iterator_data(depth, key, value), _subtree_height(subtree_height)
{}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::insertion_template_method::insertion_template_method(
    AVL_tree<tkey, tvalue> *tree,
    typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy):
    binary_search_tree<tkey, tvalue>::insertion_template_method(tree, insertion_strategy)
{}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::obtaining_template_method::obtaining_template_method(
    AVL_tree<tkey, tvalue> *tree):
    binary_search_tree<tkey, tvalue>::obtaining_template_method(tree)
{}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::disposal_template_method::disposal_template_method(
    AVL_tree<tkey, tvalue> *tree,
    typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>::disposal_template_method(tree, disposal_strategy)
{}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::AVL_tree(
    std::function<int(tkey const &, tkey const &)> comparer,
    allocator *allocator,
    logger *logger,
    typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy,
    typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>::template binary_search_tree<tkey, tvalue>(
        new typename AVL_tree<tkey, tvalue>::insertion_template_method(this, insertion_strategy),
        new typename AVL_tree<tkey, tvalue>::obtaining_template_method(this), 
        new typename AVL_tree<tkey, tvalue>::disposal_template_method(this, disposal_strategy), 
        comparer, allocator, logger)
{
    if (logger) {
        logger->debug("[AVL - tree] [Start] constructor started\n");
        logger->debug("[AVL - tree] [Finish] constructor finished\n");
    }
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::~AVL_tree() noexcept
{
    this->debug_with_guard("[AVL - tree] Destructor\n");
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::AVL_tree(
    AVL_tree<tkey, tvalue> const &other)
{
    this->debug_with_guard("[AVL - tree] [Start] copy constructor started\n");

    if (this->_root != nullptr) {
        clear(this->_root);
    }
    this->_root = copy(other._root);

    if (this->_insertion_template != nullptr) {
        delete this->_insertion_template;
    }
    this->_insertion_template = new AVL_tree<tkey, tvalue>::insertion_template_method(*other._insertion_template);
    if (this->_disposal_template != nullptr) {
        delete this->_disposal_template;
    }
    this->_disposal_template = new AVL_tree<tkey, tvalue>::disposal_template_method(*other._disposal_template);
    if (this->_obtaining_template != nullptr) {
        delete this->_obtaining_template;
    }
    this->_obtaining_template = new AVL_tree<tkey, tvalue>::obtaining_template_method(*other._obtaining_template);

    this->debug_with_guard("[AVL - tree] [Finish] copy constructor finished\n");
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue> &AVL_tree<tkey, tvalue>::operator=(
    AVL_tree<tkey, tvalue> const &other)
{
    this->debug_with_guard("[AVL - tree] [Start] operator= started\n");
    if (this == &other)
    {
        this->debug_with_guard("[AVL - tree] [Finish] operator= finished\n");
        return *this;
    }
    if (this->_root != nullptr) {
        clear(this->_root);
    }
    this->_root = copy(other._root);

    if (this->_insertion_template != nullptr) {
        delete this->_insertion_template;
    }
    this->_insertion_template = new AVL_tree<tkey, tvalue>::insertion_template_method(*other._insertion_template);
    if (this->_disposal_template != nullptr) {
        delete this->_disposal_template;
    }
    this->_disposal_template = new AVL_tree<tkey, tvalue>::disposal_template_method(*other._disposal_template);
    if (this->_obtaining_template != nullptr) {
        delete this->_obtaining_template;
    }
    this->_obtaining_template = new AVL_tree<tkey, tvalue>::obtaining_template_method(*other._obtaining_template);

    this->debug_with_guard("[AVL - tree] [Finish] operator= finished\n");
    return *this;
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::AVL_tree(
    AVL_tree<tkey, tvalue> &&other) noexcept
{
    this->debug_with_guard("[AVL - tree] [Start] move constructor started\n");
    if (this->_root != nullptr) {
        this->clear(this->_root);
    }
    this->_root = std::exchange(other._root, nullptr);

    if (this->_insertion_template != nullptr) {
        delete this->_insertion_template;
    }
    this->_insertion_template = std::exchange(other._insertion_template, nullptr);
    if (this->_disposal_template != nullptr) {
        delete this->_disposal_template;
    }
    this->_disposal_template = std::exchange(other._disposal_template, nullptr);
    if (this->_obtaining_template != nullptr) {
        delete this->_obtaining_template;
    }
    this->_obtaining_template = std::exchange(other._obtaining_template, nullptr);

    this->debug_with_guard("[AVL - tree] [Finish] move constructor finished\n");
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue> &AVL_tree<tkey, tvalue>::operator=(
    AVL_tree<tkey, tvalue> &&other) noexcept
{
    this->debug_with_guard("[AVL - tree] [Start] move operator started\n");
    if (this == &other)
    {
        this->debug_with_guard("[AVL - tree] [Finish] move operator finished\n");
        return *this;
    }
    if (this->_root != nullptr) {
        clear(this->_root);
    }
    this->_root = std::exchange(other._root, nullptr);

    if (this->_insertion_template != nullptr) {
        delete this->_insertion_template;
    }
    this->_insertion_template = std::exchange(other._insertion_template, nullptr);
    if (this->_disposal_template != nullptr) {
        delete this->_disposal_template;
    }
    this->_disposal_template = std::exchange(other._disposal_template, nullptr);
    if (this->_obtaining_template != nullptr) {
        delete this->_obtaining_template;
    }
    this->_obtaining_template = std::exchange(other._obtaining_template, nullptr);
    this->debug_with_guard("[AVL - tree] [Finish] move operator finished\n");
    return *this;
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H