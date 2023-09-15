#include "row.hpp"
#include "column_printer.hpp"

using namespace matador;

int main()
{
  row r;
  r.add_column(make_pk_column<long>("id"));
  r.add_column(make_column<std::string>("name","george", { 255, constraints::UNIQUE_NOT_NULL }));
  r.add_column(make_column<matador::date>("birthday", constraints::NOT_NULL));

  column_printer cp;

  r.serialize(cp);

  auto col = r.at(2);
  std::cout << "col " << col->name() << " val " << col->as<matador::date>() << "\n";
  col = r.at("id");
  std::cout << "col " << col->name() << " val " << col->as<long>() << "\n";
}