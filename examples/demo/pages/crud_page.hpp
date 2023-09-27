#ifndef MATADOR_CRUD_PAGE_HPP
#define MATADOR_CRUD_PAGE_HPP

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/template_engine.hpp"

#include "matador/object/json_object_mapper.hpp"

#include "matador/orm/session.hpp"
#include "matador/orm/persistence.hpp"

#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

struct crud_context {
  std::string entity_singular_name;
  std::string entity_plural_name;
  std::string entity_type_name;
};

template < class EntityType >
class crud_page
{
public:
  virtual ~crud_page() = default;

protected:
  crud_page(const crud_context &ctx,
            matador::http::server &server,
            matador::persistence &p);

  virtual void prepare_json_form_data(matador::json &data) = 0;

  matador::persistence& persistence_unit() { return persistence_; }

private:
  matador::http::response list(const matador::http::request &);
  matador::http::response view(const matador::http::request &p);
  matador::http::response create(const matador::http::request &);
  matador::http::response edit(const matador::http::request &p);
  matador::http::response remove(const matador::http::request &p);

private:
  matador::logger log_;

  matador::persistence& persistence_;

  const crud_context context_;

  std::shared_ptr<matador::http::detail::template_part> list_template_;
  std::shared_ptr<matador::http::detail::template_part> details_template_;
  std::shared_ptr<matador::http::detail::template_part> create_template_;
  std::shared_ptr<matador::http::detail::template_part> edit_template_;
  std::shared_ptr<matador::http::detail::template_part> delete_template_;
};

template < class EntityType >
crud_page<EntityType>::crud_page(const crud_context &ctx,
                                 matador::http::server &server,
                                 matador::persistence &p)
  : log_(matador::create_logger(ctx.entity_plural_name))
  , persistence_(p)
  , context_(ctx)
{
  matador::file f("../templates/" + ctx.entity_type_name + "_list.matador", "r");
  auto tmpl = read_as_text(f);
  f.close();

  using namespace matador::http;
  list_template_ = template_engine::build(tmpl);

  f.open("../templates/" + ctx.entity_type_name + "_details.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  details_template_ = template_engine::build(tmpl);

  f.open("../templates/" + ctx.entity_type_name + "_create.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  create_template_ = template_engine::build(tmpl);

  f.open("../templates/" + ctx.entity_type_name + "_edit.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  edit_template_ = template_engine::build(tmpl);

  f.open("../templates/" + ctx.entity_type_name + "_delete.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  delete_template_ = template_engine::build(tmpl);

  server.on_get("/" + ctx.entity_type_name, [this](const request &req) {
    return list(req);
  });

  server.on_get("/" + ctx.entity_type_name + "/{id: \\d+}", [this](const request &req) {
    return view(req);
  });

  server.on_get("/" + ctx.entity_type_name + "/edit/{id: \\d+}", [this](const request &req) {
    return edit(req);
  });

  server.on_get("/" + ctx.entity_type_name + "/delete/{id: \\d+}", [this](const request &req) {
    return remove(req);
  });

  server.on_get("/" + ctx.entity_type_name + "/create", [this](const request &req) {
    return create(req);
  });
}

template < class EntityType >
matador::http::response crud_page<EntityType>::list(const matador::http::request &)
{
  using namespace matador;
  using namespace matador::http;

  json data;
  data["title"] = context_.entity_plural_name;

  session s(persistence_);

  auto entities = s.select<EntityType>();

  json_object_mapper mapper;

  data[context_.entity_type_name+"list"] = mapper.to_json(entities);

  return response::ok(template_engine::render(list_template_, data), mime_types::TYPE_TEXT_HTML);
}

template < class EntityType >
matador::http::response crud_page<EntityType>::view(const matador::http::request &p)
{
  using namespace matador;
  using namespace matador::http;

  json data;
  data["title"] = context_.entity_singular_name;

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto m = s.get<EntityType>(id);

  json_object_mapper mapper;

  data["title"] = context_.entity_singular_name;
  data[context_.entity_type_name] = mapper.to_json(m);

  return response::ok(template_engine::render(details_template_, data), mime_types::TYPE_TEXT_HTML);
}

template < class EntityType >
matador::http::response crud_page<EntityType>::create(const matador::http::request & /*p*/)
{
  using namespace matador;
  using namespace matador::http;

  json data;
  data["title"] = "Create " + context_.entity_singular_name;

  prepare_json_form_data(data);

  return response::ok(template_engine::render(create_template_, data), mime_types::TYPE_TEXT_HTML);
}

template < class EntityType >
matador::http::response crud_page<EntityType>::edit(const matador::http::request &p)
{
  using namespace matador;
  using namespace matador::http;

  json data;
  data["title"] = context_.entity_singular_name;

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto entity = s.get<EntityType>(id);
  json_object_mapper mapper;
  data[context_.entity_type_name] = mapper.to_json(entity);

  prepare_json_form_data(data);

  return response::ok(template_engine::render(edit_template_, data), mime_types::TYPE_TEXT_HTML);
}

template < class EntityType >
matador::http::response crud_page<EntityType>::remove(const matador::http::request &p)
{
  using namespace matador;
  using namespace matador::http;

  json data;
  data["title"] = context_.entity_singular_name;

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto m = s.get<EntityType>(id);

  json_object_mapper mapper;

  data["title"] = std::string("Delete " + context_.entity_singular_name);
  data[context_.entity_type_name] = mapper.to_json(m);

  return response::ok(template_engine::render(delete_template_, data), mime_types::TYPE_TEXT_HTML);
}

#endif //MATADOR_CRUD_PAGE_HPP
