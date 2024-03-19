#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>

// 定义节点
template <typename K, typename V>
class Node
{
public:
    Node() {}
    Node(K k, V v, int);
    ~Node();
    K get_key() const;
    V get_value() const;
    void set_value(V);
    Node<K, V> **forward;
    int node_level;

private:
    K key;
    V value;
};

// 类拥有的构造函数
template <typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level)
{
    this->key = k;
    this->value = v;
    this->node_level = level;
    this->forward = new Node<K, V> *[level + 1];
    memset(this->forward, 0, sizeof(Node<K, V> *) * (level + 1));
};

// 类拥有的析构函数
template <typename K, typename V>
Node<K, V>::~Node()
{
    delete[] forward;
}

// 获取key成员函数
template <typename K, typename V>
K Node<K, V>::get_key() const
{
    return key;
}

// 获取value成员函数
template <typename K, typename V>
V Node<K, V>::get_value() const
{
    return value;
}

// 设置value成员函数
template <typename K, typename V>
void Node<K, V>::set_value(V v)
{
    this->value = v;
}

template <typename K, typename V>
class SkipList
{
public:
    SkipList(int);                      // 构造函数
    ~SkipList();                        // 析构函数
    int get_random_level();             // 获取节点的随机层级
    Node<K, V> *create_node(K, V, int); // 创建节点
    int insert_element(K, V);           // 插入节点
    void display_list();                // 展示节点
    bool search_element(K);             // 搜索节点
    bool delete_element(K);             // 删除节点
    void dump_file();                   // 持久化数据到文件
    void load_file();                   // 从文件加载数据
    void clear(Node<K, V> *);           // 递归删除节点
    int size();                         // 跳表中的节点个数

private:
    int _max_level;       // 最大层级
    int _skip_list_level; // 跳表当前层级
    Node<K, V> *_header;  // 头节点
    int _element_count;   // 跳表中的节点个数
};

// 跳表的构造函数
template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level)
{
    this->_max_level = max_level;                     // 设置最大层级
    this->_skip_list_level = 0;                       // 初始化跳表层级为0
    this->_element_count = 0;                         // 初始化跳表节点个数为0
    K k;                                              // 默认值
    V v;                                              // 默认值
    this->_header = new Node<K, V>(k, v, _max_level); // 创建头节点
}

/**
 * 创建一个新节点
 * @param k 节点的键
 * @param v 节点的值
 * @param level 节点的层级
 * @return 新创建的节点指针
 */
template <typename K, typename V>
Node<K, V> *SkipList<K, V>::create_node(const K k, const V v, int level)
{
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

/**
 * 搜索指定的键值是否存在于跳表中。
 * @param key 待查找的键值
 * @return 如果找到键值，返回 true；否则返回 false。
 */
template <typename K, typename V>
bool SkipList<K, V>::search_element(const K key)
{
    // 定义一个指针 current，初始化为跳表的头节点 _header
    Node<K, V> *current = _header;
    // 从跳表的最高层开始搜索
    for (int i = _skip_list_level; i >= 0; i--)
    {
        // 遍历当前层级，直到下一个节点的键值大于或等于待查找的键值
        while (current->forward[i] != nullptr && current->forward[i]->key < key)
        {
            // 移动到当前层的下一个节点
            current = current->forward[i];
        }
        // 当前节点的下一个节点的键值大于待查找的键值时，进行下沉到下一层
        // 下沉操作通过循环的 i-- 实现
    }
    // 检查当前层（最底层）的下一个节点的键值是否为待查找的键值
    current = current->forward[0];
    if (current != nullptr && current->key == key)
    {
        // 如果找到匹配的键值，返回 true
        return true;
    }
    // 如果没有找到匹配的键值，返回 false
    return false;
}

/**
 * 在跳表中插入一个新元素。
 * @param key 待插入节点的 key
 * @param value 待插入节点的 value
 * @return 如果元素已存在，返回 1；否则，进行更新 value 操作并返回 0。
 */
template <typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value)
{
    // 定义一个指针 current，初始化为跳表的头节点 _header
    Node<K, V> *current = this->_header;
    // 用于在各层更新指针的数组
    Node<K, V> *update[_max_level + 1];
    // 用于记录每层中待更新指针的节点
    memset(update, 0, sizeof(Node<K, V> *) * (_max_level + 1));
    // 从跳表的最高层开始搜索
    for (int i = _skip_list_level; i >= 0; i--)
    {
        // 遍历当前层级，直到下一个节点的键值大于或等于待插入的键值
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key)
        {
            // 移动到当前层的下一个节点
            current = current->forward[i];
        }
        // 保存每层中该节点，以便后续插入时更新指针
        update[i] = current;
    }
    // 移动到最底层的下一个节点，准备插入操作
    current = current->forward[0];
    // // 检查待插入的节点的键是否已存在
    if (current != nullptr && current->key == key)
    {
        // 键已存在，取消插入
        return 1;
    }
    if (current == NULL || current->get_key() != key)
    {
        // 通过随机函数决定新节点的层级高度
        int random_level = get_random_level();
        if (random_level > _skip_list_level)
        {
            // 更新跳表的层级高度
            for (int i = _skip_list_level + 1; i <= random_level; i++)
            {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }
        // 创建新节点
        Node<K, V> *insert_node = create_node(key, value, random_level);
        // 更新各层指针
        for (int i = 0; i <= random_level; i++)
        {
            insert_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = insert_node;
        }
        _element_count++;
    }

    return 0;
}

// 析构函数
template <typename K, typename V>
SkipList<K, V>::~SkipList()
{
    clear(_header);
}

// 随机层级的选择
template <typename K, typename V>
int SkipList<K, V>::get_random_level()
{
    // 初始化层级：每个节点至少出现在第一层
    int k = 1;
    // 随机层数增加：使用rand实现抛硬币效果，决定是否升层
    while (rand() % 2)
    {
        k++;
    }
    // 层级限制：确保节点层级不超过最大值 _max_level
    k = (k < _max_level) ? k : _max_level;
    // 返回层级：返回确定的层级值，决定节点插入的层
    return k;
}

