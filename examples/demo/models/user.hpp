#ifndef MATADOR_USER_HPP
#define MATADOR_USER_HPP


struct user
{
  long id {};
  std::string username {};
  std::string password {};
  std::string first_name {};
  std::string last_name {};

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("username", username);
    serializer.serialize("password", password);
    serializer.serialize("first_name", first_name);
    serializer.serialize("last_name", last_name);
  }
};


#endif //MATADOR_USER_HPP
