/*
 * 模块名称  : 图存储——邻接表实现
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 基于链表的图存储实现，实现 GraphBase 接口。
 *             稀疏图场景内存更优，遍历邻接点更高效。
 */

#ifndef ADJ_LIST_H
#define ADJ_LIST_H

#include "graph_base.h"

/**
 * @class AdjList
 * @brief 邻接表存储的图实现
 *
 * 内部使用头结点数组 + 链表存储边。
 * - m_p_heads[i]            第 i 个顶点的头结点
 * - m_p_heads[i].p_first_edge 指向该顶点第一条邻接边的链表
 *
 * 无向图：添加边 from→to 时同步添加 to→from
 */
class AdjList : public GraphBase
{
private:
    AdjListHead_t *m_p_heads; /* 头结点数组 */
    int m_vertex_count;       /* 当前顶点数 */
    int m_max_vertices;       /* 最大顶点容量 */
    int m_edge_count;         /* 当前边数 */
    GraphType m_graph_type;   /* 图类型 */

    /* 性能统计 */
    mutable int m_find_comparisons;
    mutable int m_edge_query_count;

public:
    /* ========== 构造/析构 ========== */
    AdjList(int max_vertices, GraphType graph_type);
    virtual ~AdjList();

    /* ========== 存储标识 ========== */
    virtual StorageType get_storage_type() const override;
    virtual const char *get_storage_name() const override;

    /* ========== 图属性查询 ========== */
    virtual int get_vertex_count() const override;
    virtual int get_edge_count() const override;
    virtual GraphType get_graph_type() const override;
    virtual int get_max_vertex_count() const override;

    /* ========== 顶点操作 ========== */
    virtual int add_vertex(const City_t &city) override;
    virtual int remove_vertex(int city_id) override;
    virtual bool has_vertex(int city_id) const override;
    virtual int get_vertex(int city_id, City_t *p_out_city) const override;

    /* ========== 边操作 ========== */
    virtual int add_edge(int from, int to, int weight) override;
    virtual int remove_edge(int from, int to) override;
    virtual int update_edge_weight(int from, int to, int new_weight) override;
    virtual int get_edge_weight(int from, int to, int *p_out_weight) const override;
    virtual bool has_edge(int from, int to) const override;

    /* ========== 遍历支持 ========== */
    virtual int get_neighbors(int vertex_id,
                             Edge_t **p_out_neighbors,
                             int *p_out_count) const override;
    virtual int get_all_vertex_ids(int **p_out_ids, int *p_out_count) const override;

    /* ========== 性能统计 ========== */
    virtual void get_performance_stats(PerfStats_t *p_out_stats) const override;
    virtual void reset_perf_counters() override;

    /* ========== 调试输出 ========== */
    virtual void print_graph() const override;

private:
    /** @brief 查找顶点对应的内部索引 */
    int find_index(int city_id) const;

    /** @brief 在链表中查找边 */
    EdgeNode_t *find_edge_node(int from, int to) const;

    /** @brief 释放指定顶点的所有邻接边链表 */
    void free_edge_list(int index);

    /** @brief 创建一个新的边结点 */
    EdgeNode_t *create_edge_node(int dest_city, int weight) const;
};

#endif /* ADJ_LIST_H */
