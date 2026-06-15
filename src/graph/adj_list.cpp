/*
 * 模块名称  : 图存储——邻接表实现
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : AdjList 成员函数实现（基于邻接表）
 */

#include "adj_list.h"
#include <iostream>
#include <cstring>

// 假设基类或公共头定义了 SUCCESS / FAILURE
#ifndef SUCCESS
#define SUCCESS 0
#define FAILURE -1
#endif

// ==================== 构造/析构 ====================

AdjList::AdjList(int max_vertices, GraphType graph_type)
    : heads(nullptr), vertex_count(0), max_vertices(max_vertices),
      edge_count(0), graph_type(graph_type),
      find_comparisons(0), edge_query_count(0)
{
    heads = new AdjListHead_t[max_vertices]();
    // 初始化所有头结点为无效状态
    for (int i = 0; i < max_vertices; ++i) {
        heads[i].valid = false;
        heads[i].first_edge = nullptr;
    }
}

AdjList::~AdjList()
{
    for (int i = 0; i < max_vertices; ++i) {
        free_edge_list(i);
    }
    delete[] heads;
    heads = nullptr;
}

// ==================== 存储标识 ====================

StorageType AdjList::get_storage_type() const
{
    return STORAGE_LIST;
}

const char *AdjList::get_storage_name() const
{
    return "邻接表";
}

// ==================== 图属性查询 ====================

int AdjList::get_vertex_count() const
{
    return vertex_count;
}

int AdjList::get_edge_count() const
{
    return edge_count;
}

GraphType AdjList::get_graph_type() const
{
    return graph_type;
}

int AdjList::get_max_vertex_count() const
{
    return max_vertices;
}

// ==================== 顶点操作 ====================

