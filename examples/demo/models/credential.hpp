#ifndef MATADOR_CREDENTIAL_HPP
#define MATADOR_CREDENTIAL_HPP

struct credential
{
  std::string username {};
  std::string password {};

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("username", username);
    serializer.serialize("password", password);
  }
};

#endif //MATADOR_CREDENTIAL_HPP
