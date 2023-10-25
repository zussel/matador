```sql
CREATE TABLE <table> (<columns datatype>,<constraints>);
```

```sql
SELECT <columns> FROM <table> WHERE <condition> ORDER BY <order> LIMIT <limit>;

SELECT TOP <rows> <columns> FROM <table> WHERE <condition> ORDER BY <order>;
```

```sql
UPDATE <table> SET <columns=values> WHERE <condition>;
```

```sql
INSERT INTO <table> (<columns>) VALUES (<values>);
```

```sql
DELETE FROM <table> WHERE <condition>;
```

kind of columns
- name only (select, insert)
- name + type (create) -> varchar, primary/foreign keys
- name + value (update)

struct column_names_token
struct create_columns_token
struct update_columns_token
