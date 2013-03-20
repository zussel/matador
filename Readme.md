Open Object Store (OOS)
=======================

Store all kind of objects in one container.

OOS is a ORM framework written in C++. I aims to encapsulate all the database work (database backends, sql statements, serialization of objects) and provide the user an easy to use interface and a unique container for all objects. Given this container the user has a centralized storage point for all objects at hand but with the ability to create views on concrete object types.

Features:

 * Encapsulate database backends
 * Encapsulate sql statements
 * One to one/many relations
 * One storage container
 * Filter with simple expressions
 * Reference counting mechanism
 * STL like interface and iterators