int AdjList::add_vertex(const City_t &city)
{
    // 检查容量
    if (vertex_count >= max_vertices)
        return FAILURE;

    // 检查重复
    if (find_index(city.id) != -1)
        return FAILURE;

    // 寻找第一个空位
    int idx = -1;
    for (int i = 0; i < max_vertices; ++i) {
        if (!heads[i].valid) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return FAILURE; // 理论上不应发生

    heads[idx].data = city;
    heads[idx].valid = true;
    heads[idx].first_edge = nullptr;
    vertex_count++;
    return SUCCESS;
}

int AdjList::remove_vertex(int city_id)
{
    int idx = find_index(city_id);
    if (idx == -1)
        return FAILURE;

    // 1. 删除该顶点的所有出边（自己的边链表）
    free_edge_list(idx);

    // 2. 删除其他顶点指向该顶点的入边（无向图也需处理）
    for (int i = 0; i < max_vertices; ++i) {
        if (!heads[i].valid || i == idx) continue;
        EdgeNode_t *prev = nullptr;
        EdgeNode_t *curr = heads[i].first_edge;
        while (curr != nullptr) {
            if (curr->dest_city == city_id) {
                // 删除当前边结点
                if (prev == nullptr) {
                    heads[i].first_edge = curr->next;
                } else {
                    prev->next = curr->next;
                }
                EdgeNode_t *tmp = curr;
                curr = curr->next;
                delete tmp;
                edge_count--;
                // 如果是有向图，只删除这一条入边即可
                // 无向图的反向边已在 free_edge_list 中删除，此处不用重复处理
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }

    // 标记顶点无效
    heads[idx].valid = false;
    vertex_count--;
    return SUCCESS;
}

bool AdjList::has_vertex(int city_id) const
{
    return find_index(city_id) != -1;
}

int AdjList::get_vertex(int city_id, City_t *out_city) const
{
    int idx = find_index(city_id);
    if (idx == -1 || out_city == nullptr)
        return FAILURE;

    *out_city = heads[idx].data;
    return SUCCESS;
}

// ==================== 边操作 ====================

int AdjList::add_edge(int from, int to, int weight)
{
    int from_idx = find_index(from);
    int to_idx = find_index(to);
    if (from_idx == -1 || to_idx == -1)
        return FAILURE;

    // 查重
    if (find_edge_node(from, to) != nullptr)
        return FAILURE;

    // 头插法插入边 from -> to
    EdgeNode_t *new_edge = create_edge_node(to, weight);
    new_edge->next = heads[from_idx].first_edge;
    heads[from_idx].first_edge = new_edge;
    edge_count++;

    // 无向图：插入反向边
    if (graph_type == GRAPH_UNDIRECTED) {
        EdgeNode_t *rev_edge = create_edge_node(from, weight);
        rev_edge->next = heads[to_idx].first_edge;
        heads[to_idx].first_edge = rev_edge;
        edge_count++;
    }

    return SUCCESS;
}

int AdjList::remove_edge(int from, int to)
{
    int from_idx = find_index(from);
    int to_idx = find_index(to);
    if (from_idx == -1 || to_idx == -1)
        return FAILURE;

    // 删除 from -> to
    EdgeNode_t *prev = nullptr;
    EdgeNode_t *curr = heads[from_idx].first_edge;
    bool found = false;
    while (curr != nullptr) {
        if (curr->dest_city == to) {
            if (prev == nullptr) {
                heads[from_idx].first_edge = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            edge_count--;
            found = true;
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) return FAILURE;

    // 无向图：删除反向边
    if (graph_type == GRAPH_UNDIRECTED) {
        prev = nullptr;
        curr = heads[to_idx].first_edge;
        while (curr != nullptr) {
            if (curr->dest_city == from) {
                if (prev == nullptr) {
                    heads[to_idx].first_edge = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                edge_count--;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    return SUCCESS;
}

int AdjList::update_edge_weight(int from, int to, int new_weight)
{
    EdgeNode_t *edge = find_edge_node(from, to);
    if (edge == nullptr)
        return FAILURE;

    edge->weight = new_weight;

    // 无向图更新反向边权值
    if (graph_type == GRAPH_UNDIRECTED) {
        EdgeNode_t *rev_edge = find_edge_node(to, from);
        if (rev_edge != nullptr) {
            rev_edge->weight = new_weight;
        }
    }
    return SUCCESS;
}

int AdjList::get_edge_weight(int from, int to, int *out_weight) const
{
    EdgeNode_t *edge = find_edge_node(from, to);
    if (edge == nullptr || out_weight == nullptr)
        return FAILURE;

    *out_weight = edge->weight;
    return SUCCESS;
}

bool AdjList::has_edge(int from, int to) const
{
    return find_edge_node(from, to) != nullptr;
}

// ==================== 遍历支持 ====================

int AdjList::get_neighbors(int vertex_id,
                          Edge_t **out_neighbors,
                          int *out_count) const
{
    int idx = find_index(vertex_id);
    if (idx == -1 || out_neighbors == nullptr || out_count == nullptr)
        return FAILURE;

    // 统计邻居数量
    int count = 0;
    EdgeNode_t *curr = heads[idx].first_edge;
    while (curr != nullptr) {
        count++;
        curr = curr->next;
    }

    if (count == 0) {
        *out_neighbors = nullptr;
        *out_count = 0;
        return SUCCESS;
    }

    Edge_t *neighbors = new Edge_t[count];
    curr = heads[idx].first_edge;
    int pos = 0;
    while (curr != nullptr) {
        neighbors[pos].dest_city_id = curr->dest_city;
        neighbors[pos].weight = curr->weight;
        pos++;
        curr = curr->next;
    }

    *out_neighbors = neighbors;
    *out_count = count;
    return SUCCESS;
}

int AdjList::get_all_vertex_ids(int **out_ids, int *out_count) const
{
    if (out_ids == nullptr || out_count == nullptr)
        return FAILURE;

    *out_ids = nullptr;
    *out_count = 0;

    if (vertex_count == 0) return SUCCESS;

    int *ids = new int[vertex_count];
    int pos = 0;
    for (int i = 0; i < max_vertices; ++i) {
        if (heads[i].valid) {
            ids[pos++] = heads[i].data.id;
        }
    }

    *out_ids = ids;
    *out_count = vertex_count;
    return SUCCESS;
}

// ==================== 性能统计 ====================

void AdjList::get_performance_stats(PerfStats_t *out_stats) const
{
    if (out_stats == nullptr) return;

    // 头结点数组大小
    int head_mem = max_vertices * sizeof(AdjListHead_t);
    // 边结点内存（遍历所有有效顶点的边链表）
    int edge_mem = 0;
    for (int i = 0; i < max_vertices; ++i) {
        if (!heads[i].valid) continue;
        EdgeNode_t *curr = heads[i].first_edge;
        while (curr != nullptr) {
            edge_mem += sizeof(EdgeNode_t);
            curr = curr->next;
        }
    }
    out_stats->memory_bytes = head_mem + edge_mem;
    out_stats->find_comparisons = find_comparisons;
    out_stats->edge_query_count = edge_query_count;
    out_stats->traverse_time_us = 0;   // 可后续填充
}

void AdjList::reset_perf_counters()
{
    find_comparisons = 0;
    edge_query_count = 0;
}

// ==================== 调试输出 ====================

void AdjList::print_graph() const
{
    std::cout << "=== 邻接表 ===" << std::endl;
    std::cout << "顶点数: " << vertex_count << "  边数: " << edge_count << std::endl;

    for (int i = 0; i < max_vertices; ++i) {
        if (!heads[i].valid) continue;
        std::cout << "[" << heads[i].data.id << "]: ";
        EdgeNode_t *curr = heads[i].first_edge;
        if (curr == nullptr) {
            std::cout << "(无邻接点)";
        } else {
            while (curr != nullptr) {
                std::cout << "-> (" << curr->dest_city << ", w:" << curr->weight << ") ";
                curr = curr->next;
            }
        }
        std::cout << std::endl;
    }
    std::cout << "==================" << std::endl;
}

// ==================== 私有方法 ====================

int AdjList::find_index(int city_id) const
{
    find_comparisons++;
    for (int i = 0; i < max_vertices; ++i) {
        if (heads[i].valid && heads[i].data.id == city_id) {
            return i;
        }
    }
    return -1;
}

EdgeNode_t *AdjList::find_edge_node(int from, int to) const
{
    edge_query_count++;
    int from_idx = find_index(from);
    if (from_idx == -1) return nullptr;

    EdgeNode_t *curr = heads[from_idx].first_edge;
    while (curr != nullptr) {
        if (curr->dest_city == to) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

void AdjList::free_edge_list(int index)
{
    if (index < 0 || index >= max_vertices) return;
    EdgeNode_t *curr = heads[index].first_edge;
    while (curr != nullptr) {
        EdgeNode_t *tmp = curr;
        curr = curr->next;
        delete tmp;
        // 注意：这里减少 edge_count 可能不准确，因为外部调用时 edge_count 需要手动管理
        // 为避免重复计数，在 remove_vertex 中单独处理边计数
    }
    heads[index].first_edge = nullptr;
}

EdgeNode_t *AdjList::create_edge_node(int dest_city, int weight) const
{
    EdgeNode_t *node = new EdgeNode_t;
    node->dest_city = dest_city;
    node->weight = weight;
    node->next = nullptr;
    return node;
}