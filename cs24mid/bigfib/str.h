/* A growable string-buffer type. */
typedef struct str_t {
    /* The actual size of the memory that psz points to. */
    int capacity;

    /* The length of the string stored in psz, not including trailing '\0'. */
    int length;

    /* A pointer to the memory for the string, or NULL if empty. */
    char *psz;
} str_t;

void init_str(str_t *s);
void clear_str(str_t *s);

void append_char(str_t *s, char ch);
void reverse_str(str_t *s);

