
#ifdef __cplusplus
extern "C" {
#endif

struct bds_list;
struct bds_list_node;

/* void bds_list_ctor(struct bds_list *list, size_t object_len, int (*object_compar)(const void *, const void *), */
/*                    void (*object_dtor)(void *)); */

/* void bds_list_dtor(struct bds_list *list); */

struct bds_list *bds_list_alloc(size_t object_len, int (*object_compar)(const void *, const void *),
                                void (*object_dtor)(void *));

void bds_list_free(struct bds_list **list);

// static inline struct bds_list_node *bds_list_begin(struct bds_list *list) { return list->head; }
// static inline struct bds_list_node *bds_list_end() { return NULL; }
// static inline struct bds_list_node *bds_list_iterate(struct bds_list_node *node) { return node->next; }

struct bds_list_node *bds_list_begin(struct bds_list *list);
static inline struct bds_list_node *bds_list_end() { return NULL; }
struct bds_list_node *bds_list_iterate(struct bds_list_node *node);

// static inline void *bds_list_object(struct bds_list_node *node) { return node->object; }
void *bds_list_object(struct bds_list_node *node);

void bds_list_print(const struct bds_list *list, void (*print_object)(const void *));

void bds_list_append(struct bds_list *list, const void *object);

int bds_list_remove(struct bds_list *list, const void *key);

int bds_list_insert_after(struct bds_list *list, const void *object, const void *key);

int bds_list_insert_before(struct bds_list *list, const void *object, const void *key);

int bds_list_insert_sort(struct bds_list *list, const void *object);

#ifdef __cplusplus
}
#endif
