#ifndef MATADOR_MODEL_METAS_HPP
#define MATADOR_MODEL_METAS_HPP

#include "matador/query/meta_table_macro.hpp"

META_TABLE(recipes, RECIPE, id, name);
META_TABLE(ingredients, INGREDIENT, id, name);
META_TABLE(recipe_ingredients, RECIPE_INGREDIENT, recipe_id, ingredient_id);
META_TABLE(airplanes, AIRPLANE, id, brand, model);
META_TABLE(flights, FLIGHT, id, airplane_id, pilot_name);
META_TABLE(shipments, SHIPMENT, id, tracking_number)
META_TABLE(packages, PACKAGE, id, weight, shipment_id)
META_TABLE(persons, PERSON, id, name, age, image)
META_TABLE(departments, DEPARTMENT, id, name)
META_TABLE(employees, EMPLOYEE, id, first_name, last_name, dep_id)
META_TABLE(authors, AUTHOR, id, first_name, last_name, date_of_birth, year_of_birth, distinguished)
META_TABLE(books, BOOK, id, title, author_id, published_in)
META_TABLE(orders, ORDER, order_id, order_date, required_date, shipped_date, ship_via, freight, ship_name, ship_address, ship_city, ship_region, ship_postal_code, ship_country)
META_TABLE(courses, COURSE, id, title)
META_TABLE(users, USER, id, name, password, session_id)
META_TABLE(user_sessions, USER_SESSION, id, session_token, user_id)

#endif //MATADOR_MODEL_METAS_HPP