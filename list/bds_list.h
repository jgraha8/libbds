
#ifdef __cplusplus
extern "C" {
#endif 

struct list_node {
        void *v;
        struct list_node *next;
};
	
struct bds_list {
        size_t object_len;
        int (*object_compar)(const void *, const void *);
        void (*object_dtor)(void *);
        struct list_node *head;
};

void bds_list_ctor(struct bds_list *list, size_t object_len, int (*object_compar)(const void *, const void *),
                   void (*object_dtor)(void *));

void bds_list_dtor(struct bds_list *list);

struct bds_list *bds_list_alloc(size_t object_len, int (*object_compar)(const void *, const void *),
                                void (*object_dtor)(void *));

void bds_list_free(struct bds_list **list);

void bds_list_append(struct bds_list *list, const void *v);

int bds_list_remove(struct bds_list *list, const void *key);

int bds_list_insert_after(struct bds_list *list, const void *v, const void *key);

int bds_list_insert_before(struct bds_list *list, const void *v, const void *key);

int bds_list_insert_sort(struct bds_list *list, const void *v);
	
#ifdef __cplusplus
}
#endif 
