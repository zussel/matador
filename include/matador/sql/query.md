```sql_context
CREATE TABLE <table> (<columns datatype>,<constraints>);
```

```sql_context
SELECT <columns> FROM <table> WHERE <condition> ORDER BY <order> LIMIT <limit>;

SELECT TOP <rows> <columns> FROM <table> WHERE <condition> ORDER BY <order>;
```

```sql_context
UPDATE <table> SET <columns=values> WHERE <condition>;
```

```sql_context
INSERT INTO <table> (<columns>) VALUES (<values>);
```

```sql_context
DELETE FROM <table> WHERE <condition>;
```

kind of columns
- name only (select, insert)
- name + type (create) -> varchar, primary/foreign keys
- name + value (update)
