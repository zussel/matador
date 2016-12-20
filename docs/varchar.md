---
title: Varchar
---
{% include base_path %}

The ```varchar<T>``` class represents a VARCHAR database data type. Internally it contains
a ```std::string```. The template parameter takes the size of the varchar. If the assigned
string is exceeds the size, the string is truncated on database.

### Creation
